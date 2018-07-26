//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32f4xx_hal.h"
#include "Timer.h"
#include "BlinkLed.h"
#include "PlayMP3.h"
#include "cortexm/ExceptionHandlers.h"
#include "generic.h"
#include "timeKeeping.h"
#include "DebugPort.h"
#include "AudioChip.h"

//
// Disable specific warnings
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"



// ----------------------------------------------------------------------------
//
// Standalone STM32F4 Simple Alarm Clock Stub Code
//
// This code just plays an MP3 file off of a connected USB flash drive.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

void
//	Display7Segment( void ),
//	DisplayClockMode( void ),
//	ConvertTo24Hour( void ),
//	SetTime( void ),
//	SetAlarm( void ),
//	Snooze( void ),
//	ProcessButtons( void ),
//	GetCurrentTime( void ),
	SystemClock_Config( void ),
	MX_GPIO_Init( void ),
	MX_I2C1_Init( void ),
	MX_USB_HOST_Process( void );

//uint16_t
//	CheckButtons( void );


// STMCube Example declarations.
// static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

static void
	MSC_Application(void);

//static void
	//Error_Handler(void);

//
// Global variables
//

//RTC_InitTypeDef
//	ClockInit;				// Structure used to initialize the real time clock
//
RTC_TimeTypeDef
	ClockTime;				// Structure to hold/store the current time

RTC_DateTypeDef
	ClockDate;				// Structure to hold the current date

RTC_AlarmTypeDef
	ClockAlarm;				// Structure to hold/store the current alarm time

TIM_HandleTypeDef
	Timer6_44Khz,			// Structure for the audio play back timer subsystem
	DisplayTimer;			// Structure for the LED display timer subsystem

DAC_HandleTypeDef
	AudioDac;				// Structure for the audio digital to analog converter subsystem

DMA_HandleTypeDef
	AudioDma;				// Structure for the audio DMA direct memory access controller subsystem

RTC_HandleTypeDef
	RealTimeClock;			// Structure for the real time clock subsystem

I2C_HandleTypeDef			// Structure for I2C subsystem. Used for external audio chip
	I2c;

volatile int
	DisplayClockModeCount,	// Number of display ticks to show the current clock mode time format
	PlayMusic = FALSE,		// Flag indicating if music should be played
	DebounceCount = 0;		// Buttons debounce count

volatile uint16_t
	ButtonsPushed;			// Bit field containing the bits of which buttons have been pushed

FATFS
	UsbDiskFatFs;			// File system object for USB disk logical drive

char
	UsbDiskPath[4];			// USB Host logical drive path

int
	BcdTime[4],				// Array to hold the hours and minutes in BCD format
	DisplayedDigit = 0,		// Current digit being displayed on the LED display

							// Current format for the displayed time ( IE 12 or 24 hour format )
	ClockHourFormat = CLOCK_HOUR_FORMAT_12,
	AlarmPmFlag = 0,
	TimePmFlag = 0;


//
// Functions required for long files names on fat32 partitions
//

WCHAR ff_convert (WCHAR wch, UINT dir)
{
	if (wch < 0x80)
	{
//
// ASCII Char
//
		return wch;
	}

//
// unicode not supported
//
	return 0;
}

WCHAR ff_wtoupper (WCHAR wch)
{
	if (wch < 0x80)
	{
//
// ASCII Char
//
		if (wch >= 'a' && wch <= 'z')
		{
			wch &= ~0x20;
		}

		return wch;
	}

//
// unicode not supported
//
	return 0;
}









//
// Dummy interrupt handler function
//
void TIM6_DAC_IRQHandler(void)
{
	HAL_NVIC_DisableIRQ( TIM6_DAC_IRQn );
}



/*
 * Function: ConfigureAudioDma
 *
 * Description:
 *
 * Initialize DMA, DAC and timer 6 controllers for a mono channel audio to be played on PA4
 *
 */

void ConfigureAudioDma( void )
{

	TIM_MasterConfigTypeDef
		Timer6MasterConfigSync;

	GPIO_InitTypeDef
		GPIO_InitStructure;

	DAC_ChannelConfTypeDef
		DacConfig;

//
// If we have the timer 6 interrupt enabled then disable the timer from running when we halt the processor or hit a breakpoint.
// This also applies to printing using the semihosting method which also uses breakpoints to transfer data to the host computer
//


	__HAL_DBGMCU_UNFREEZE_TIM5();

//
// Enable the clocks for GPIOA, GPIOC and Timer 6
//
	__HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();


//
// Configure PA4 as an analog output ( used for D/A output of the analog signal )
//

	GPIO_InitStructure.Pin = GPIO_PIN_4;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStructure.Alternate = 0;
	HAL_GPIO_Init( GPIOA, &GPIO_InitStructure);




//
// Configure timer 6 for a clock frequency of 44Khz and a triggered output for the DAC
//
	Timer6_44Khz.Instance = TIM6;
	Timer6_44Khz.Init.Prescaler = 20; //this value may have to be changed
	Timer6_44Khz.Init.CounterMode = TIM_COUNTERMODE_UP;
	Timer6_44Khz.Init.Period = 90; // this value may have to be changed
	Timer6_44Khz.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init( &Timer6_44Khz );

	Timer6MasterConfigSync.MasterOutputTrigger = TIM_TRGO_UPDATE;
	Timer6MasterConfigSync.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization( &Timer6_44Khz, &Timer6MasterConfigSync );

//
// Set the priority of the interrupt and enable it
//
	NVIC_SetPriority(TIM6_DAC_IRQn, 0);
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

//
// Clear any pending interrupts
//
	__HAL_TIM_CLEAR_FLAG( &Timer6_44Khz, TIM_SR_UIF );



//
// Enable the timer interrupt and the DAC Trigger
//

	__HAL_TIM_ENABLE_DMA( &Timer6_44Khz, TIM_DIER_UDE );


//
// Enable the clocks for the DAC
//
	__HAL_RCC_DAC_CLK_ENABLE();

	AudioDac.Instance = DAC;
	if ( HAL_OK != HAL_DAC_Init( &AudioDac ))
	{
		trace_printf("DAC initialization failure\n");
		return;
	}

//
// Enable the trigger from the DMA controller and the output buffer of the DAC
//
	DacConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	DacConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	if ( HAL_DAC_ConfigChannel(&AudioDac, &DacConfig, DAC_CHANNEL_1) != HAL_OK )
	{
		trace_printf("DAC configuration failure\n");
		return;
	}

//
// Enable the clock for the DMA controller
//
	__HAL_RCC_DMA1_CLK_ENABLE();

//
// Initialize the stream and channel number and the memory transfer settings
//

    AudioDma.Instance = DMA1_Stream5;
    AudioDma.Init.Channel = DMA_CHANNEL_7;
    AudioDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    AudioDma.Init.PeriphInc = DMA_PINC_DISABLE;
    AudioDma.Init.MemInc = DMA_MINC_ENABLE;
    AudioDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    AudioDma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    AudioDma.Init.Mode = DMA_NORMAL;
    AudioDma.Init.Priority = DMA_PRIORITY_MEDIUM;
    AudioDma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init( &AudioDma );

//
// Link the DMA channel the to the DAC controller
//
    __HAL_LINKDMA( &AudioDac, DMA_Handle1, AudioDma );

//
// Enable the interrupt for the specific stream
//
    HAL_NVIC_SetPriority( DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ( DMA1_Stream5_IRQn );

//
// Start the timer
//
	__HAL_TIM_ENABLE( &Timer6_44Khz );

	return;
}

#define CLK 0
#define CLKSET 1
#define ALARM 2
#define ALMSET 3
#define AMPM 4
#define ALMSET2 5
#define INIT 6
#define true 1
#define false 0

int b1 = true;
int b2 = true;
int b3 = true;
int b4 = true;
int b5 = true;

int button(int buttonnum,TIM_HandleTypeDef s_TimerInstance){

	switch(buttonnum){
	case 1:
	 if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4 ) && b1) //check if debounced button is pressed down
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b1 = 0;
	    	  return 1;

	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4 ) && !b1) //check if debounced button is released
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b1 = 1;
	    	  return 0;
	      }else{
	    	  return 0;
	      }
	break;

	case 2:
	 if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5 ) && b2) //check if debounced button is pressed down
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	     b2 = 0;
	    	     return 1;
	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5 ) && !b2) //check if debounced button is released
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b2 = 1;
	    	  return 0;
	      }else{
	    	  return 0;
	      }
	break;

	case 3:
	 if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8 ) && b3) //check if debounced button is pressed down
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b3 = 0;
	    	  return 1;
	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8 ) && !b3) //check if debounced button is released
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b3 = 1;
	    	  return 0;
	      }else{
	    	  return 0;
	      }
	break;

	case 4:
	 if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9 ) && b4) //check if debounced button is pressed down
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b4 = 0;
	    	  return 1;

	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9 ) && !b4) //check if debounced button is released
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b4 = 1;
	    	  return 0;
	      }else{
	    	  return 0;
	      }
	break;

	case 5:
	 if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11 ) && b5) //check if debounced button is pressed down
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b5 = 0;
	    	  return 1;

	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11 ) && !b5) //check if debounced button is released
	      {
	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
	    	  b5 = 1;
	    	  return 0;
	      }else{
	    	  return 0;
	      }
	break;
	}//end of switch
	 return 0;
}

void sideLEDs(int alarm, int pm, int extra){

	if(alarm)
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_4 , GPIO_PIN_SET );
	else
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_4 , GPIO_PIN_RESET );
	if(pm)
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_2 , GPIO_PIN_SET );
	else
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_2 , GPIO_PIN_RESET );
	if(extra)
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_6 , GPIO_PIN_SET );
	else
	HAL_GPIO_WritePin( GPIOE, GPIO_PIN_6 , GPIO_PIN_RESET );
}

void displaynum(int num){
	switch(num){
		case 0:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
		break;

		case 1:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_RESET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
		break;

		case 2:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_RESET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 3:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 4:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_RESET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 5:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_RESET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 6:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_RESET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 7:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
		break;

		case 8:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;

		case 9:
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
			HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
		break;
	}//end switch number
}

int main(int argc, char* argv[])
{

//
// Reset of all peripherals, Initializes the Flash interface and the System timer.
//
	HAL_Init();

	__GPIOD_CLK_ENABLE();
				__GPIOE_CLK_ENABLE();
				__GPIOC_CLK_ENABLE();

		   		GPIO_InitTypeDef GPIO_InitStructure4; // handle for pointing GPIO

		   		GPIO_InitStructure4.Pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
		   		GPIO_InitStructure4.Mode = GPIO_MODE_INPUT;
		   		GPIO_InitStructure4.Speed = GPIO_SPEED_HIGH;
		   		GPIO_InitStructure4.Pull = GPIO_PULLUP;
		   		GPIO_InitStructure4.Alternate = 0;

		   		 HAL_GPIO_Init(GPIOC, &GPIO_InitStructure4);

		   		GPIO_InitTypeDef GPIO_InitStructure5; // handle for pointing GPIO

		   		GPIO_InitStructure5.Pin = GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_2;
		   		GPIO_InitStructure5.Mode = GPIO_MODE_OUTPUT_PP;
		   		GPIO_InitStructure5.Speed = GPIO_SPEED_LOW;
		   		GPIO_InitStructure5.Pull = GPIO_NOPULL;
		   		GPIO_InitStructure5.Alternate = 0;

		   		 HAL_GPIO_Init(GPIOE, &GPIO_InitStructure5);

			   	GPIO_InitTypeDef GPIO_InitStructure3; // handle for pointing GPIO

			   		GPIO_InitStructure3.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7 ;
			   		GPIO_InitStructure3.Mode = GPIO_MODE_OUTPUT_PP;
			   		GPIO_InitStructure3.Speed = GPIO_SPEED_LOW;
			   		GPIO_InitStructure3.Pull = GPIO_NOPULL;
			   		GPIO_InitStructure3.Alternate = 0;

			   		 HAL_GPIO_Init(GPIOD, &GPIO_InitStructure3);

			   		//HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4 );
					__HAL_RCC_TIM3_CLK_ENABLE();// enable clock for Timer 3
					TIM_HandleTypeDef s_TimerInstance; // define a handle to initialize timer
					s_TimerInstance.Instance = TIM3; // Point to Timer 3
					s_TimerInstance.Init.Prescaler = 83; // Timer clock maximum frequency is 84 MHz.
					// This prescaler will give 10 kHz timing_tick_frequency
					s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
					s_TimerInstance.Init.Period = 999999999; // To count until 10ms seconds.
					// http://cerdemir.com/timers-in-stm32f4-discovery.
					//see the above link for Prescaler and period settings
					s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
					s_TimerInstance.Init.RepetitionCounter = 0;
					HAL_TIM_Base_Init( &s_TimerInstance );// Initialize timer with above parameters
					HAL_TIM_Base_Start(&s_TimerInstance); // start timer


				__HAL_RCC_TIM1_CLK_ENABLE();// enable clock for Timer 4

				TIM_HandleTypeDef s_TimerInstance2; // define a handle to initialize timer

				s_TimerInstance2.Instance = TIM1; // Point to Timer 3
				s_TimerInstance2.Init.Prescaler =  6; // Timer clock maximum frequency is 84 MHz.
				// This prescaler will give 10 kHz timing_tick_frequency
				s_TimerInstance2.Init.CounterMode = TIM_COUNTERMODE_UP;
				s_TimerInstance2.Init.Period = 999999999; // To count until 10ms seconds.
				// http://cerdemir.com/timers-in-stm32f4-discovery.
				//see the above link for Prescaler and period settings
				s_TimerInstance2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
				s_TimerInstance2.Init.RepetitionCounter = 0;
				HAL_TIM_Base_Init( &s_TimerInstance2 );// Initialize timer with above parameters
				HAL_TIM_Base_Start(&s_TimerInstance2); // start timer

				__HAL_RCC_TIM4_CLK_ENABLE();
				TIM_HandleTypeDef s_TimerInstance3; // define a handle to initialize timer
				s_TimerInstance3.Instance = TIM4 ; // Point to Timer 4
				s_TimerInstance3.Init.Prescaler = 8399;
				// This prescaler will give 10 kHz timing_tick_frequency
				s_TimerInstance3.Init.CounterMode = TIM_COUNTERMODE_UP;
				s_TimerInstance3.Init.Period = 4999; // To count until 10ms seconds.
				// http://cerdemir.com/timers-in-stm32f4-discovery.
				//see the above link for Prescaler and period settings
				s_TimerInstance3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
				s_TimerInstance3.Init.RepetitionCounter = 0;
				HAL_TIM_Base_Init( &s_TimerInstance3 );// Initialize timer with above parameters
				HAL_TIM_Base_Start(&s_TimerInstance3); // start timer


				__GPIOA_CLK_ENABLE(); // enable clock for GPIO port A
					GPIO_InitTypeDef GPIO_InitStructure2; //a handle to initialize GPIO port A

					GPIO_InitStructure2.Pin = GPIO_PIN_0; // pin corresponding to push button
					GPIO_InitStructure2.Mode = GPIO_MODE_INPUT;
					GPIO_InitStructure2.Speed = GPIO_SPEED_HIGH;
					GPIO_InitStructure2.Pull = GPIO_NOPULL;
					GPIO_InitStructure2.Alternate = 0;
					HAL_GPIO_Init(GPIOA, &GPIO_InitStructure2); // Initialize GPIO port A with above parameters

				int digit = 3;
				int hr = 12;
				int min = 00;
				int hrL;
				int hrR;
				int minL;
				int minR;
				int colon = 2;
				int Benable = 1;
				int Benable2 = 1;
				int number = 0;

				int mode = INIT;

				int wait = 0;
				int almhr = 12;
				int almmin = 34;
				int almhrL;
				int almhrR;
				int almminL;
				int almminR;
				int cycle = 0;
				int toggle = 0;
				int alarm = true;
				int pm = true;

//
// Configure the system clock
//
	SystemClock_Config();

//
// Initialize all configured peripherals
//
	MX_GPIO_Init();

//
// Enable the serial debug port. This allows for text messages to be sent via the STlink virtual communications port to the host computer.
//
	DebugPortInit();

//
// Display project name with version number
//
	trace_puts(
			"*\n"
			"*\n"
			"* Alarm clock project for stm32f4discovery board V2.00\n"
			"*\n"
			"*\n"
			);

// Initialize the I2C port for the external CODEC
	MX_I2C1_Init();

// Configure the CODEC for analog pass through mode.
// This allows for audio to be played out of the stereo jack
	InitAudioChip();
// Initialize the flash file and the USB host adapter subsystem
	MX_FATFS_Init();
	MX_USB_HOST_Init();
// Initialize the DMA and DAC systems. This allows for audio to be played out of GPIOA pin 4
	ConfigureAudioDma();
	trace_puts("Initialization Complete");
// At this stage the system clock should have already been configured at high speed.
	trace_printf("System clock: %u Hz\n", SystemCoreClock);
// Start the system timer
	timer_start();

// Wait until the drive is mounted before we can play some music
	do {
		MX_USB_HOST_Process();
	} while ( Appli_state != APPLICATION_READY );

	trace_printf( "\n" );

PlayMusic = false;

	while ( TRUE )
	{
		 hrL = hr/10;
			  hrR = hr%10;
			  minL = min/10;
			  minR = min%10;
			  almhrL = almhr/10;
			  almhrR = almhr%10;
			  almminL = almmin/10;
			  almminR = almmin%10;

			  if (__HAL_TIM_GET_FLAG(&s_TimerInstance2, TIM_FLAG_UPDATE) != RESET){
				  __HAL_TIM_CLEAR_IT(&s_TimerInstance2, TIM_IT_UPDATE);
				  digit = (digit+1)%7;
			  }
		      switch(mode){
		      	  case INIT:
		      		  switch(digit){
					case 0: //off
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );

						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					break;

					case 1: //dig1
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );
						sideLEDs(false, false, false);

					displaynum(8);
					break;

					case 2: //dig2
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(8);
					//parse digit
					break;

					case 3: //dig3
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(8);
					//parse digit
					break;

					case 4: //dig4
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(8);
					//parse digit
					break;

					case 5: //colon
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(2);
					//parse digit
					break;

					case 6: //LEDS
					sideLEDs(alarm, pm, true);
					break;
				}
		      	if(button(1, s_TimerInstance)){
		      		mode = CLK;
		      	}
		      	break;
		  		case CLK:
		  			if (button(1, s_TimerInstance) || button(2, s_TimerInstance) || button(3, s_TimerInstance) || button(4, s_TimerInstance) || button(5, s_TimerInstance)){
		  				 min++;
		  				    	        if(min >= 60){
		  				    	        	min = 0;
		  				    	        	hr++;
		  				    	        }if(hr >= 25){
		  				    	        	hr = 1;
		  				    	        }
		  			}
				switch(digit){
					case 0: //off
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );

						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );

					break;

					case 1: //dig1
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );

					displaynum(hrL);
					break;

					case 2: //dig2
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(hrR);
					//parse digit
					break;

					case 3: //dig3
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(minL);
					//parse digit
					break;

					case 4: //dig4
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(minR);
					//parse digit
					break;

					case 5: //colon
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
						HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
					displaynum(colon);
					//parse digit
					break;
				}

				break;
				}//end Switch mode

//
//	checks for the alarm interrupt and call the music playing module
//
		if ( TRUE == PlayMusic )
		{

    	 	 MSC_Application();
    	 	 PlayMusic = FALSE;
		}

	 	__asm__ volatile ( "wfi" );
//
// Wait for an interrupt to occur
//



	}
}//end of main

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
void MX_I2C1_Init(void)
{

	I2c.Instance = I2C1;
	I2c.Init.ClockSpeed = 100000;
	I2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2c.Init.OwnAddress1 = 0;
	I2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2c.Init.OwnAddress2 = 0;
	I2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init( &I2c );

}

void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();



	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
			|Audio_RST_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : CS_I2C_SPI_Pin */
	GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_Init( GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
	GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PDM_OUT_Pin */
	GPIO_InitStruct.Pin = PDM_OUT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);


	/*Configure GPIO pins : PA5 PA6 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : BOOT1_Pin */
	GPIO_InitStruct.Pin = BOOT1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : CLK_IN_Pin */
	GPIO_InitStruct.Pin = CLK_IN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
	GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
			|Audio_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
	GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : MEMS_INT2_Pin */
	GPIO_InitStruct.Pin = MEMS_INT2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

}

/**
 * @brief  Main routine for Mass Storage Class
 * @param  None
 * @retval None
 */
static void MSC_Application(void)
{
	FRESULT
		Result;                                          /* FatFs function common result code */

//
// Mount the flash drive using a fat file format
//

	Result = f_mount( &UsbDiskFatFs, (TCHAR const*)USBH_Path, 0);
	if( FR_OK == Result )
	{

//
// File system successfully mounted, play all the music files in the directory.
//
		while ( TRUE == PlayMusic )
		{
			PlayDirectory( "", 0 );
		}
	}
	else
	{
//
// FatFs Initialization Error
//
	//	Error_Handler();
	}

//
// Unlink the USB disk I/O driver
//
	FATFS_UnLinkDriver( UsbDiskPath );
}



/*
 * Function: TIM5_IRQHandler
 *
 * Description:
 *
 * Timer interrupt handler that is called at a rate of 500Hz. This function polls the time and
 * displays it on the 7 segment display. It also checks for button presses and handles any bounce conditions.
 *
 */

void TIM5_IRQHandler(void)
{


}


/*
 * Function: RTC_Alarm_IRQHandler
 *
 * Description:
 *
 * When alarm occurs, clear all the interrupt bits and flags then start playing music.
 *
 */

void RTC_Alarm_IRQHandler(void)
{

//
// Verify that this is a real time clock interrupt
//
	if( __HAL_RTC_ALARM_GET_IT( &RealTimeClock, RTC_IT_ALRA ) != RESET )
	{

//
// Clear the alarm flag and the external interrupt flag
//
    	__HAL_RTC_ALARM_CLEAR_FLAG( &RealTimeClock, RTC_FLAG_ALRAF );
    	__HAL_RTC_EXTI_CLEAR_FLAG( RTC_EXTI_LINE_ALARM_EVENT );

//
// Restore the alarm to it's original time. This could have been a snooze alarm
//
    	HAL_RTC_SetAlarm_IT( &RealTimeClock, &ClockAlarm, RTC_FORMAT_BCD );

    	PlayMusic = TRUE;

	}


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

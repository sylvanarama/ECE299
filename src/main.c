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
#include "init.h"

// Disable specific warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

// Alarm clock states
#define INIT 	 	0
#define TIME		1
#define ALARM 	 	2
#define MODE	 	3

// Set time modes
#define TELL_TIME   0
#define SET_TIME    1
#define SET_ALARM   2
#define SET_24HR    3

volatile int state = INIT;
volatile int mode = TELL_TIME;
volatile int hour_format = 0; // 0 = 12 hrs, 1 = 24 hrs
volatile int pm = 0;
volatile int alarm_set = 0;
int b1 = 0;
int b2 = 0;
int b3 = 0;
int b4 = 0;
int b5 = 0;

//interrupt for updating clock display
void TIM5_DAC_IRQHandler(void)
{
	digit = ((digit+1)%5)+1;
	update_display();
	HAL_NVIC_DisableIRQ( TIM5_DAC_IRQn );
}

// interrupt for alarm
void RTC_Alarm_IRQHandler(void)
{
	// Verify that this is a real time clock interrupt
	if( __HAL_RTC_ALARM_GET_IT( &RealTimeClock, RTC_IT_ALRA ) != RESET )
	{
		// Clear the alarm flag and the external interrupt flag
    	__HAL_RTC_ALARM_CLEAR_FLAG( &RealTimeClock, RTC_FLAG_ALRAF );
    	__HAL_RTC_EXTI_CLEAR_FLAG( RTC_EXTI_LINE_ALARM_EVENT );

    	// Restore the alarm to it's original time. This could have been a snooze alarm
    	HAL_RTC_SetAlarm_IT( &RealTimeClock, &ClockAlarm, RTC_FORMAT_BCD );

    	state == ALARM;
	}
}

//interrupt for button 1 (mode)
void EXTI4_IRQHandler(void)
{
	state = MODE;
	mode = (mode+1)%4;
	HAL_NVIC_DisableIRQ( EXTI4_IRQn );
}

//interrupt for button 2 (alarm on/off)
void EXTI5_IRQHandler(void)
{
	state = TIME;
	HAL_NVIC_DisableIRQ( EXTI5_IRQn );
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
}//display_num

void update_display(digit){

	switch(digit){
/*			case 0: //off
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
			break;*/

			case 1: //dig1
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_2 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_6 , GPIO_PIN_RESET );

				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );
				displaynum((ClockTime.Hours && 0xF0) >> 4); // 10s digit of hours
			break;

			case 2: //dig2
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET ); // turn off digit 1
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );   // turn on digit 2
				displaynum(ClockTime.Hours && 0x0F); // 1s digit of hours
			break;

			case 3: //dig3
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET ); // turn off digit 3
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );   // turn on digit 4
				displaynum((ClockTime.Minutes && 0xF0) >> 4); // 10s digit of minutes
			break;

			case 4: //dig4
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET ); // turn off digit 3
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );   // turn on digit 4
				displaynum(ClockTime.Minutes && 0x0F); // 1s digit of minutes
			break;

			case 5: //colon
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET ); // turn off digit 4
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );   // turn on the colon
				displaynum(colon); // middle colon
			break;

			case 6: // alarm mode lights
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET ); // turn off the colon
				if(alarm_set)
					HAL_GPIO_WritePin( GPIOE, GPIO_PIN_4 , GPIO_PIN_SET ); // turn on the alarm light
				if(pm)
					HAL_GPIO_WritePin( GPIOE, GPIO_PIN_2 , GPIO_PIN_SET ); // turn on the PM light
				if(hour_format == 1)
					HAL_GPIO_WritePin( GPIOE, GPIO_PIN_6 , GPIO_PIN_SET ); // turn on the 24hr light
		}//switch
}//display_time

void system_init() {
	HAL_init(); // Reset of all peripherals, Initializes the Flash interface and the System timer.
	SystemClockConfig(); // Configure the system clock
	MX_GPIO_Init(); // Initialize all configured peripherals
	DebugPortInit(); // Enable the serial debug port
	MX_I2C1_Init();  // Initialize the I2C port for the external CODEC
	InitAudioChip(); // Configure audio to play out the stereo jack
	MX_FATFS_Init(); // Initialize the flash file and
	MX_USB_HOST_Init(); // Initialize the USB host adapter subsystem
	ConfigureAudioDma(); // Configure audio to be played out of GPIOA pin 4

	init_GPIO(); // Initialize the display and button pins
	init_timers(); // Set up timers for display and buttons
	init_interrupts(); // Set up interrupts for display and buttons

	// Send a greeting to the trace device (skipped on Release).
	trace_puts("Initialization Complete");

	// At this stage the system clock should have already been configured at high speed.
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	// Start the system timer
	timer_start();
	blink_led_init();

	// Wait until the drive is mounted before we can play some music
	do {
		MX_USB_HOST_Process();
	} while ( Appli_state != APPLICATION_READY );

	trace_printf( "\n" );

	// Display project name with version number
	trace_puts(
			"*\n"
			"*\n"
			"* Alarm clock project for stm32f4discovery board V3.00\n"
			"*\n"
			"*\n"
			);

}//system_init

int main() {

	switch(state) {
		case INIT:
			system_init();
			break;
		case TIME:
			timeHourCheck();
			timeMinuteCheck():
			break;
		case ALARM:
			play_music();
			break;
		case MODE:
			if (mode == SET_TIME) set_time();
			else if (mode == SET_ALARM) set_alarm();
			else if (mode == SET_24HR)  set_24hr();
			break;
	}

	return 0;
}


#pragma GCC diagnostic pop
// ----------------------------------------------------------------------------

/*
 * This file is part of the  µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------
#include "cmsis/cmsis_device.h"
#include "stm32f4xx.h"
#include "ctype.h"
#include "diag/Trace.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define CLK 0
#define CLKSET 1
#define ALARM 2
#define ALMSET 3
#define AMPM 4
#define ALMSET2 5
int main(int argc, char* argv[]){
  // At this stage the system clock should have already been configured
  // at high speed.
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

	   		GPIO_InitTypeDef GPIO_InitStructure; // handle for pointing GPIO

	   		GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_2;
	   		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	   		GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	   		GPIO_InitStructure.Pull = GPIO_NOPULL;
	   		GPIO_InitStructure.Alternate = 0;

	   		 HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

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
			s_TimerInstance2.Init.Prescaler = 10; // Timer clock maximum frequency is 84 MHz.
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
			int min = 34;
			int hrL;
			int hrR;
			int minL;
			int minR;
			int colon = 2;
			int Benable = 1;
			int Benable2 = 1;
			int number = 0;
			int mode = ALMSET;
			int wait = 0;
			int almhr = 12;
			int almmin = 34;
			int almhrL;
			int almhrR;
			int almminL;
			int almminR;
			int cycle = 0;
			int toggle = 0;
  // Infinite loop
  while (1)
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
		  digit = (digit+1)%6;
	  }
//button counting start

/*      if (mode != CLKSET && __HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 ) && Benable) //check if debounced button is pressed down
      {
    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag

    	  //Digit counter
    	  	 min++;
    	        if(min >= 60){
    	        	min = 0;
    	        	hr++;
    	        }if(hr >= 25){
    	        	hr = 1;
    	        }

    	       Benable = 0;
      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 ) && !Benable) //check if debounced button is released
      {
    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
    	  Benable = 1;
      }*/


//button counting end

      switch(mode){
      case ALMSET: //set alarm
  			if (__HAL_TIM_GET_FLAG(&s_TimerInstance3, TIM_FLAG_UPDATE) != RESET){
				__HAL_TIM_CLEAR_IT(&s_TimerInstance3, TIM_IT_UPDATE);
				if(wait >= 2){
					mode = ALMSET2;
					wait = 0;
				}
				wait++;
  			}
    	  switch(digit){
  			case 0://off
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  			break;

  			case 1: //dig1
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );
  			break;

  			case 2: //dig2
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  			//parse digit
  			break;

  			case 3: //dig3
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  			//parse digit
  			break;

  			case 4: //dig4
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  			//parse digit
  			break;

  			case 5: //colon
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  			//parse digit
  			break;
  		}
  		switch(digit){ //What letter to Display
  			case 1:
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_SET );//A
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_SET );//G
  			break;

  			case 2:
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_RESET );//A
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_RESET );//B
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_RESET );//C
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_SET );//D
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_SET );//E
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_SET );//F
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
  			break;

  			case 3:
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_RESET );//A
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_SET );//B
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_SET );//C
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
  			break;

  			case 5:
  			case 0:
  			case 4:
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_12 , GPIO_PIN_RESET );//A
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_7 , GPIO_PIN_RESET );//B
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_13 , GPIO_PIN_RESET );//C
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_8, GPIO_PIN_RESET );//D
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_14 , GPIO_PIN_RESET );//E
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_15 , GPIO_PIN_RESET );//F
  				HAL_GPIO_WritePin( GPIOE, GPIO_PIN_10 , GPIO_PIN_RESET );//G
  			break;
  	}
  		break;

  		case ALMSET2: //part2 of setting the alarm
  			if(__HAL_TIM_GET_FLAG(&s_TimerInstance3, TIM_FLAG_UPDATE) != RESET){
				__HAL_TIM_CLEAR_IT(&s_TimerInstance3, TIM_IT_UPDATE);
				toggle = (toggle + 1)%2;
  	  	}

  			if ( __HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4 ) && Benable) //check if debounced button is pressed down
  			  	      {
  			  	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
  			  	    	  cycle++;
  			  	    	       Benable = 0;
  			  	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4 ) && !Benable) //check if debounced button is released
  			  	      {
  			  	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
  			  	    	  Benable = 1;
  			  	      }

  			if ( __HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 ) && Benable2) //check if debounced button is pressed down
  	      {
  	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
  	    	  if(cycle%2 == 0){
  	    	  	 almmin = (almmin+ 1)%60;
  	    	  }else{
  	    		 almhr++;
  	    		 if (almhr >= 25){
  	    			 almhr = 1;
  	    		 }
  	    	  }
  	    	       Benable2 = 0;
  	      }else if (__HAL_TIM_GET_FLAG(&s_TimerInstance, TIM_FLAG_UPDATE) != RESET && !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0 ) && !Benable2) //check if debounced button is released
  	      {
  	    	  __HAL_TIM_CLEAR_IT(&s_TimerInstance, TIM_IT_UPDATE);// clear timer period flag
  	    	  Benable2 = 1;
  	      }
  			switch(digit){
  						case 0: //off
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  						break;

  						case 1: //dig1

  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							if (cycle%2 == 1){
  								if (toggle){
  									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );
  								}else
  									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  							}else if(cycle%2 == 0)
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );
  						number = almhrL;
  						break;

  						case 2: //dig2
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  							if (cycle%2 == 1){
  								if (toggle){
  									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
  								}else
  									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							}else if(cycle%2 == 0)
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  						number = almhrR;
  						//parse digit
  						break;

  						case 3: //dig3
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  							if (cycle%2 == 0){
								if (toggle){
									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
								}else
									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
							}else if(cycle%2 == 1)
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  						number = almminL;
  						//parse digit
  						break;

  						case 4: //dig4
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET);
  							if (cycle%2 == 0){
								if (toggle){
									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
								}else
									HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
							}else if(cycle%2 == 1)
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  						number = almminR;
  						//parse digit
  						break;

  						case 5: //colon
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
  							HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
  						number = colon;
  						//parse digit
  						break;
  					}
  					switch(number){
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



  					}
  		break;

  		case CLK:
		switch(digit){
			case 0: //off
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );

			break;

			case 1: //dig1
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_SET );

			number = hrL;
			break;

			case 2: //dig2
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_SET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
			number = hrR;
			//parse digit
			break;

			case 3: //dig3
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_SET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
			number = minL;
			//parse digit
			break;

			case 4: //dig4
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_SET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
			number = minR;
			//parse digit
			break;

			case 5: //colon
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_1 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_3 , GPIO_PIN_SET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_4 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_6 , GPIO_PIN_RESET );
				HAL_GPIO_WritePin( GPIOD, GPIO_PIN_7 , GPIO_PIN_RESET );
			number = colon;
			//parse digit
			break;
		}


		switch(number){
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
		break;
		}//end switch number
		}//end Switch mode

    }//while
}//main

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------




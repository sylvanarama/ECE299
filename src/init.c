void init_GPIO() {

	// PORT C: Buttons
	__GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure4; // handle for pointing GPIO
	GPIO_InitStructure4.Pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
	GPIO_InitStructure4.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure4.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure4.Pull = GPIO_PULLUP;
	GPIO_InitStructure4.Alternate = 0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure4);

	 // PORT D: Digits
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure5; //a handle to initialize GPIO port D
	GPIO_InitStructure5.Pin = GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_2;
	GPIO_InitStructure5.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure5.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure5.Pull = GPIO_NOPULL;
	GPIO_InitStructure5.Alternate = 0;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure5); // Initialize GPIO port D with above parameters

	// PORT E: LEDs
	__GPIOE_CLK_ENABLE(); // enable clock for GPIO port E
	GPIO_InitTypeDef GPIO_InitStructure3; //a handle to initialize GPIO port E
	GPIO_InitStructure3.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7 ;
	GPIO_InitStructure3.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure3.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure3.Pull = GPIO_NOPULL;
	GPIO_InitStructure3.Alternate = 0;
	 HAL_GPIO_Init(GPIOD, &GPIO_InitStructure3);  // Initialize GPIO port E with above parameters

	// PORT A: push button
	 __GPIOA_CLK_ENABLE(); // enable clock for GPIO port A
	GPIO_InitTypeDef GPIO_InitStructure2; //a handle to initialize GPIO port A
	GPIO_InitStructure2.Pin = GPIO_PIN_0; // pin corresponding to push button
	GPIO_InitStructure2.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure2.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure2.Pull = GPIO_NOPULL;
	GPIO_InitStructure2.Alternate = 0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure2); // Initialize GPIO port A with above parameters

}//init_GPIO

void init_timers() {

	// TIMER 3:
	__HAL_RCC_TIM3_CLK_ENABLE();// enable clock for Timer 3
	TIM_HandleTypeDef debounce_timer; // define a handle to initialize timer
	debounce_timer.Instance = TIM3; // Point to Timer 3
	debounce_timer.Init.Prescaler = 83; // Timer clock maximum frequency is 84 MHz.

	// This prescaler will give 10 kHz timing_tick_frequency
	debounce_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
	debounce_timer.Init.Period = 999999999; // To count until 10ms seconds.
	debounce_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	debounce_timer.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init( &debounce_timer );// Initialize timer with above parameters
	HAL_TIM_Base_Start(&debounce_timer); // start timer

	// TIMER 5
	__HAL_RCC_TIM5_CLK_ENABLE();// enable clock for Timer 1
	TIM_HandleTypeDef display_timer; // define a handle to initialize timer
	display_timer.Instance = TIM5; // Point to Timer 3
	display_timer.Init.Prescaler =  6; // Timer clock maximum frequency is 84 MHz.
	// This prescaler will give 10 kHz timing_tick_frequency
	display_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
	display_timer.Init.Period = 999999999; // To count until 10ms seconds.
	display_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	display_timer.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init( &display_timer );// Initialize timer with above parameters
	HAL_TIM_Base_Start(&display_timer); // start timer

	// TIMER 4
	__HAL_RCC_TIM4_CLK_ENABLE();
	TIM_HandleTypeDef alarm_timer; // define a handle to initialize timer
	alarm_timer.Instance = TIM4 ; // Point to Timer 4
	alarm_timer.Init.Prescaler = 8399;
	// This prescaler will give 10 kHz timing_tick_frequency
	alarm_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
	alarm_timer.Init.Period = 4999; // To count until 10ms seconds.
	alarm_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	alarm_timer.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init( &alarm_timer );// Initialize timer with above parameters
	HAL_TIM_Base_Start(&alarm_timer); // start timer

}//init_timers

void init_interrupts() {
	NVIC_InitTypeDef NVIC_InitStruct;
	// configure interrupt for button 1 (mode) on PC4
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	//configure interrupt for button 2 (alarm on/off) on PC5
	NVIC_InitStruct.NVIC_IRQChannel = EXTI5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	//configure interrupt for updating display on TIM5
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	//configure interrupt for debounce delay on TIM3
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	//configure interrupt for alarm time on TIM4
	NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

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

}//MX_GPIO_Init

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

}//MX_I2C1_Init

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
}//SystemClockConfig

void ConfigureAudioDma( void )
{

	TIM_MasterConfigTypeDef Timer6MasterConfigSync;

	GPIO_InitTypeDef GPIO_InitStructure;

	DAC_ChannelConfTypeDef DacConfig;

	// If we have the timer 6 interrupt enabled then disable the timer from running when we halt the processor or hit a breakpoint.
	// This also applies to printing using the semihosting method which also uses breakpoints to transfer data to the host computer
	__HAL_DBGMCU_UNFREEZE_TIM6();

	// Enable the clocks for GPIOA, GPIOC and Timer 6
	__HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA4 as an analog output ( used for D/A output of the analog signal )
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStructure.Alternate = 0;
	HAL_GPIO_Init( GPIOA, &GPIO_InitStructure);

	// Configure timer 6 for a clock frequency of 44Khz and a triggered output for the DAC
	Timer6_44Khz.Instance = TIM6;
	Timer6_44Khz.Init.Prescaler = 20; //this value may have to be changed
	Timer6_44Khz.Init.CounterMode = TIM_COUNTERMODE_UP;
	Timer6_44Khz.Init.Period = 90; // this value may have to be changed
	Timer6_44Khz.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init( &Timer6_44Khz );

	Timer6MasterConfigSync.MasterOutputTrigger = TIM_TRGO_UPDATE;
	Timer6MasterConfigSync.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization( &Timer6_44Khz, &Timer6MasterConfigSync );

	// Set the priority of the interrupt and enable it
	NVIC_SetPriority(TIM6_DAC_IRQn, 0);
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	// Clear any pending interrupts
	__HAL_TIM_CLEAR_FLAG( &Timer6_44Khz, TIM_SR_UIF );

	// Enable the timer interrupt and the DAC Trigger
	__HAL_TIM_ENABLE_DMA( &Timer6_44Khz, TIM_DIER_UDE );

	// Enable the clocks for the DAC
	__HAL_RCC_DAC_CLK_ENABLE();

	AudioDac.Instance = DAC;
	if ( HAL_OK != HAL_DAC_Init( &AudioDac ))
	{
		trace_printf("DAC initialization failure\n");
		return;
	}

	// Enable the trigger from the DMA controller and the output buffer of the DAC
	DacConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	DacConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	if ( HAL_DAC_ConfigChannel(&AudioDac, &DacConfig, DAC_CHANNEL_1) != HAL_OK )
	{
		trace_printf("DAC configuration failure\n");
		return;
	}

	// Enable the clock for the DMA controller
	__HAL_RCC_DMA1_CLK_ENABLE();

	// Initialize the stream and channel number and the memory transfer settings
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

    // Link the DMA channel the to the DAC controller
    __HAL_LINKDMA( &AudioDac, DMA_Handle1, AudioDma );

    // Enable the interrupt for the specific stream
    HAL_NVIC_SetPriority( DMA1_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ( DMA1_Stream5_IRQn );


    // Start the timer
	__HAL_TIM_ENABLE( &Timer6_44Khz );

	return;
}//ConfigureAudioDMA

#include "pwm.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include "det.h"



static channel_t pwm_channels[PWM1_CHANNEL_NUMBER];
static uint8_t initflag = 0;
static uint8_t isrunning = 0;

static TIM_HandleTypeDef htim2;
#define TIM2_PERIOD  1000
#define LED_ON() 					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)
#define LED_OFF()					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)


void pwm_init(void)
{
	if(!initflag)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		// Initialize the TIM2 low level resources
		__HAL_RCC_TIM2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE(); // use TIM2-CH1
		// GPIO init. use TIM2 CHANNEL 2 as pwm output.

		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		__HAL_RCC_GPIOC_CLK_ENABLE();
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		LED_OFF();
		

	// Configure the TIM2.
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = 7;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = TIM2_PERIOD - 1; // 1Mhz/1000 = 1000hz
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
		if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
		{
			Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_INIT, PWM_E_UNINIT);
		}

	// Configure the TIM2 CHANNEL1 in PWM no out put
			/* TIM2 interrupt Init */
		HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
		
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		__HAL_TIM_ENABLE_OCxPRELOAD(&htim2,TIM_CHANNEL_1);
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_INIT, PWM_E_UNINIT);
		}
	// Configure the TIM2 CHANNEL2 in PWM mode
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		__HAL_TIM_ENABLE_OCxPRELOAD(&htim2,TIM_CHANNEL_2);
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
		{
			Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_INIT, PWM_E_UNINIT);
		}
		initflag = 1;
		
		pwm_init_state(&pwm_channels[0], 1, "pwm IT simulation led",0,1000,0);
		pwm_init_state(&pwm_channels[1], 2, "pwm pin PA1 output",0,1000,0);

	}
}

void pwm_on(void)
{
	isrunning = 1;
}
void pwm_off(void)
{
	isrunning = 0;
}


void pwm_deinit(void)
{
	__HAL_RCC_TIM2_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	initflag = 0;
}
	
	
void pwm_setduty(uint8_t channel_id, uint8_t duty)
{
	
	if(!initflag)
	{
			Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_HW_INIT_FAILED);
		return;
	}
	
	if(!isrunning)
	{
		return;
	}

	if(channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_PARAM_CHANNEL);
		return;
	}

	if(duty > 100)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_PARAM_DUTY);
		return;
	}

	uint32_t hal_channel;
	uint16_t tim2_compare;
	
	switch(channel_id)
	{
		case 1:
			hal_channel = TIM_CHANNEL_1;
			break;
		case 2:
			hal_channel = TIM_CHANNEL_2;
			break;
	}

	tim2_compare = duty * TIM2_PERIOD / 100;

	if (channel_id == 1)
	{
    if (duty == 0)
    {
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
      LED_OFF();
			pwm_channels[0].state = 0;
    }
    else if (duty == 100)
    {
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
      LED_ON();
			pwm_channels[0].state = 0;
    }
		else
		{
			__HAL_TIM_SET_COMPARE(&htim2, hal_channel, tim2_compare);
			if(pwm_channels[0].state == 0)
			{
				__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
				if(HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1) != HAL_OK)
				{
						__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
						Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_HW_START_FAILED);
						return;
				}
				pwm_channels[0].state = 1;
			}
		}
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim2, hal_channel, tim2_compare);
	}
	pwm_channels[channel_id - 1].duty = duty;
	
}


uint8_t pwm_getduty(uint8_t channel_id)
{
	if(!isrunning)
	{
	return 0;
	}
	uint32_t hal_channel = 0;
	if(0 < channel_id && channel_id < PWM1_CHANNEL_NUMBER)
	{
		/*
		switch(channel_id)
		{
			case 1:
				hal_channel = TIM_CHANNEL_1;
			break;
			case 2:
				hal_channel = TIM_CHANNEL_2;
			break;
			
		}
		uint16_t tim2_compare = __HAL_TIM_GET_COMPARE(&htim2,hal_channel);
		uint8_t duty = tim2_compare / 10;
		return duty;*/
			return pwm_channels[channel_id-1].duty;
	}

	Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_GetDuty, PWM_E_PARAM_CHANNEL);
	return 0;

}

void pwm_get_state(uint8_t channel_id,channel_t* c)
{
	
    if(!initflag)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
                        PWM_API_GetState, PWM_E_HW_INIT_FAILED);
        return;
    }

    if(c == NULL)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
                        PWM_API_GetState, PWM_E_PARAM_POINTER);
        return;
    }

    if(channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
                        PWM_API_GetState, PWM_E_PARAM_CHANNEL);
        return;
    }


		c->channel_id = pwm_channels[channel_id - 1].channel_id;
		c->duty = pwm_channels[channel_id - 1].duty;
		c->frequency = pwm_channels[channel_id - 1].frequency;
		c->name = pwm_channels[channel_id - 1].name;
		c->state = pwm_channels[channel_id - 1].state;
		return;

}

void pwm_init_state(channel_t* c ,uint8_t channel_id,	const char* name,	uint8_t duty,	uint32_t frequency,	uint8_t state)
{
    if(c == NULL)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
                        PWM_API_UpdateState, PWM_E_PARAM_POINTER);
        return;
    }

    if(channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
                        PWM_API_UpdateState, PWM_E_PARAM_CHANNEL);
        return;
    }
		

		if(c->channel_id == 0){ c->channel_id = channel_id;}
		c->name = name;
		c->duty = duty;
		c->frequency = frequency;
		c->state = state;
		pwm_setduty(channel_id,duty);
		return;
}

void pwm_start(uint8_t channel_id)
{
	if(!isrunning)
	{
	return ;
	}

	if (!initflag)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_HW_INIT_FAILED);
		return;
	}

	/* 检查通道号 */
	if (channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_PARAM_CHANNEL);
		return;
	}

	/* 已经启动，不重复执行 */
	if (pwm_channels[channel_id - 1].state == 1)
	{
		return;
	}

	/* 根据通道启动对应的 PWM */
	if (channel_id == 1)
	{
		/* 启动 CH1，TIM2 计数器开始运行 */
		if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
		{
				Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
												PWM_API_Start, PWM_E_HW_START_FAILED);
				return;
		}

		/* CH1 使用软件中断模拟 LED PWM */
		if (pwm_channels[0].duty == 0)
		{
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

			LED_OFF();
		}
		else if (pwm_channels[0].duty == 100)
		{
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
			__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

			LED_ON();
		}
		else
		{
			/* 清除可能残留的中断标志 */
			__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
			__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

			/* 开启比较中断和更新中断 */
			__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
			__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
		}
	}
	else if (channel_id == 2)
	{
		/* 启动 CH2 硬件 PWM */
		if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
		{
				Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID,
												PWM_API_Start, PWM_E_HW_START_FAILED);
				return;
		}
	}

	/* 只有启动成功后才更新状态 */
	pwm_channels[channel_id - 1].state = 1;
}

void pwm_stop(uint8_t channel_id)
{	
	if(!isrunning)
	{
	return;
	}

	switch(channel_id)
	{
		case 1:
			if(pwm_channels[channel_id - 1].state == 1)
			{
				__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);
				HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
				pwm_channels[channel_id - 1].state = 0;
				LED_OFF();
			}
			break;

		
		case 2:
			if(pwm_channels[channel_id - 1].state == 1)
			{		
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
				pwm_channels[channel_id - 1].state = 0;
			}

		break;
	}
		
}



void PWM_TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);
}


// software stimulate PWM
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		LED_OFF();
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	
	if(htim->Instance == TIM2)
	{
		LED_ON();
	}
}
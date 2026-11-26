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
	if (!initflag)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_HW_INIT_FAILED);
		return;
	}
	if (isrunning)
	{
		return;
	}
	
	isrunning = 1;
}
void pwm_off(void)
{
	if (!initflag)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_HW_INIT_FAILED);
		return;
	}
	if (!isrunning)
	{
		return;
	}
	
 /* 关闭 CH1 的比较中断和更新中断 */
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
	__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

		/* 清除 CH1 相关中断标志 */
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
	
		/* 关闭 CH1 和 CH2 的输出使能 */
	TIM_CCxChannelCmd(htim2.Instance,TIM_CHANNEL_1, TIM_CCx_DISABLE);
	TIM_CCxChannelCmd(htim2.Instance,TIM_CHANNEL_2, TIM_CCx_DISABLE);

	
	/* 停止 TIM2 计数器 */
	__HAL_TIM_DISABLE(&htim2);
	/* LED 设置为低电平（PC13 为低电平点亮，实际表现为熄灭需要确认） */
	LED_OFF();

	/* 更新所有通道状态 */
	for (uint8_t i = 0; i < PWM1_CHANNEL_NUMBER; i++)
	{
			pwm_channels[i].state = 0;
	}
	/* 最后关闭模块运行标志 */
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
    uint32_t tim2_compare;
	
    /* 1. 检查初始化状态 */
    if (!initflag)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_HW_INIT_FAILED);
        return;
    }

    /* 2. 检查模块运行状态 */
    if (!isrunning)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_RT_NOTRUN_FAILED);
        return;
    }

    /* 3. 检查通道号 */
    if (channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_PARAM_CHANNEL);
        return;
    }

    /* 4. 检查占空比范围 */
    if (duty > 100)
    {
        Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_SetDuty, PWM_E_PARAM_DUTY);
        return;
    }
    /* 5. 计算比较值 */
    tim2_compare = ((uint32_t)duty * TIM2_PERIOD) / 100U;
    /* 6. 根据通道分别处理 */
    if (channel_id == 1)
    {
        /* CH1：软件模拟 PWM */
        /* 更新 CCR1 */
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, tim2_compare);
        /* 通道尚未启动，只保存配置，不产生输出 */
        if (pwm_channels[0].state == 0)
        {
            /* 不操作 LED 和中断 */
        }
        else
        {
            /* 通道已经启动，根据占空比更新输出 */

            if (duty == 0)
            {
                /* 0%：LED 恒灭 */
                __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
                __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

                LED_OFF();
            }
            else if (duty == 100)
            {
                /* 100%：LED 恒亮 */
                __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
                __HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

                LED_ON();
            }
            else
            {
                /* 1%~99%：开启软件 PWM */

                /* 先设置 LED 初始状态 */
                LED_OFF();

                /* 清除残留中断标志 */
                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
                __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

                /* 开启比较中断和更新中断 */
                __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
                __HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
            }
        }
    }
    else if (channel_id == 2)
    {
			/* CH2：硬件 PWM */
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tim2_compare);
    }

	/* 7. 最后更新占空比配置 */
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
		return;
}

void pwm_start(uint8_t channel_id)
{
	/* 1. 检查是否已经初始化 */
	if (!initflag)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_HW_INIT_FAILED);
		return;
	}
	/* 2. 检查模块是否处于运行状态 */
	if (!isrunning)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_RT_NOTRUN_FAILED);
		return;
	}
	/* 3. 检查通道号 */
	if (channel_id < 1 || channel_id > PWM1_CHANNEL_NUMBER)
	{
		Det_ReportError(PWM_MODULE_ID, PWM1_INSTANCE_ID, PWM_API_Start, PWM_E_PARAM_CHANNEL);
		return;
	}
	/* 4. 已经启动，不重复执行 */
	if (pwm_channels[channel_id - 1].state == 1)
	{
		return;
	}
	/* 5. 根据通道启动 PWM */
	switch (channel_id)
{
		case 1:
		{
				/* CH1：软件模拟 PWM */
				/* 设置比较值 */
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,
						(pwm_channels[0].duty * TIM2_PERIOD) / 100);

				/* 先关闭相关中断，避免启动时产生意外回调 */
				__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_CC1);
				__HAL_TIM_DISABLE_IT(&htim2, TIM_IT_UPDATE);

				/* 清除残留中断标志 */
				__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1);
				__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

				/* 根据占空比设置 LED 初始状态 */
				if (pwm_channels[0].duty == 0)
				{
						LED_OFF();
				}
				else if (pwm_channels[0].duty == 100)
				{
						LED_ON();
				}
				else
				{
						LED_OFF();
						/* 开启比较中断和更新中断 */
						__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_CC1);
						__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);
				}
				/* 更新通道状态 */
				pwm_channels[0].state = 1;

				/* 使能 CH1 */
			TIM_CCxChannelCmd(htim2.Instance,TIM_CHANNEL_1, TIM_CCx_ENABLE);
				break;
		}
		case 2:
		{
				/* CH2：硬件 PWM */
				/* 更新通道状态 */
				pwm_channels[1].state = 1;
				/* 使能 CH2 */
			TIM_CCxChannelCmd(htim2.Instance,TIM_CHANNEL_2, TIM_CCx_ENABLE);
				break;
		}
		default:
				return;
	}

	/* 6. 确保 TIM2 计数器运行 */
	__HAL_TIM_ENABLE(&htim2);
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

uint8_t pwm_isrunning(void)
{
	if(!initflag)
	{
		return 0;
	}
	return isrunning;
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
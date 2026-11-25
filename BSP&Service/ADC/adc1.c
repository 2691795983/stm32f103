#include "adc1.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "det.h"

static ADC_HandleTypeDef hadc1;

void adc1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
// 1. ADC prerequisite.
	// CLOCK init.
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
	// ADC pins init.
	  /**ADC1 GPIO Configuration
    PB0     ------> ADC1_IN8
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	// optionally, ADC NVIC init.
	// optionally, DMA init.
	
// 2. Configurating  ADC.
	// ADC parameters(resulotion, data alignment)
  ADC_ChannelConfTypeDef sConfig = {0};
	
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
		Det_ReportError(ADC_MODULE_ID, ADC_ADC1_INSTANCE_ID, ADC_API_INIT, ADC_E_UNINIT);
  }

	// ADC Channels
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
		Det_ReportError(ADC_MODULE_ID, ADC_ADC1_INSTANCE_ID, ADC_API_INIT, ADC_E_UNINIT);
  }
	
}

void adc1_deinit(void)
{
		__HAL_RCC_ADC1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
}

uint32_t adc1_execute_convention(void)
{
  uint32_t adc_value = 0;
  if(HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    Det_ReportError(ADC_MODULE_ID, ADC_ADC1_INSTANCE_ID, ADC_API_EXECUTE_CONVENTION, ADC_E_UNINIT);
    return 0;
  }
  if(HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK)
  {
    HAL_ADC_Stop(&hadc1);
    Det_ReportError(ADC_MODULE_ID, ADC_ADC1_INSTANCE_ID, ADC_API_EXECUTE_CONVENTION, ADC_E_TIMEOUT);
    return 0;
  }

  adc_value = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);
  return adc_value;
}
#include "shell_adc1.h"
#include "shell.h"
#include "adc1.h"

void shell_adc1_list(void)
{

}

void shell_adc1_get(void)
{
    uint32_t adcValue = 0;
    float vSense = 0.0f;
    float temperature = 0.0f;
    adcValue = adc1_execute_convention();
    // VSENSE = (adcValue / 4096.0) * 3.3
    vSense = ((float)adcValue / 4096.0f) * 3.3f;
    // Temperature (°C) = ((V₂₅ - VSENSE) / Avg_Slope) + 25
    temperature = ((1.43f - vSense) / (4.3f/1000.0f)) + 25.0f;


    shellPrint(shellGetCurrent(), "Raw adc value: %d, temperature: %.2f°C.\r\n", adcValue, temperature);
	
}
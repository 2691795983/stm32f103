#ifndef ADC_CFG_H
#define ADC_CFG_H
#include "det_moduleid.h"
/* ADC Module ID */
#define ADC_MODULE_ID              DET_MODULE_ID_ADC

/* ADC Instance Number */
#define ADC_INSTANCE_NUMBER        1U

/* ADC Instance ID */
#define ADC_ADC1_INSTANCE_ID       0x00U


/* ADC API ID */
#define ADC_API_INIT               0x00U
#define ADC_API_READ_CHANNEL       0x01U
#define ADC_API_EXECUTE_CONVENTION   0x02U

/* ADC Error ID */
#define ADC_E_PARAM_POINTER        0x01U
#define ADC_E_PARAM_CHANNEL        0x02U
#define ADC_E_PARAM_GROUP          0x03U
#define ADC_E_UNINIT               0x04U
#define ADC_E_BUSY                 0x05U
#define ADC_E_TIMEOUT              0x06U

#endif
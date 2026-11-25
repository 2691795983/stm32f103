#ifndef PWM_CFG_H
#define PWM_CFG_H
#include "det_moduleid.h"

/* PWM Module ID */
#define PWM_MODULE_ID 	 							 DET_MODULE_ID_PWM

/* PWM Instance Number */
#define PWM_INSTANCE_NUMBER        1U

/* PWM Instance ID */
#define PWM_PWM1_INSTANCE_ID       0x00U

/* PWM API ID */
#define PWM_API_INIT               0x00U
#define PWM_API_Start       			 0x01U
#define PWM_API_Stop   						 0x02U
#define PWM_API_SetDuty   				 0x03U
#define PWM_API_UpdateState   		 0x04U
#define PWM_API_GetDuty   				 0x05U
#define PWM_API_GetState   		 		 0x06U
/* PWM Error ID */
/* Development Errors */
#define PWM_E_PARAM_CHANNEL      0x01U
#define PWM_E_PARAM_FREQUENCY    0x02U
#define PWM_E_PARAM_DUTY         0x03U
#define PWM_E_PARAM_POINTER      0x04U
#define PWM_E_UNINIT             0x05U

/* Runtime / Hardware Errors */
#define PWM_E_HW_INIT_FAILED     0x20U
#define PWM_E_HW_CONFIG_FAILED   0x21U
#define PWM_E_HW_START_FAILED    0x22U
#define PWM_E_HW_STOP_FAILED     0x23U

#endif
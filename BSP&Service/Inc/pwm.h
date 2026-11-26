#ifndef PWM_H
#define PWM_H
#include "pwm_cfg.h"
#define PWM1_INSTANCE_ID PWM_PWM1_INSTANCE_ID
#define PWM1_CHANNEL_NUMBER 2
#include <stdint.h>

enum pwm_channel_state
{
	OFF,
	ON,
};

typedef struct _pwm_channel_t{
	uint8_t channel_id;
	const char* name;
	uint8_t duty;
	uint32_t frequency;
	uint8_t state;
} channel_t;


// module init
void pwm_init(void);

void pwm_deinit(void);

void pwm_on(void);
void pwm_off(void);

void pwm_start(uint8_t channel_id);
void pwm_stop(uint8_t channel_id);

void pwm_setduty(uint8_t channel, uint8_t duty); // 0%~100%
uint8_t pwm_getduty(uint8_t channel);
void pwm_get_state(uint8_t channel_id,channel_t* c);
void pwm_init_state(channel_t* c, uint8_t channel_id,	const char* name,	uint8_t duty,	uint32_t frequency,	uint8_t state);


#endif
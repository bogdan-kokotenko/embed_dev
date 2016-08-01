//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_pwm  MSP430F5x PWM APIs
//! \brief MSP430F5x PWM APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/pwm.h
//! \brief  MSP430 timer based PWM API
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------
//!  11/04/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "pwm_config.h"

// Define timers output modes
#define TIMER_TOGGLE        OUTMOD_4    //!< output: toggle
#define TIMER_SET_RESET     OUTMOD_3    //!< output: PWM set/reset
#define TIMER_RESET_SET     OUTMOD_7    //!< output: PWM reset/set    
    
//! Get PWM0 source clock frequency
#define PWM0_clk()   PWM0_FREQ 
    
//! TIMER based PWM initialization
void PWM0_init(void);    
    
//! Start PWM0 generation
void PWM0_start(void);
//! Stop PWM0 generation
void PWM0_stop(void);
//! Set PWM0 parameters
void PWM0_set(uint16_t period, uint16_t duty1, uint16_t duty2);

//! Get output state of PWM0
bool PWM0_isEnabled(void);
//! Get period of PWM0
uint16_t PWM0_period(void);
//! Get duty cycle of PWM0 ch1
uint16_t PWM0_duty1(void);
//! Get duty cycle of PWM0 ch2
uint16_t PWM0_duty2(void);

//! Get PWM1 source clock frequency
#define PWM1_clk()   PWM1_FREQ 
    
//! TIMER based PWM1 initialization
void PWM1_init(void);    
    
//! Start PWM1 generation
void PWM1_start(void);
//! Stop PWM1 generation
void PWM1_stop(void);
//! Set PWM1 parameters
void PWM1_set(uint16_t period, uint16_t duty1, uint16_t duty2);

//! Get output state of PWM1
bool PWM1_isEnabled(void);
//! Get period of PWM1
uint16_t PWM1_period(void);
//! Get duty cycle of PWM1 ch1
uint16_t PWM1_duty1(void);
//! Get duty cycle of PWM1 ch2
uint16_t PWM1_duty2(void);

#ifdef __cplusplus
}
#endif

#endif	//PWM_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

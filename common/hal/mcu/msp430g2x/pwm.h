//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_pwm  MSP430G2x PWM APIs
//! \brief MSP430G2x PWM APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/pwm.h
//! \brief      MSP430 timer based PWM API			
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

//! Get PWM source clock frequency
#define PWM_clk()   PWM_FREQ 
    
//! TIMER1 based PWM initialization
void PWM_init(void);    
    
//! Start PWM generation (based on TIMER1)
void PWM_start(void);
//! Stop PWM generation (based on TIMER1)
void PWM_stop(void);
//! Set PWM parameters (based on TIMER1)
void PWM_set(uint16_t period, uint16_t duty1, uint16_t duty2);

//! Get output state of PWM
bool PWM_isEnabled(void);
//! Get period of PWM
uint16_t PWM_period(void);
//! Get duty cycle of PWM ch1
uint16_t PWM_duty1(void);
//! Get duty cycle of PWM ch2
uint16_t PWM_duty2(void);

#ifdef __cplusplus
}
#endif

#endif	//PWM_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

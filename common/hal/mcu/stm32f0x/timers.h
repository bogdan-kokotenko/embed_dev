//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup stm32f0x_hal
//! @{
//! \defgroup stm32f0x_timers  STM32F0x Timers APIs
//! \brief STM32F0x timers APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/stm32f0x/timers.h
//! \brief  STM32F0x timers API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef TIMERS_H
#define TIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include STD definitions
#include <time.h>
    
// Include configurations
#include "timers_config.h"

//! Returns the approximate processor time since restart as a value of clock_t
//clock_t clock(void);  

//! Watchdog timer initialization
void WDT_init(void);

//! Clear watchdog timer to prevent time out reset
void WDT_feedWatchdog(void);

//! SysTick timer initialization
void SysTick_init(uint32_t tickInterval);

//! TIMER0 initialization (TA0)
void TIMER0_init(void);

//! TIMER0_1 initialization (TA0 CCR1)
//void TIMER0_init1(uint16_t period);

//! TIMER0_2 initialization (TA0 CCR2)
//void TIMER0_init2(uint16_t period);

//! TIMER1 initialization (TA1)
//void TIMER1_init(void);
//! TIMER1 set CCR0, CCR1, CCR2
//void TIMER1_set(uint16_t period, uint16_t duty1, uint16_t duty2);
//! TIMER1 start (count up to CCR0)
//void TIMER1_start(void);
//! TIMER1 stop
//void TIMER1_stop(void);

//! Get output state of TIMER1
//bool TIMER1_isEnabled(void);
//! Get period of TIMER1
//uint16_t TIMER1_period(void);
//! Get duty cycle of TIMER1 CCR1
//uint16_t TIMER1_duty1(void);
//! Get duty cycle of TIMER1 CCR2
//uint16_t TIMER1_duty2(void);

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_timers  MSP430G2x Timers APIs
//! \brief MSP430G2x Timers APIs
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/msp430g2x/timers.h
//! \brief  MSP430 Timers API			
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------
//!  09/02/2015 | Bogdan Kokotenko | Initial draft
//!  09/02/2015 | Bogdan Kokotenko | Added new APIs for TIMER1
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
clock_t clock(void);  

//! Watchdog timer initialization
void WDT_init(void);

//! Clear watchdog timer to prevent time out reset
void WDT_feedWatchdog(void);

//-------------------------------- TIMER0 --------------------------------------
//! SysTick (TIMER0) initialization
void SysTick_init(uint16_t period);

//! TIMER0_1 initialization (TA0 CCR1)
void TIMER0_init1(uint16_t period, void (*handler)(void));
//! TIMER0_1 stop (TA0, CCR1)
void TIMER0_stop1(void);

//! TIMER0_2 initialization (TA0 CCR2)
void TIMER0_init2(uint16_t period, void (*handler)(void));
//! TIMER0_2 stop (TA0, CCR2)
void TIMER0_stop2(void);

//-------------------------------- TIMER1 --------------------------------------
//! TIMER1 initialization (TA1)
void TIMER1_init(void);

//! TIMER1_0 initialization (TA1 CCR0)
void TIMER1_init0(uint16_t period, void (*handler)(void));
//! TIMER1_0 stop (TA1, CCR0)
void TIMER1_stop0(void);

//! TIMER1_1 initialization (TA1 CCR1)
void TIMER1_init1(uint16_t period, void (*handler)(void));
//! TIMER1_1 stop (TA1, CCR1)
void TIMER1_stop1(void);

//! TIMER1_2 initialization (TA1 CCR2)
void TIMER1_init2(uint16_t period, void (*handler)(void));
//! TIMER1_2 stop (TA1, CCR2)
void TIMER1_stop2(void);

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

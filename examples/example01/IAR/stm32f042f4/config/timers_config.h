//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_stm32f0x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/stm32f042f4/config/timers_config.h
//! \brief  STM32F0x timers configuration			
//
//******************************************************************************
#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"    
    
//! Clear default system definition
#undef CLOCKS_PER_SEC

//! Equal to the number of clock ticks per second
//! POSIX defines CLOCKS_PER_SEC as one million
#define CLOCKS_PER_SEC      TCLK_FREQ

//! BASE TIMER initialization ( used by clock() )
#define BASETIMER_init()    SysTick_init() 

//! Enable WDT in reset WDT_RST mode
//!
//! Supported options: 8~6552 ms (depends on temperature), commented (disabled)
//! \sa WDT_init(), WDT_feedWatchdog()
#define WDT_RST             1000    // ms

//! Timer0 clock frequency
//! Supported options: TCLK_FREQ  with dividers 2,4,8
#define T0CLK_FREQ          (TCLK_FREQ)

//! Timer1 clock frequency
//! Supported options: TCLK_FREQ  with dividers 2,4,8
#define T1CLK_FREQ          TCLK_FREQ

//------------------------------------------------------------------------------
// Callbacks section

//! Systick overflow handler occupied by clock_t
#define Systick_OverflowHandler()       STIMER_tick()
    
//! TIMER0 CCR1 handler
//#define TIMER0_Handler1()
//! TIMER0 CCR2 handler
//#define TIMER0_Handler2() 
    
//! TIMER1 overflow handler
//#define TIMER1_Handler()          

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430g2x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/msp430g2553/config/timers_config.h
//! \brief  MSP430G2x timers configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  09/02/2015 | Bogdan Kokotenko | Initial draft
//!  15/11/2015 | Bogdan Kokotenko | Added TIMER1 output options
//
//******************************************************************************
#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Enable WDT in reset (WDT_RST) or interval (WDT_INTERVAL) mode
//!
//! Supported options: WDT_RST, WDT_INTERVAL, commented (disabled)
//! \note WDT reset mode interval is 400~1600 ms (about 800ms for 20*C)
//! 	WDT interval mode is 1000, ~33, ~16 (depends on avaliable clock source)  
//! \sa WDT_init(), WDT_Handler(), WDT_clear()
//#define WDT_RST     800 

//! Timer0 clock frequency
//! Supported options: SMCLK_FREQ, ACLK_FREQ  with dividers 2,4,8
#define T0CLK_FREQ  (ACLK_FREQ)

//! Enable TIMER1
//#define TIMER1_EANBLED
    
//! Timer1 clock frequency
//! Supported options: SMCLK_FREQ, ACLK_FREQ  with dividers 2,4,8
#define T1CLK_FREQ  SMCLK_FREQ

//------------------------------------------------------------------------------
//! BASE TIMER initialization ( used by clock() )
#define BASETIMER_init()    TIMER0_init() 

//! Set the number of clock ticks per second
#undef CLOCKS_PER_SEC                   //!< Clear default system definition
#define CLOCKS_PER_SEC  (T0CLK_FREQ)    //!< New value of clock ticks per second
     
//------------------------------------------------------------------------------
// Callbacks section

//! WDT handler
//#define WDT_Handler()

//! SysTick TIMER0 CCR0 handler
#define SysTick_Handler0()  STIMER_tick()
    
// TIMER0 overflow handler
//#define TIMER0_Overflow()

//! TIMER1 overflow handler
//#define TIMER1_OvfHandler()          

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

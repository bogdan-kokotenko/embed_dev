//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430f5x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/msp430f5438/config/timers_config.h
//! \brief  MSP430F5438A Timers configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  12/01/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Enable WDT in reset (WDT_RST) or interval (WDT_INTERVAL) mode
//!
//! WDT reset mode interval is 400~1600 ms (about 800ms for 20*C)
//! WDT interval mode is 1000, ~33, ~16 (depends on avaliable clock source)  
//! \sa WDT_init(), WDT_Handler(), WDT_clear()
#define WDT_RST     800 

//------------------------------------------------------------------------------
//! Timer0 clock frequency
#define T0CLK_FREQ  (SMCLK_FREQ/16)

//! Set the number of clock ticks per second
#undef CLOCKS_PER_SEC                   //!< Clear default system definition
#define CLOCKS_PER_SEC  (T0CLK_FREQ)    //!< New value of clock ticks per second
    
//------------------------------------------------------------------------------
//! Enable TIMER1
//#define TIMER1_EANBLED
    
//! Timer1 clock frequency
#define T1CLK_FREQ  (SMCLK_FREQ)

//------------------------------------------------------------------------------
//! Enable TIMER2
//#define TIMER2_EANBLED

//! Timer2 clock frequency
#define T2CLK_FREQ (SMCLK_FREQ)

//------------------------------------------------------------------------------
//! Timers, which avaliable on the specified MCU
#if (defined(__MSP430F5329__))

//! Timer3 clock frequency
#define T3CLK_FREQ (SMCLK_FREQ)

// Timer2 output initializations
//              | TA2.0 | TA2.1 | TA2.2        
// -------------|-------|-------|-------
// MSP430F5329  | P2.3  | P2.4  | P2.5  
    
//#define TA2_0_OUT_PIN             //!< TA2.0 ->
//#define TA2_1_OUT_PIN             //!< TA2.1 ->
//#define TA2_2_OUT_PIN             //!< TA2.2 ->

// Set CCR modes   
// Options: TIMER_TOGGLE, TIMER_TOGGLE_RESET, TIMER_TOGGLE_SET,
//          TIMER_SET_RESET, TIMER_RESET_SET
//#define TA2_CCR0_MODE   
//#define TA2_CCR1_MODE     
//#define TA2_CCR2_MODE
    
#endif

//------------------------------------------------------------------------------
// Callbacks section

//! WDT handler
//#define WDT_Handler()

//! SysTick handler
#define SysTick_Handler()   STIMER_tick()
    
// TIMER0 overflow handler
//#define TIMER0_Overflow()

//! TIMER1 overflow handler
//#define TIMER1_OvfHandler()          

//! TIMER1 CCR0 handler
//#define TIMER1_Handler0()

//! TIMER2 overflow handler
//#define TIMER2_OvfHandler()          

//! TIMER2 CCR0 handler
//#define TIMER2_Handler0()

//! TIMER3 overflow handler
//#define TIMER3_OvfHandler()          

//! TIMER3 CCR0 handler
//#define TIMER3_Handler0()

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

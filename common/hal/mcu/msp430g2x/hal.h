//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \defgroup msp430g2x_hal MSP430G2x HAL
//! \brief  The MSP430G2x HAL provides the APIs for 
//!         the microcontroller peripherals
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/msp430g2x/hal.h
//! \brief  MSP430G2x hardware abstraction layer definitions			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  19/02/2015 | Bogdan Kokotenko | Initial draft
//!  20/11/2015 | Bogdan Kokotenko | Fixed low-power mode selection
//
//******************************************************************************
#ifndef HAL_H
#define HAL_H

// Hardware specific include
#include "msp430.h"

// Include configurations
#include "hal_config.h"

//! MCU core HAL definition
#define _MSP430G2X_HAL_

#if defined(USE_LOW_POWER_MODE_CRYSTAL)
//! Low power mode definition
#define USE_LOW_POWER_MODE

//! \brief Enter LPM3 low-power mode
//! \hideinitializer
#define LPM_enable()    __bis_SR_register(GIE+LPM3_bits)

//! \brirf Exit low-power mode (called from ISR)
//! \hideinitializer
#define LPM_disable()   __bic_SR_register_on_exit(LPM3_bits)

#elif defined(USE_LOW_POWER_MODE_DCO)
//! Low power mode definition
#define USE_LOW_POWER_MODE

//! \brief Enter LPM0 low-power mode
//! \hideinitializer
#define LPM_enable()    __bis_SR_register(GIE+LPM0_bits)

//! \brief Exit low-power mode (called from ISR)
//! \hideinitializer
#define LPM_disable()   __bic_SR_register_on_exit(LPM0_bits)

#endif

//! \brief Enable global interrupts
//! \hideinitializer
#define MCU_enableInterrupts()  __enable_interrupt()

//! \brief Call soft reset
//! \details Write to WDT without PWD cause RST 
//! \hideinitializer
#define MCU_reset()     WDTCTL = 0

//! \brief Macro function for entering critical section 
//! \details (Save the current global maskable interrupt flag state, and disabling it)
//! \sa LeaveCriticalSection(), LeaveCriticalSectionAndSuspend()
//! \hideinitializer
#define EnterCriticalSection()                  \
{                                               \
    extern uint16_t GINT_criticalNesting;       \
    __disable_interrupt();                      \
    GINT_criticalNesting++;                     \
}

//! \brief Macro function for leaving critical section 
//! \details (Restore the global maskable interrupt flag from the last state)
//! \sa EnterCriticalSection(), LeaveCriticalSectionAndSuspend()
//! \hideinitializer
#define LeaveCriticalSection()                  \
{                                               \
    extern uint16_t GINT_criticalNesting;       \
    GINT_criticalNesting--;                     \
	if( GINT_criticalNesting == 0 )             \
		__enable_interrupt();                   \
}

//! \brief Macro function for leaving critical section with further suspend
//! \sa EnterCriticalSection(), LeaveCriticalSection()
//! \hideinitializer
#define LeaveCriticalSectionAndSuspend()        \
{                                               \
    extern uint16_t GINT_criticalNesting;       \
    GINT_criticalNesting--;                     \
	if( GINT_criticalNesting == 0 )             \
        LPM_enable();                           \
}  

//! @}
#endif // HAL_H
//******************************************************************************
// End of file
//******************************************************************************

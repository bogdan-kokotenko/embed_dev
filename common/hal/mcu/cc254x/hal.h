//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \defgroup cc254x_hal CC254x HAL
//! \brief  The CC254x HAL provides the APIs for 
//!         the microcontroller peripherals
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/cc254x/hal.h
//! \brief  CC254x hardware abstraction layer definitions			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef HAL_H
#define HAL_H

#ifndef _SYSTEM_BUILD
#pragma system_include
#endif

// Hardware specific include
#include <ioCC254x_bitdef.h>
// Include device specific file.
#if defined (__CC2541__)
#include "ioCC2541.h"
#elif defined (__CC2543__)
#include "ioCC2543.h"
#elif defined (__CC2544__)
#include "ioCC2544.h"
#elif defined (__CC2545__)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif

// Include configurations
#include "hal_config.h"

//! MCU core HAL definition
#define _CC254x_HAL_

#ifndef __no_operation
//! No operation intrinsic
#define __no_operation()    asm("NOP")
#endif

#ifndef __enable_interrupt
//! Global interrupt enable intrinsic
#define __enable_interrupt()    EA = 1
#endif

#ifndef __disable_interrupt
//! Global interrupt enable intrinsic
#define __disable_interrupt()   EA = 0
#endif

//! \brief Enter low-power mode
//! \hideinitializer
#define LPM_enable()            PCON |= PCON_IDLE

//! \brirf Exit low-power mode (called from ISR)
//! \hideinitializer
#define LPM_disable()

//! \brief Enable global interrupts
//! \hideinitializer
#define MCU_enableInterrupts()  __enable_interrupt()

//! \brief Call soft reset
//! \details Write to WDT without PWD cause RST 
//! \hideinitializer
#define MCU_reset()

//! \brief Macro function for entering critical section 
//! \details Save the global maskable interrupt flag state and disable it
//! \sa LeaveCriticalSection(), LeaveCriticalSectionAndSuspend()
//! \hideinitializer
#define EnterCriticalSection()                      \
{                                                   \
    extern __data uint16_t GINT_criticalNesting;   \
    __disable_interrupt();                          \
    GINT_criticalNesting++;                         \
}

//! \brief Macro function for leaving critical section 
//! \details Restore the global maskable interrupt flag from the last state
//! \sa EnterCriticalSection(), LeaveCriticalSectionAndSuspend()
//! \hideinitializer
#define LeaveCriticalSection()                      \
{                                                   \
    extern __data uint16_t GINT_criticalNesting;   \
    GINT_criticalNesting--;                         \
	if( GINT_criticalNesting == 0 )                 \
		__enable_interrupt();                       \
}

//! \brief Macro function for leaving critical section with further suspend
//! \sa EnterCriticalSection(), LeaveCriticalSection()
//! \hideinitializer
#define LeaveCriticalSectionAndSuspend()            \
{                                                   \
    extern __data uint16_t GINT_criticalNesting;   \
    GINT_criticalNesting--;                         \
	if( GINT_criticalNesting == 0 )                 \
        LPM_enable();                               \
}

//! @}
#endif // HAL_H
//******************************************************************************
// End of file
//******************************************************************************

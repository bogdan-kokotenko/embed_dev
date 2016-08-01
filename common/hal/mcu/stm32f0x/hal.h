//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup stm32f0x_hal  STM32F0x HAL
//! \brief The STM32F0x HAL provides the APIs for the microcontroller peripherals
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/stm32f0x/hal.h   
//! \brief  STM32F0x hardware abstraction layer definitions			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef HAL_H
#define HAL_H

// Hardware specific include
#include "stm32f0xx.h"
#include "core_cm0.h"

// Include configurations
#include "hal_config.h"

//! MCU core HAL definition
#define _STM32F0X_HAL_

//! \brief Enable global interrupts
//! \hideinitializer
#define MCU_enableInterrupts()  __enable_interrupt()

//! \brief Call soft reset
//! \hideinitializer
#define MCU_reset()             NVIC_SystemReset()

//! \brief Macro function for entering critical section 
//! \details (Check the global maskable interrupt flag and disable it)
//! \sa LeaveCriticalSection(), LeaveCriticalSectionAndSuspend()
//! \hideinitializer
#define EnterCriticalSection()                  \
{                                               \
    extern uint16_t GINT_criticalNesting;       \
    __disable_interrupt();                      \
    GINT_criticalNesting++;                     \
	__DSB();                                    \
	__ISB();                                    \
}

//! \brief Macro function for leaving critical section 
//! \details (Restor the global maskable interrupt flag from the last state)
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
    {                                           \
        /* would act as a NOP if INT is pending*/\
        __WFI();                                \
		__enable_interrupt();                   \
    }                                           \
}  

//! @}
#endif // HAL_H
//******************************************************************************
// End of file
//******************************************************************************

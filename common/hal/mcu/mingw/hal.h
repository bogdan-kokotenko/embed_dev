//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup mingwhal MinGW HAL
//! \brief The MinGW HAL provides the APIs which simulate the MCU peripherals
//! @{
//******************************************************************************
//   File description:
//! \file           hal\mcu\mingw\hal.h   	
//! \brief          MinGW hardware abstraction layer definitions
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

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "hal_config.h"

//! MCU core HAL definition
#define _MINGW_HAL_

//! Enter sleep mode
void LPM_enable(void);

//! Exit low-power mode (called from ISR)
void LPM_disable(void);

//! Enable global interrupts
void MCU_enableInterrupts(void);

//! Call soft reset
void MCU_reset(void);

//! Enter critical section
void EnterCriticalSection(void);

//! Leave critical section
void LeaveCriticalSection(void);

//! Leave critical section with further suspend
void LeaveCriticalSectionAndSuspend(void);

#ifdef __cplusplus
}
#endif

//! @}
#endif // HAL_H_
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup mingwhal
//! @{
//******************************************************************************
//	File description:
//! \file       hal\mcu\mingw\timers.h
//! \brief      MinGW timers API			
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

//! Watchdog timer initialization
void WDT_init(void);

//! Clear watchdog timer to prevent time out reset
void WDT_feedWatchdog(void);

//! SysTick timer initialization
void SysTick_init(uint32_t tickInterval);

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

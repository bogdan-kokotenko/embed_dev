//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup cc254x_hal
//! @{
//! \defgroup cc254x_timers  CC254x Timers APIs
//! \brief CC254x Timers APIs
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/cc254x/timers.h
//! \brief  CC254x Timers API			
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

#ifdef __cplusplus
}
#endif

#endif	//TIMERS_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

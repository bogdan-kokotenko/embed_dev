//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup system
//! @{
//! \defgroup stimer Software Timer
//! \brief Software timer which extends system hardware timer.
//! \details Provides task scheduling, software timers and I/O protocols.  
//! @{    
//******************************************************************************
//   File description:
//! \file       sys/stimer.h
//! \brief      Software timer APIs
//!
//! \details    Implements software timer which extend system hardware timer.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  19/03/2011 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef STIMER_H
#define STIMER_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configuration
#include "stimer_config.h"
  
//! Clear software timer schedule and its clock source.
void STIMER_init(void);

//! Update software timer (Issued by hardware timer ISR).
void STIMER_tick(void);

//! Get local time (time from restart in miliseconds).
//! \note Reset after one day passed
int32_t STIMER_timeMs(void);

//! Returns time point after timeout passed.
int32_t STIMER_nextTimeAfterMs(int32_t timeout);

//! Add new task to the software timer schedule.
bool STIMER_add(task_t handle, int32_t timeout);

//! Remove task from the schedule.
bool STIMER_remove(task_t handle);

#ifdef __cplusplus
}
#endif

#endif // STIMER_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//  File description:
//! \file       hal\mcu\mingw\hal.c
//! \brief      MinGW interrupt simulation unit
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  23/07/2016 | Bogdan Kokotenko | Initial draft
//!  24/07/2016 | Bogdan Kokotenko | Added simulation of GINT and LPM
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MINGW_HAL_) )
#warning CLOCKS: Unknown MCU core, check HAL configuration!
#else

#include <assert.h>
#include <pthread.h>

//! Global interrupt simulation mutex
//static std::recursive_mutex     GINT_mutex;
static pthread_mutex_t GINT_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

//! Condition which wait while no task to do
static pthread_cond_t  LPM_condition = PTHREAD_COND_INITIALIZER;
//! LPM condition lock
static pthread_mutex_t LPM_lock = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------
// Function:	
//              LPM_enable()
// Description:
//! \brief      Enter sleep mode
//! \details    Simulate low-power mode
//------------------------------------------------------------------------------
void LPM_enable()
{
    pthread_mutex_lock(&LPM_lock);

    pthread_cond_wait(&LPM_condition,&LPM_lock);

    pthread_mutex_unlock(&LPM_lock);
}

//------------------------------------------------------------------------------
// Function:
//              LPM_disable()
// Description:
//! \brief      Exit low-power mode (called from ISR)
//------------------------------------------------------------------------------
void LPM_disable()
{
    pthread_cond_signal(&LPM_condition);
}

//------------------------------------------------------------------------------
// Function:
//              EnterCriticalSection()
// Description:
//! \brief      Enter critical section
//! \details    Check last state of global maskable interrupt flag and disable it
//! \sa LeaveCriticalSection(), LeaveCriticalSectionAndSuspend()
//------------------------------------------------------------------------------
void EnterCriticalSection()
{
    pthread_mutex_lock(&GINT_mutex);
}

//------------------------------------------------------------------------------
// Function:
//              LeaveCriticalSection()
// Description:
//! \brief      Leave critical section
//! \details    Restore the global maskable interrupt flag from the last state
//! \sa EnterCriticalSection(), LeaveCriticalSectionAndSuspend()
//------------------------------------------------------------------------------
void LeaveCriticalSection()
{
    pthread_mutex_unlock(&GINT_mutex);
}

//------------------------------------------------------------------------------
// Function:
//              LeaveCriticalSectionAndSuspend()
// Description:
//! \brief      Leave critical section with further suspend
//! \sa EnterCriticalSection(), LeaveCriticalSection()
//------------------------------------------------------------------------------
void LeaveCriticalSectionAndSuspend()
{
    pthread_mutex_lock(&LPM_lock);

    pthread_mutex_unlock(&GINT_mutex);

    pthread_cond_wait(&LPM_condition,&LPM_lock);

    pthread_mutex_unlock(&LPM_lock);
}

//------------------------------------------------------------------------------
// Function:
//              MCU_enableInterrupts()
// Description:
//! \brief      Enable global interrupts
//! \details    Reset GINT and LPM mutex
//------------------------------------------------------------------------------
void MCU_enableInterrupts()
{
    LPM_lock = PTHREAD_MUTEX_INITIALIZER;
    GINT_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
}

//------------------------------------------------------------------------------
// Function:
//              MCU_reset()
// Description:
//! \brief      Call soft reset
//! \details    Simulate reset using assert
//------------------------------------------------------------------------------
void MCU_reset()
{
    assert(!"Device reset by WDT!!!");
}

#endif // _MINGW_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

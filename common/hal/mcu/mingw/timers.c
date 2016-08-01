//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//	File description:
//! \file       hal\mcu\mingw\timers.c
//! \brief      MinGW timers functions implementation			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------------
//!  23/07/2016 | Bogdan Kokotenko | Initial draft
//!  24/07/2016 | Bogdan Kokotenko | Added simulation of SysTick and WDT
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "devtime.h"
#include "thread.h"
#include "timers.h"

#include "timer.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MINGW_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

#include <assert.h>
#include <unistd.h>
#include <pthread.h>

//! SysTick interrupt handler prototype
static void SysTick_Handler(void);

#ifdef WDT_RST

//! WDT tick interval
#define WDT_TICK_INTERVAL       100     // ms

//! WDT reload value
#define WDT_RELOAD_VALUE        (WDT_RST/WDT_TICK_INTERVAL)

//! WDT timeout
int32_t WDT_timeout = WDT_RELOAD_VALUE;

//! WDT mutex
static pthread_mutex_t WDT_mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------
// Function:
//              WDT_thread()
// Description:
//! \brief      Watchdog timer thread
//------------------------------------------------------------------------------
void* WDT_thread(void *)
{
    // Prepare thread for fast cancelation
    if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0)
        assert(!"ERROR: WDT thread could not be configured!");

    while(true)
    {
        usleep(WDT_TICK_INTERVAL*1000);
        pthread_mutex_lock(&WDT_mutex);
        if(--WDT_timeout <=0)
            MCU_reset();
        pthread_mutex_unlock(&WDT_mutex);
    }
    return NULL;
}

//------------------------------------------------------------------------------
// Function:
//				WDT_feedWatchdog()
// Description:
//! \brief      Clear watchdog timer to prevent time out reset
//------------------------------------------------------------------------------
void WDT_feedWatchdog(void)
{
    pthread_mutex_lock(&WDT_mutex);
    WDT_timeout = WDT_RELOAD_VALUE;
    pthread_mutex_unlock(&WDT_mutex);
}
#endif

//------------------------------------------------------------------------------
// Function:	
//              WDT_init()
// Description:
//! \brief      Watchdog timer initialization
//------------------------------------------------------------------------------
void WDT_init(void)
{
    #ifdef WDT_RST
    static bool isAllocated = false;
    static pthread_t thread;

    if(isAllocated)
    {
        if(pthread_cancel(thread) != 0)
            assert(!"ERROR: WDT thread could not be canceled!");
    }

    if (pthread_create(&thread, NULL, WDT_thread, NULL) != 0)
        assert(!"ERROR: WDT thread could not be created!");
    else
        isAllocated = true;
    #endif
}

//------------------------------------------------------------------------------
// Function:
//				SysTick_thread()
// Description:
//! \brief      SysTick timer thread
//------------------------------------------------------------------------------
void SysTick_thread(void)
{
    EnterCriticalSection();
    SysTick_Handler();
    LeaveCriticalSection();
}

//------------------------------------------------------------------------------
// Function:	
//				SysTick_init()
// Description:
//! \brief      SysTick timer initialization
//------------------------------------------------------------------------------
void SysTick_init(uint32_t tickInterval)
{
    static bool isAllocated = false;

    if(isAllocated)
        timer_stop();

    if(timer_start(tickInterval, SysTick_thread))
        assert(!"ERROR: WDT thread could not be created!");
    else
        isAllocated = true;
}

//------------------------------------------------------------------------------
// Function:	
//				SysTick_Handler()
// Description:
//! \brief      SysTick timer handler
//------------------------------------------------------------------------------
void SysTick_Handler(void)
{
#ifdef Systick_OverflowHandler
    Systick_OverflowHandler();
#endif

#ifdef USE_LOW_POWER_MODE
    LPM_disable();
#endif
}

#endif // _MINGW_HAL_

//******************************************************************************
// End of file
//******************************************************************************

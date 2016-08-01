//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   sys/devtime.c
//! \brief 	Handle system time			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------
//!  15/02/2015 | Bogdan Kokotenko | Initial draft
//!  24/02/2015 | Bogdan Kokotenko | Added standard functions
//!  16/04/2015 | Bogdan Kokotenko | Added timestamp functions
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "timers.h"
#include "thread.h"
#include "devtime.h"
#include "task.h"

//! Define number of ticks in sec (1024 for 32768 Hz)
#define DEVTIME_SEC_TICKS   \
    (DEVTIME_CLK_FREQUENCY/((DEVTIME_CLK_FREQUENCY)*DEVTIME_TICK_INTERVAL/1000))

//! System local time in time_t
static time_t  DEVTIME_systemTime = 0;

#if (DEVTIME_CLK_FREQUENCY == 32768)
//! Define correction interval
#define DEVTIME_CORRECTION_INTERVAL 43      // 41,992 ms (for 32768 Hz)
//! Correct system time
static void DEVTIME_correction(void);
#endif // DEVTIME_CORRECTION_INTERVAL

//------------------------------------------------------------------------------
// Function:	
//				DEVTIME_time()
// Description:
//! \brief      Get the current calendar time as a value of type time_t
//! \details    Use instead of standard time()
//!
//! \note       The value returned generally represents the number of seconds
//!             since 00:00 hours, Jan 1, 1970 UTC (i.e., POSIX timestamp)
//!
//! \param *t   If it is not a NULL, sets the value to the pointed object.
//! \return     Return time as type of time_t. 
//------------------------------------------------------------------------------
time_t DEVTIME_time(time_t *t)
{
    if(t)
        *t = DEVTIME_systemTime;
      
    return DEVTIME_systemTime;
}

//------------------------------------------------------------------------------
// Function:	
//				DEVTIME_stime()
// Description:
//! \brief      Sets the system's idea of the time and date.
//! \details    Use instead of standard stime()
//!
//! \note       The time, pointed to by t, is measured in seconds 
//!             since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
//!
//! \param t    If it is not a NULL, sets the systemTime.
//! \return     On success, zero is returned. On error, -1 is returned. 
//------------------------------------------------------------------------------
int DEVTIME_stime(time_t *t)
{
    if(t == NULL)
        return -1;

    DEVTIME_systemTime = *t;
    return 0;
}

//------------------------------------------------------------------------------
// Function:	
//				DEVTIME_init()
// Description:
//! \brief      Initialize device time
//------------------------------------------------------------------------------
void DEVTIME_init(void)
{
#ifdef DEVTIME_RTC
    // to do...
#endif
}

//------------------------------------------------------------------------------
// Function:	
//				DEVTIME_tick()
// Description:
//! \brief      Update system time
//------------------------------------------------------------------------------
void DEVTIME_tick(void)
{
    // Count timer ticks
    static uint16_t  DEVTIME_tickCounter = 0;
   
    if(++DEVTIME_tickCounter >= DEVTIME_SEC_TICKS)
    {
        DEVTIME_tickCounter = 0;
        DEVTIME_systemTime++;
    }

    // Correct system time
    #ifdef DEVTIME_CORRECTION_INTERVAL
        DEVTIME_correction();
    #endif // DEVTIME_CORRECTION_INTERVAL
}

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
// File description:
//! \file       sys/stimer.c
//! \brief      Software timer library
//!
//! \details    Implements software timer which extends
//!             system hardware timer.
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------
//!  19/03/2011 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "timers.h"
#include "devtime.h"
#include "task.h"
#include "stimer.h"

//! Software timer context structure
static struct STIMER_Context{
    task_t  handle;                     //!< timeout function pointer
    int32_t deadline;                   //!< time when function has to be called
}STIMER_schedule[STIMER_SCHEDULE_SIZE]; //!< Software timer schedule

//! Define number of ticks in one day
#define STIMER_DAY_MS           (86400000L) // msec (24*60*60*1000)

//! System local time in msec since restart
//! Reset after one day passed
static int32_t STIMER_systemTimeMs = 0;

#if (STIMER_CLK_FREQUENCY == 32768)
//! Define correction interval
#define STIMER_CORRECTION_INTERVAL  43     // 41,992 msec(for 32768 Hz)
//! Define correction value
#define STIMER_CORRECTION_VALUE     (-1)
#endif // STIMER_CLK_FREQUENCY

//------------------------------------------------------------------------------
// Function:	
//              STIMER_init()
// Description:
//! \brief      Clear software timer schedule.
//------------------------------------------------------------------------------
void STIMER_init(void)
{
    // Avoid any interrupts
    EnterCriticalSection();
    
    // Clear all variables which reset them to the default state
    memset(STIMER_schedule, 0x00, sizeof(STIMER_schedule));
    STIMER_systemTimeMs = 0;

    // Initialize software timer
    STIMER_sourceInit();

    // Initialize device time
    #ifndef DEVTIME_RTC
        DEVTIME_init();
    #endif // DEVTIME_RTC

    // Leave critical section
    LeaveCriticalSection();
}

//------------------------------------------------------------------------------
// Function:
//				STIMER_checkSchedule()
// Description:
//! \brief      Check timeouts within the software timer schedule.
//! \detailed   Has to be called in MainLoop or put to the task queue.
//------------------------------------------------------------------------------
static void STIMER_checkSchedule(void)
{
    uint8_t index;

    // Avoid any interrupts while task queue modification
    EnterCriticalSection();

    for(index = 0; index < STIMER_SCHEDULE_SIZE; index++)
    {
        if(STIMER_schedule[index].handle)
        {
            if(STIMER_schedule[index].deadline <= STIMER_systemTimeMs)
            {
                task_t handler = STIMER_schedule[index].handle;
                STIMER_schedule[index].handle = NULL;

                LeaveCriticalSection();
                TASK_createUnique(handler);

                EnterCriticalSection();
            }
        }
    }

    // Leave critical section
    LeaveCriticalSection();
}

//------------------------------------------------------------------------------
// Function:
//				DEVTIME_tick()
// Description:
//! \brief      Update software timers (Issued by hardware timer ISR).
//------------------------------------------------------------------------------
void STIMER_tick(void)
{
    // Update system timer (msec) and its schedule
    STIMER_systemTimeMs += STIMER_LATENCY;
    if(STIMER_systemTimeMs >= STIMER_DAY_MS)
    {
        STIMER_systemTimeMs = 0;

        uint8_t index = STIMER_SCHEDULE_SIZE;
        while(--index)
            STIMER_schedule[index].deadline -= STIMER_DAY_MS;
    }

    // Correct system time
    #ifdef STIMER_CORRECTION_INTERVAL
    static uint8_t  correctionCounter = 0;
    if(++correctionCounter >= STIMER_CORRECTION_INTERVAL)
    {
        correctionCounter = 0;
        STIMER_systemTimeMs += STIMER_CORRECTION_VALUE;
    }
    #endif // STIMER_CORRECTION_INTERVAL

    // Check timers list
    TASK_createUnique(STIMER_checkSchedule);

    // Update device time
    #ifndef DEVTIME_RTC
        DEVTIME_tick();
    #endif // DEVTIME_RTC
}

//------------------------------------------------------------------------------
// Function:
//				STIMER_timeMs()
// Description:
//! \brief      Get time interval passed since restart in msec.
//! \note       Reset after one day passed.
//------------------------------------------------------------------------------
int32_t STIMER_timeMs(void)
{
    return STIMER_systemTimeMs;
}

//------------------------------------------------------------------------------
// Function:
//				STIMER_nextTimeAfterMs()
// Description:
//! \brief      Returns time point after timeout passed.
//------------------------------------------------------------------------------
int32_t STIMER_nextTimeAfterMs(int32_t timeout)
{
    int32_t next = STIMER_systemTimeMs + timeout;

    if(next >= STIMER_DAY_MS)
      next -= STIMER_DAY_MS;

    return next;
}

//------------------------------------------------------------------------------
// Function:	
//		            STIMER_add()
// Description:
//! \brief          Add new task to the software timer schedule.
//!
//! \param handle   Pointer to the timeout handler
//! \param timeout  Set the timeout
//! \return         true - in case of success, false - otherwise
//------------------------------------------------------------------------------
bool STIMER_add(task_t handle, int32_t timeout)
{
    uint8_t index;
    
    EnterCriticalSection();

    for(index = 0; index < STIMER_SCHEDULE_SIZE; index++)
    {
        if(!STIMER_schedule[index].handle)
        {
            STIMER_schedule[index].handle = handle;
            STIMER_schedule[index].deadline = STIMER_systemTimeMs + timeout;

            LeaveCriticalSection();             // leave critical section
            return true;                        // and return success
        }
    }

    // No empty slots in the schedule
    LeaveCriticalSection();                     // leave critical section
    return false;                               // and return failure
}

//------------------------------------------------------------------------------
// Function:	
//				    STIMER_remove()
// Description:
//! \brief          Remove task from the software timer schedule.
//!
//! \param handle   Pointer to the timeout function
//! \return         true - in case of success, false - otherwise
//------------------------------------------------------------------------------
bool STIMER_remove(task_t handle)
{
    uint8_t index;
    
    EnterCriticalSection();
    
    for(index = 0; index < STIMER_SCHEDULE_SIZE; index++)
    {
        if(STIMER_schedule[index].handle == handle)
        {
            STIMER_schedule[index].handle = NULL;
            
            LeaveCriticalSection();         // leave critical section
            return true;                    // and return success
        }
    }

    // No task found with such handle
    LeaveCriticalSection();                 // leave critical section
    return false;                           // and return failure
}

//******************************************************************************
// End of file
//******************************************************************************

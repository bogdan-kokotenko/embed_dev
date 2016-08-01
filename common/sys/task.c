//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file       sys/task.c
//! \brief      Simple task queue and scheduler
//!
//! \details    Implements non-preemptive task queue and scheduling.
//!             Task queue is handled in forever loop.
//!             Task schedule is hadled in sytem timer ISR. 
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------------
//!  19/03/2011 | Bogdan Kokotenko | Initial draft
//!  15/02/2015 | Bogdan Kokotenko | Fixed issue with task scheduler.
//!  15/02/2015 | Bogdan Kokotenko | Fixed issue with task self-delete.
//!  22/07/2016 | Bogdan Kokotenko | Software timers moved to separated unit.
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "timers.h"
#include "devtime.h"
#include "task.h"

//! Tasks queue structure
static struct TaskQueue{
    uint8_t first;                  //!< first item index
    uint8_t last;                   //!< last item index
    uint8_t count;                  //!< number of items in the queue
    task_t  item[TASK_QUEUE_SIZE];  //!< task item list
}TASK_queue;                        //!< Task queue

//! current task handle
static task_t  TASK_current;

//------------------------------------------------------------------------------
// Function:	
//              TASK_init()
// Description:
//! \brief      Initilize (clear) task queue and schedule.
//------------------------------------------------------------------------------
void TASK_init(void)
{
    // Clear all variables which reset them to the default state
  	memset(&TASK_queue, 0x00, sizeof(TASK_queue));
}

//------------------------------------------------------------------------------
// Macro:
//				TASK_QUEUE_next()
// Description:
//! \brief      Take next task from main task queue
//!             Has to be called within critical section
//------------------------------------------------------------------------------
#define TASK_QUEUE_next(NextItem)                                              \
{                                                                              \
    /* Check if queue is empty */                                              \
    if(TASK_queue.count == 0)                                                  \
        NextItem = NULL;                        /* return NULL pointer */      \
    else                                                                       \
    {                                                                          \
        /* Take first item */                                                  \
        NextItem = TASK_queue.item[TASK_queue.first];                          \
        TASK_queue.item[TASK_queue.first] = NULL;                              \
                                                                               \
        /* Shift queue index to next item */                                   \
        TASK_queue.first++;                                                    \
        if (TASK_queue.first >= TASK_QUEUE_SIZE)/* if queue border reached */  \
            TASK_queue.first = 0;               /* roll to the beginning */    \
        TASK_queue.count--;                                                    \
    }                                                                          \
}

//------------------------------------------------------------------------------
// Function:	
//              TASK_runScheduler()
// Description:     
//! \brief      Run infinite loop to execute tasks.
//!             Replaces MAIN LOOP and RTOS.
//------------------------------------------------------------------------------
void TASK_runScheduler(void)
{
    while(true)                     // LOOP FOREVER
    {
        // Avoid any interrupts while task queue modification 
        EnterCriticalSection();

        #ifdef WDT_RST
        // Feed watchdog to prevent reset
        // and make sure that no task hangs the CPU
        WDT_feedWatchdog();
        #endif

        // Take next task handle
        TASK_QUEUE_next(TASK_current);
        
        // If handle is valid ..
        if(TASK_current)
        {   // .. leav critical section and execute task
            LeaveCriticalSection();
            TASK_current();
        }
        else 
        {   
            #ifdef USE_LOW_POWER_MODE
            // If no task to do, switch of CPU
            // Leaving critical section and enter to suspend has to be atomic
            // Otherwise last task may be delayed till wake-up
                LeaveCriticalSectionAndSuspend();
            #else
                LeaveCriticalSection();
            #endif
        }
    }
}

//------------------------------------------------------------------------------
// Function:
//              TASK_getCurrent()
// Description:
//! \brief      Return current task handle.
//------------------------------------------------------------------------------
task_t TASK_getCurrent(void)
{
    return TASK_current;
}

//------------------------------------------------------------------------------
// Function:
//              TASK_getQueueSize()
// Description:
//! \brief      Current size of task queue.
//------------------------------------------------------------------------------
uint8_t TASK_getQueueSize(void)
{
    return TASK_queue.count;
}

//------------------------------------------------------------------------------
// Function:
//				TASK_isQueued()
// Description:
//! \brief      Check if this task has been already enqueued
//------------------------------------------------------------------------------
static bool TASK_isQueued(task_t handle)
{
    // Avoid any interrupts while task queue modification
    EnterCriticalSection();

    uint8_t index;
    for(index = 0; index < TASK_queue.count; index++)
    {
        // Shift queue index to next item
        uint8_t next = TASK_queue.first + index;
        if (next >= TASK_QUEUE_SIZE)    // if index reached queue border
            next -= TASK_QUEUE_SIZE;	// roll to the beginning

        if(TASK_queue.item[next] == handle)
        {
            LeaveCriticalSection();
            return true;
        }
    }

    LeaveCriticalSection();
    return false;
}

//------------------------------------------------------------------------------
// Function:
//				    TASK_createUnique()
// Description:
//! \brief          Put task to the queue if task is not present.
//!
//! \param handle   Pointer to the task function
//! \return         true - in case of success, false - otherwise
//------------------------------------------------------------------------------
bool TASK_createUnique(task_t handle)
{
    // Avoid any interrupts while task queue modification
    EnterCriticalSection();

    // Check if this task has been already enqueued
    if(TASK_isQueued(handle))
    {
        LeaveCriticalSection();         // leave critical section
        return false;                   // and return failure
    }

    // Create task
    bool result = TASK_create(handle);

    LeaveCriticalSection();             // leave critical section
    return result;                      // and return success
}

//------------------------------------------------------------------------------
// Function:	
//				    TASK_QUEUE_create()
// Description:
//! \brief          Put task to the queue.
//!                 Since the scheduler is non-preemptive, task has to release 
//!                 the CPU as soon as possible.
//!
//! \param handle   Pointer to the task function
//! \return         true - in case of success, false - otherwise
//------------------------------------------------------------------------------
bool TASK_create(task_t handle)
{
    // Avoid any interrupts while task queue modification 
    EnterCriticalSection();

    // Check if queue is full
    if(TASK_queue.count >= TASK_QUEUE_SIZE)
    {
        LeaveCriticalSection();         // leave critical section
        return false;                   // and return failure
    }
    
    // Save item to the queue
	TASK_queue.item[TASK_queue.last] = handle;
    TASK_queue.count++;
    
    // Shift queue index to next item
    TASK_queue.last++;         
    if (TASK_queue.last >= TASK_QUEUE_SIZE)// if index reached queue border
        TASK_queue.last = 0;		    // roll to the beginning
	
    LeaveCriticalSection();             // leave critical section
    return true;                        // and return success
}

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup system Framework Kernel
//! \brief The Framework Kernel contains basic system functions
//! \details Provides task scheduling, software timers and I/O protocols.  
//! @{
//! \defgroup threads Protothreads
//! \brief Custom protothreads
//! \details Original implementation provided by Adam Dunkels.  
//! @{
//******************************************************************************
//   File description:
//! \file       sys/thread.h 
//! \brief      Custom protothread APIs
//!
//! \details    Implements non-preemptive threads.
//!             Uses task queue and software timers.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  19/03/2011 | Bogdan Kokotenko | Initial draft
//!  15/02/2015 | Bogdan Kokotenko | Code refactoring
//!  26/10/2015 | Bogdan Kokotenko | Improved by prothread (Adam Dunkels) 
//!  25/07/2016 | Bogdan Kokotenko | Protothread moved to separated file
//
//******************************************************************************
#ifndef THREAD_H
#define THREAD_H

// Include dependencies
#include "lc.h"
#include "task.h"
#include "stimer.h"

#ifdef __cplusplus
extern "C" {
#endif

//! Declare the start of a protothread inside the C function
//! implementing the protothread.
//! \hideinitializer
#define THREAD_BEGIN(h)                     \
{                                           \
    static lc_t THREAD_lc = 0;              \
    char THREAD_yieldFlag = 1;              \
    LC_RESUME(THREAD_lc)

//! Declare the end of a protothread.
//! \hideinitializer
#define THREAD_END()                        \
    LC_END(THREAD_lc);                      \
    LC_INIT(THREAD_lc);                     \
    return;                                 \
}

//! Return current protothread handle
//! \hideinitializer
#define THREAD_CURRENT()                    \
    TASK_getCurrent()

//! Resume execution of waiting protothread.
//! \note Must not be applied for the current protothread.
//! \param h Pointer to the task function.
//! \hideinitializer
#define THREAD_RESUME(h)                    \
    TASK_createUnique(h))

//! Start the protothread.
//! \hideinitializer
#define THREAD_START(h)                     \
    TASK_createUnique(h)

//! Restart the protothread.
//! \hideinitializer
#define THREAD_RESTART()                    \
do{                                         \
    LC_INIT(THREAD_lc);                     \
    TASK_createUnique(THREAD_CURRENT());    \
    return;                                 \
} while(false)

//! Finish current thread execution at the specified point.
//! \note Task state machine is reloaded.
//! \hideinitializer
#define THREAD_FINISH()                     \
do{                                         \
    LC_INIT(THREAD_lc);                     \
    return;                                 \
} while(false)

//! Yield from the current protothread.
//! Task will be automatically resumed after other processing.
//! \hideinitializer
#define THREAD_YIELD()                      \
do{                                         \
    THREAD_yieldFlag = 0;                   \
    LC_SET(THREAD_lc);                      \
    if(THREAD_yieldFlag == 0){              \
        TASK_createUnique(THREAD_CURRENT());\
        return;                             \
    }                                       \
}while(false)

//! Block protothread and wait till timeout or some event resumes this task.
//! \param t Time to wait in ms.
//! \sa THREAD_WAIT_FOREVER(), THREAD_WAIT_TIMEOUT()
//! \hideinitializer
#define THREAD_WAIT(t)                      \
do{                                         \
    THREAD_yieldFlag = 0;                   \
    LC_SET(THREAD_lc);                      \
    if(THREAD_yieldFlag == 0){              \
        STIMER_add(THREAD_CURRENT(),t);     \
        return;                             \
    }                                       \
}while(false)

//! Block protothread and wait till some event resumes this task.
//! Task must be set by another task or ISR 
//! \sa  THREAD_WAIT(), THREAD_WAIT_TIMEOUT()
//! \hideinitializer
#define THREAD_WAIT_FOREVER()               \
do{                                         \
    THREAD_yieldFlag = 0;                   \
    LC_SET(THREAD_lc);                      \
    if(THREAD_yieldFlag == 0){              \
        return;                             \
    }                                       \
}while(false)

//! Check if last THREAD_WAIT() resumed by timeout
//! Must be called once after THREAD_WAIT()
//! \param h Pointer to the task function.
//! \return Timeout state.
//! \sa THREAD_WAIT(), THREAD_WAIT_FOREVER()
//! \hideinitializer
#define THREAD_CHECK_TIMEOUT(h) (STIMER_remove(h) == 0)
        
#ifdef __cplusplus
}
#endif

#endif // THREAD_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

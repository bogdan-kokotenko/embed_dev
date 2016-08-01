//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup system
//! @{
//! \defgroup task Tasklets
//! \brief Simple tasklets
//! @{
//******************************************************************************
//   File description:
//! \file       sys/task.h
//! \brief      Simple task (tasklet) scheduler APIs
//!
//! \details    Implements non-preemptive tasklet queue.
//!             Tasklet queue is handled in forever loop.
//!
//!             Tasklets are deferrable functions which are called
//!             outside the ISRs and has to be executed fast and
//!
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  19/03/2011 | Bogdan Kokotenko | Initial draft
//!  15/02/2015 | Bogdan Kokotenko | Code refactoring
//!  26/10/2015 | Bogdan Kokotenko | Improved by prothread (Adam Dunkels) 
//!  25/07/2016 | Bogdan Kokotenko | Protothread moved to separated file
//!
//******************************************************************************
#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
extern "C" {
#endif

//! Set the maximal number of taskd in the task queue
#define TASK_QUEUE_SIZE    8

//! Task function prototype definition 
typedef void (*task_t)(void);

//! Initialize (clear) task queue.
void TASK_init(void);

//! Run infinite loop to execute tasks.
void TASK_runScheduler(void);

//! Return current task handle.
task_t TASK_getCurrent(void);

//! Current number of created tasks.
uint8_t TASK_getQueueSize(void);

//! Put task to the queue if task is not present.
bool TASK_createUnique(task_t handle);

//! Put task to the queue.
bool TASK_create(task_t handle);

#ifdef __cplusplus
}
#endif

#endif // TASK_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

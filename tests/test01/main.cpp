//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup test01 Test01
//! \brief Unit tests for task scheduler and software timers
//! \details See \ref test01/main.cpp
//******************************************************************************
//   File description:
//! \file               test01/main.cpp 
//! \brief              Contains unit tests implementation
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/06/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "timers.h"
#include "thread.h"

#include <unistd.h>
#include <pthread.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AtLeast;

//! Tasklet condition
pthread_cond_t FW_taskletCond = PTHREAD_COND_INITIALIZER;
//! Tasklet condition mutex
pthread_mutex_t FW_taskletMut = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------
// Function:
//              FW_main()
// Description:
//! \brief      Firmware start point
//------------------------------------------------------------------------------
void* FW_main(void*)
{
    // Device initialization

    // Initialize watchdog timer
    WDT_init();

    // Initialize CLOCKs
    CLK_init();

    // Enable global interrupts
    MCU_enableInterrupts();

    // Initialize device system timer (used by scheduler)
    STIMER_init();

    // Initialize task queue and schedule
    TASK_init();

    // Initialize process
    // LED_init();

    // Start scheduler which replaces MAIN LOOP and RTOS
    TASK_runScheduler();
    
    // As the scheduler has been started the firmware should never get here!
    return NULL;
}

//------------------------------------------------------------------------------
// Function:
//              FW_dummyTask()
// Description:
//! \brief      Dummy task
//------------------------------------------------------------------------------
void FW_dummyTask(void)
{
    // Notify test thread about successful tasklet execution
    pthread_mutex_lock(&FW_taskletMut);
    pthread_cond_signal(&FW_taskletCond);
    pthread_mutex_unlock(&FW_taskletMut);
}

//------------------------------------------------------------------------------
// Class:
//              SchedulerTestFixture
// Description:
//! \brief      Fixtures for SchedulerTest test case
//------------------------------------------------------------------------------
class SchedulerTestFixture : public ::testing::Test
{
protected:
    pthread_t thread;   //!< FW thread

    //! Test case setup
    void SetUp()
    {
        if (pthread_create(&thread, NULL, FW_main, NULL) != 0)
            assert(!"ERROR: FW thread could not be created!");

        // Wait while firmware thread is initializing (50 msec)
        usleep(50000);
    }

    //! Test case tear down
    void TearDown()
    {
        if(pthread_cancel(thread) != 0)
            assert(!"ERROR: FW thread could not be canceled!");

        // Wait while firmware thread is canceling (50 msec)
        usleep(50000);

    }
};

//------------------------------------------------------------------------------
// Function:
//              SchedulerTest.TASKcreateUnique_True()
// Description:
//! \brief      Check if unique tasklet is created correctly
//------------------------------------------------------------------------------
TEST(SchedulerTest, TASKcreateUnique_True)
{
    // FW simulation is not required, only tasklet queue is checked

    TASK_init();                                    // clear tasklet queue

    ASSERT_TRUE(TASK_createUnique(FW_dummyTask));   // create one unique task
    ASSERT_EQ(1, TASK_getQueueSize());              // andcheck queue size
    ASSERT_FALSE(TASK_createUnique(FW_dummyTask));  // create one more task
    ASSERT_EQ(1, TASK_getQueueSize());              // and check queue size
}

//------------------------------------------------------------------------------
// Function:
//              SchedulerTest.TASKcreate_QueueOverflow()
// Description:
//! \brief      Check if tasklet queue handles overflow correctly
//------------------------------------------------------------------------------
TEST(SchedulerTest, TASKcreate_QueueOverflow)
{
    // FW simulation is not required, only tasklet queue is checked

    TASK_init();                                    // clear tasklet queue

    for(int count = 1; count <= TASK_QUEUE_SIZE; count++)
    {
        ASSERT_TRUE(TASK_create(FW_dummyTask));     // create one task and
        ASSERT_EQ(count, TASK_getQueueSize());      // check queue size
    }
    ASSERT_EQ(TASK_QUEUE_SIZE, TASK_getQueueSize());// check queue size
    ASSERT_FALSE(TASK_create(FW_dummyTask));        // try to create task
    ASSERT_EQ(TASK_QUEUE_SIZE, TASK_getQueueSize());// check queue size
}

//------------------------------------------------------------------------------
// Function:
//              ShedulerTest.TASKcreate_waitForCall()
// Description:
//! \brief      Check if created tasklet is called function correctly
//------------------------------------------------------------------------------
TEST_F(SchedulerTestFixture, TASKcreate_waitForCall)
{
    // Get current time
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);

    // Wait for call about 2 sec
    timeout.tv_sec += 2;
    timeout.tv_nsec *= 1000;

    pthread_mutex_lock(&FW_taskletMut);

    ASSERT_TRUE(TASK_create(FW_dummyTask));         // create one task

    int retcode = pthread_cond_timedwait(&FW_taskletCond, &FW_taskletMut, &timeout);
    pthread_mutex_unlock(&FW_taskletMut);
    ASSERT_NE(ETIMEDOUT, retcode);
    ASSERT_EQ(0, retcode);
}

//------------------------------------------------------------------------------
// Function:
//              ShedulerTest.STIMERadd_waitForDeferedCall()
// Description:
//! \brief      Check if software timer calls handler function correctly
//------------------------------------------------------------------------------
TEST_F(SchedulerTestFixture, STIMERadd_waitForDeferedCall)
{
    // Create new defered task (10 sec)
    ASSERT_TRUE(STIMER_add(FW_dummyTask, 10000));

    // Get current time
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    // Wait for call about 12 sec
    struct timespec timeout;
    timeout.tv_sec = now.tv_sec + 12;
    timeout.tv_nsec = now.tv_nsec*1000;

    pthread_mutex_lock(&FW_taskletMut);
    int retcode = pthread_cond_timedwait(&FW_taskletCond, &FW_taskletMut, &timeout);
    pthread_mutex_unlock(&FW_taskletMut);
    ASSERT_NE(ETIMEDOUT, retcode);
    ASSERT_EQ(0, retcode);

    // Get current time
    struct timespec old;
    old.tv_sec = now.tv_sec;
    clock_gettime(CLOCK_REALTIME, &now);

    // Check delay
    ASSERT_NEAR(old.tv_sec+10, now.tv_sec, 1);
}

//--------------------------l----------------------------------------------------
int main(int argc, char* argv[])
{
    // Initialize Google Test Framework
    testing::InitGoogleTest(&argc, argv);
    // Run all tests
    return RUN_ALL_TESTS();
}

//******************************************************************************
// End of file
//******************************************************************************

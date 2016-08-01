//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//      			
//   File description:
//! \file       example01/MinGW/main.c
//! \brief      Contatins main(), initializations and starts main loop.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  28/01/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "timers.h"
#include "thread.h"
#include "leds.h"

//------------------------------------------------------------------------------
//! Firmware start point
int main(int argc, char* argv[])
{
    // Unused variables
    (void) argc;
    (void) argv;

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

    // Initialize system logic
    LED_init();

    // Start scheduler which replaces MAIN LOOP and RTOS
    TASK_runScheduler();
    
    // As the scheduler has been started the firmware should never get here!
    return 0;
}

//******************************************************************************
// End of file
//******************************************************************************

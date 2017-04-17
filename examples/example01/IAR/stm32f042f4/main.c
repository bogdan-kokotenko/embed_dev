//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//      			
//   File description:
//! \file       example01/IAR/stm32f030f4/main.c
//! \brief      Contatins main(), device initializations and starts main loop.
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
void main(void)
{
    // Device initialization

    // Initialize watchdog timer
    WDT_init();

    // Initialize CLOCKs
    CLK_init();

    // Initialize device system timer (used by scheduler)
    STIMER_init();

    // Initialize task queue and schedule
    TASK_init();

    // Enable global interrupts
    MCU_enableInterrupts();
    
    // Initialize system logic
    LED_init();
    
    // Start scheduler which replaces MAIN LOOP and RTOS
    TASK_runScheduler();
    
    // As the scheduler has been started the firmware should never get here!
    while(true);
}

//******************************************************************************
// End of file
//******************************************************************************

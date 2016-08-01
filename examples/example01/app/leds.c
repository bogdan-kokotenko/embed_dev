//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   example01/app/leds.c
//! \brief  LEDs handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  13/06/2013 | Bogdan Kokotenko | Initial draft
//!  26/02/2015 | Bogdan Kokotenko | Added task scheduling usage
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "thread.h"
#include "leds.h"

//------------------------------------------------------------------------------
// Function:	
//				LED_init()
// Description:
//! \brief      Initialize LEDs
//------------------------------------------------------------------------------
void LED_init(void)
{
    // Set LED pin to output
    LED0_init();

    // Start blinking
    THREAD_START(LED_update);
}

//------------------------------------------------------------------------------
// Function:	
//				LED_update()
// Description:
//! \brief      Thread which update device LEDs state
//------------------------------------------------------------------------------
void LED_update(void)
{
    /*----------------*/ THREAD_BEGIN(); /*----------------*/

    THREAD_WAIT(2000);

    // Heartbeet LED blinking
    while(true)
    {
        LED0_set();
        THREAD_WAIT(50);
        LED0_clear();

        THREAD_WAIT(100);

        LED0_set();
        THREAD_WAIT(50);
        LED0_clear();

        THREAD_WAIT(1000);
    }

    /*--------------------*/ THREAD_END(); /*--------------------*/
}

//******************************************************************************
// End of file
//******************************************************************************

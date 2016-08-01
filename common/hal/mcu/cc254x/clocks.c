//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//	File description:
//! \file   hal/mcu/cc254x/clocks.c
//! \brief  CC254x clock sources handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_CC254x_HAL_) )
#warning CLOCKS: Unknown MCU core, check HAL configuration!
#else

//! Software delay (ms)


//------------------------------------------------------------------------------
// Function:	
//              __delay_us()
// Description:
//! \brief      Busy wait function. Waits the specified number of microseconds.
//! \details    Use assumptions about number of clock cycles needed for 
//!             the various instructions. This function assumes a 32 MHz clock.
//! \note       This function is highly dependent on architecture and compiler!
//! \param usec number of microseconds delays
//------------------------------------------------------------------------------
#pragma optimize=none
void __delay_us(uint16_t usec)
{
    usec>>= 1;
    while(usec--)
    {
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
    }
}

//------------------------------------------------------------------------------
// Function:	
//              CLK_init()
// Description:
//! \brief      Busy wait function. Waits the specified number of milliseconds.
//! \details    Use assumptions about number of clock cycles needed for the
//!             various instructions.
//! \note       This function is highly dependent on architecture and compiler!
//! \param msec number of milliseconds delay
//------------------------------------------------------------------------------
#pragma optimize=none
void __delay_ms(uint16_t msec)
{
    while(msec--)
        __delay_us(1000);
}

//------------------------------------------------------------------------------
// Function:	
//              CLK_init()
// Description:
//! \brief      Initialize system clocks
//------------------------------------------------------------------------------
void CLK_init(void)
{
    // Select 32kHz clock source
    #if defined(XOSC32K)
    CLKCONCMD &= ~(CLKCON_OSC32K);
    #endif
    
    // Select system clock source
    #if defined(XOSC_FREQ)
        // Change the system clock source to HS XOSC 32 MHz
        CLKCONCMD &= ~(CLKCON_CLKSPD | CLKCON_OSC);
        #if (SYSCLK_FREQ != 32000000)
            #error System clock source does not support 32 Mzh without XOSC! 
        #endif
    // Wait until system clock source has changed to HS XOSC (CLKCON.OSC = 0).
    while(CLKCONSTA & CLKCON_OSC);
    // We need to wait approx. 2 ms until the 32 kHz RCOSC is calibrated.
    //__delay_ms(2);
    #else
        #if (SYSCLK_FREQ != 16000000)
            #error Wrong system clock source!
        #endif
    #endif
    
    // Select timer clock source
    #if (!defined(XOSC_FREQ) && TCLK_FREQ == 32000000)
        #error Timer clock does not support 32 Mzh without XOSC! 
    #endif
    
    #if (TCLK_FREQ == 32000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_32M;
    #elif (TCLK_FREQ == 16000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_16M;
    #elif (TCLK_FREQ == 8000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_8M;
    #elif (TCLK_FREQ == 4000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_4M;
    #elif (TCLK_FREQ == 2000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_2M;
    #elif (TCLK_FREQ == 1000000)
        CLKCONCMD = (CLKCONSTA & ~(CLKCON_TICKSPD)) | CLKCON_TICKSPD_1M;
    #endif
}

#endif // _CC254x_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

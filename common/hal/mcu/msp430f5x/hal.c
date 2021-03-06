//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/hal.c
//! \brief  MSP430F5x HAL functions.			
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

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning HAL: Unknown MCU core, check HAL configuration!
#else

//! Critical nesting variable.
uint16_t GINT_criticalNesting = 0;    

//------------------------------------------------------------------------------
// Function:	
//				__low_level_init()
// Description:
//! \brief      Low-level initializations
//------------------------------------------------------------------------------
int __low_level_init(void)
{
    // Return value:
    // *  1 - Perform data segment initialization.
    // *  0 - Skip data segment initialization.
    return 1;
}

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

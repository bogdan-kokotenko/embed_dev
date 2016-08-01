//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/cc254x/gpio.c
//! \brief  CC254x GPIO access functions and ISRs
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------
//!  28/06/2013 | Bogdan Kokotenko | Initial draft
//!  15/06/2013 | Bogdan Kokotenko | Commnts improved
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_CC254x_HAL_) )
#warning GPIO: Unknown MCU core, check HAL configuration!
#else


#endif  // _CC254x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

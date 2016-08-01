//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/cc254x/hal.c
//! \brief  CC254x HAL functions.			
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
#if ( !defined (_CC254x_HAL_) )
#warning HAL: Unknown MCU core, check HAL configuration!
#else

//! Critical nesting variable.
__data uint16_t GINT_criticalNesting = 0;    

#endif // _CC254x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

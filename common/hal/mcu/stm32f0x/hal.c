//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/stm32f0x/hal.c
//! \brief  STM32F0x HAL functions.			
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
#if ( !defined (_STM32F0X_HAL_) )
#warning HAL: Unknown MCU core, check HAL configuration!
#else

//! Critical nesting variable.
uint16_t GINT_criticalNesting = 0;    

//------------------------------------------------------------------------------
// Function:	
//				SystemInit()
// Description:
//! \brief      Low-level initializations
//------------------------------------------------------------------------------
void SystemInit(void)
{
    // Enabled events and all interrupts can wakeup the processor
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
}

#endif // _STM32F0X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

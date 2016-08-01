//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup stm32f0x_hal
//! @{
//! \defgroup stm32_clocks  STM32F0x Clocks APIs
//! \brief STM32F0x clocks sources APIs
//! @{
//******************************************************************************
//  File description:
//! \file   hal/mcu/stm32f0x/clocks.h
//! \brief  STM32F0x clock sources API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_H
#define CLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "clocks_config.h"

//! Internal low-speed oscillator frequency (LSI)
#define LSI_FREQ        40000       // Hz

//! Internal high-speed oscillator frequency (HSI)
#define HSI_FREQ        8000000     // Hz

//! Initialize system clocks
void CLK_init(void);

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_stm32f0x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/stm32f030f4/config/clocks_config.h
//! \brief  STM32F0x clock sources configuration			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_CONFIG_H
#define CLOCKS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! External crystal frequency
//! Supported options: 32768 Hz or 4...32 MHz
//! \note MCU configuration macro use HSE as more preferable
//#define HSE_FREQ        10000000L   // Hz

//! PLL input frequency
//! Options:    HSI_FREQ/2 (4 MHz)
//!             HSE_FREQ with divider 1...16
#define PLL_SRC         (HSI_FREQ/2)//(HSE_FREQ/2)

//! PLL frequency
//! Options:    PLL_SRC with multiplier 2..16
#define PLL_FREQ        (PLL_SRC*12)         // 10MHz/2*9 = 48MHz

//! Internal system frequency
//! Options: HSI_FREQ, HSE_FREQ, PLL_FREQ
#define SYSCLK_FREQ     (PLL_FREQ)

//! CPU/Bus clock frequency
//! Supported options: SYSCLK_FREQ with dividers 1/2/4/8/16/64/128/256
#define HCLK_FREQ       (SYSCLK_FREQ)

//! Peripheral clock frequency
//! Supported options: HCLK_FREQ with dividers 1/2/4/8/16
#define PCLK1_FREQ      (HCLK_FREQ)

//! Timers clock frequency
//! Supported options: HCLK_FREQ with dividers 1/2/4/8/16
#if (PCLK1_FREQ == HCLK_FREQ)
#define TCLK_FREQ       (PCLK1_FREQ)    
#else
#define TCLK_FREQ       (PCLK1_FREQ/2)  
#endif

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

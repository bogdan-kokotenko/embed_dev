//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430f5x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/msp430f5438/config/clocks_config.h
//! \brief  MSP430F5438A clock configuration
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  14/12/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_CONFIG_H
#define CLOCKS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Internal trimmed reference oscillator frequency
#define REFO_FREQ       32768       // Hz
//! Internal low-power oscillator frequency (VLO)
#define VLO_FREQ        12000       // Hz
// External XT1 crystal frequency
//#define XT1_FREQ      32768       // Hz
//! External XT2 crystal frequency
//#define XT2_FREQ        16000000L   // Hz

//! Internal digital oscillator frequency
#define DCO_FREQ        24000000L

//! CPU/System clock maximal frequency
#define MCLK_FREQ       24000000L   // Hz
//! Peripheral clock frequency
#define SMCLK_FREQ      24000000L   // Hz
//! Auxiliary clock frequency
#define ACLK_FREQ       32768       // Hz

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

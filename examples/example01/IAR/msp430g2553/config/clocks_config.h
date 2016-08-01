//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430g2x_config
//! @{
//******************************************************************************
//	File description:
//! \file       example01/IAR/msp430g2553/config/clocks_config.h
//! \brief      MSP430G2x clock sources configuration			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//!  26/01/2015 | Bogdan Kokotenko | Added macros for usec delay
//
//******************************************************************************
#ifndef CLOCKS_CONFIG_H
#define CLOCKS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! External XT1 crystal frequency
//! MSP430G2x support only low-frequency crystals. 
//! Option: 32768 Hz (for MSP430G2x only)
#define XT1_FREQ        32768        // Hz

//! Internal digital oscillator frequency
//! Options: 1, 8, 12, 16 MHz
#define DCO_FREQ        16000000L    // Hz

//! CPU/System clock maximal frequency
//! Supported options: XT1_FREQ, DCO_FREQ
//! \sa CPU_setPerformance()
#define MCLK_FREQ       (DCO_FREQ)  // 16 Mhz

//! Peripheral clock frequency
//! Supported options: XT1_FREQ, DCO_FREQ with dividers 1/2/4/8
#define SMCLK_FREQ      (DCO_FREQ)  // 16 MHz

//! Auxiliary clock frequency
//! Supported options: XT1_FREQ with dividers 1/2/4/8
//! If no crystal present, VLO_FREQ (~12kHz) must be used.
#define ACLK_FREQ       XT1_FREQ     // 32768Hz

//! Enable SMCLK output on P1.4
//#define SMCLK_OUT

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

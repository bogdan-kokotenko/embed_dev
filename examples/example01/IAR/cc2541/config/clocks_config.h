//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_cc254x_config
//! @{
//******************************************************************************
//	File description:
//! \file       example01/IAR/cc2541/config/clocks_config.h
//! \brief      CC2541 clock sources configuration			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_CONFIG_H
#define CLOCKS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! \brief  External XOSC crystal frequency
//! \details If commented, RCOSC (16MHz) is used the system clock source.
//! \note   RF transceiver requires the 32-MHz crystal oscillator.
#define XOSC_FREQ       32000000L   // Hz

//! \brief  External 32kHz crystal enable
//! \details If commented, internal RC oscillator is selected.
#define XOSC32K

//! \brief Select system clock source
//! \details Options: 32 MHz (XOSC) or 16 MHz (RCOSC)
#define SYSCLK_FREQ     32000000L   // Hz

//! \brief Timer clock source
//! \details Options: 32,16,8,4,2,1 MHz
#define TCLK_FREQ       32000000L   // Hz
    
#ifdef __cplusplus
}
#endif

#endif // CLOCKS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

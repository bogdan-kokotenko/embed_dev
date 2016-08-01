//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430f5x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/msp430f5438/config/leds_config.h
//! \brief  LEDs configuration
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  24/06/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H

//! \brief LED initialization
//! \hideinitializer
#define LED0_init()     SetPinOut(1,0)

//! \brief LED set
//! \hideinitializer
#define LED0_set()      SetPin(1,0)

//! \brief LED reset
//! \hideinitializer
#define LED0_clear()    ClearPin(1,0)

#endif // LEDS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_cc254x_config
//! @{
//******************************************************************************
//   File description:
//! \file       example01/IAR/cc2541/config/leds_config.h
//! \brief      LEDs GPIO configuration
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  24/06/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H

//! \brief LED initialization
//! \hideinitializer
#define LED0_init()     SetPinOut(2,0)

//! \brief LED set
//! \hideinitializer
#define LED0_set()      SetPin(2,0)

//! \brief LED reset
//! \hideinitializer
#define LED0_clear()    ClearPin(2,0)

#endif // LEDS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

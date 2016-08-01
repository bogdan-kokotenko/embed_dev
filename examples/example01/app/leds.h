//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup example01 Example01
//! \brief Blinking LED example
//! @{
//******************************************************************************
//   File description:
//! \file   example01/app/leds.h
//! \brief  Implement LED blinking using protothread and scheduling
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  13/06/2013 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef LEDS_H
#define LEDS_H

#ifdef __cplusplus
extern "C" {
#endif

// LEDs HAL configuration
#include "leds_config.h"

//! Initialize LEDs
void LED_init(void);

//! Thread which update device LEDs state
void LED_update(void);

#ifdef __cplusplus
}
#endif

#endif // LEDS_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

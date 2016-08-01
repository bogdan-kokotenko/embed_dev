//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_mingw_config
//! @{
//******************************************************************************
//  File description:
//! \file       example01/MinGW/config/leds_config.h
//! \brief      LEDs driver configuration
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H

#include "stdio.h"

//! \brief Simulate LED initialization
//! \hideinitializer
#define LED0_init()     printf("LED0: initialized. \n")

//! \brief Simulate LED set
//! \hideinitializer
#define LED0_set()      printf("LED0: * \n")

//! \brief Simulate LED reset
//! \hideinitializer
#define LED0_clear()    printf("LED0: o \n")

#endif // LEDS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

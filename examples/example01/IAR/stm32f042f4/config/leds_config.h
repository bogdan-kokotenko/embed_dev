//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_stm32f0x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/stm32f042f4/config/leds_config.h
//! \brief  LEDs configuration
//
//******************************************************************************
#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H

//! \brief LED initialization
//! \hideinitializer
#define LED0_init()     GPIO_setOut(GPIOB,1)

//! \brief LED set
//! \hideinitializer
#define LED0_set()      GPIO_setHigh(GPIOB,1)

//! \brief LED reset
//! \hideinitializer
#define LED0_clear()    GPIO_setLow(GPIOB,1)

#endif // LEDS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

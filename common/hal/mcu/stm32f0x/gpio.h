//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup stm32f0x_hal
//! @{
//! \defgroup stm32f0x_gpio  STM32F0x GPIO APIs
//! \brief STM32F0x GPIO Access APIs
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/stm32f0x/gpio.h
//! \brief  STM32F0x GPIO handling macro definitions
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "gpio_config.h"    
    
//! Set pin direction to input
void GPIO_setIn(GPIO_TypeDef* port, uint8_t pin);

//! Set pin direction to output
void GPIO_setOut(GPIO_TypeDef* port, uint8_t pin);

//! Set pin to '1'
void GPIO_setHigh(GPIO_TypeDef* port, uint8_t pin);

//! Set pin to '0'
void GPIO_setLow(GPIO_TypeDef* port, uint8_t pin);

//! Pull-up pin
void GPIO_pullUp(GPIO_TypeDef* port, uint8_t pin);

//! Pull-down pin
void GPIO_pullDown(GPIO_TypeDef* port, uint8_t pin);

//! Disable pull resistor
void GPIO_pullDisable(GPIO_TypeDef* port, uint8_t pin);

//! Enable pin interrupt
void GPIO_interruptEnable(GPIO_TypeDef* port, uint8_t pin);

//! Disable pin interrupt
void GPIO_interruptDisable(GPIO_TypeDef* port, uint8_t pin);

//! Get pin state
bool GPIO_test(GPIO_TypeDef* port, uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

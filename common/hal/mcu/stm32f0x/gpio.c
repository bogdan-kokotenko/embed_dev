//******************************************************************************
//   Copyright (C) 2015 Bogdan Kokotenko
//
// File description:
//! \file       hal/mcu/stm32f0x/gpio.c
//! \brief      STM32F0x GPIO handling functions
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_STM32F0X_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setOut()
// Description:
//! \brief      Set pin direction to input
//------------------------------------------------------------------------------
void GPIO_setIn(GPIO_TypeDef* port, uint8_t pin)
{

}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setOut()
// Description:
//! \brief      Set pin direction to output
//------------------------------------------------------------------------------
void GPIO_setOut(GPIO_TypeDef* port, uint8_t pin)
{
    // Enable clock for GPIO
    if(port == GPIOA)
        RCC->AHBENR |= RCC_AHBENR_GPIOBEN;      
    else
    if(port == GPIOB)
        RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    else
    if(port == GPIOC)
        RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    else
    if(port == GPIOD)
        RCC->AHBENR |= RCC_AHBENR_GPIODEN;

    port->MODER |= (GPIO_MODER_MODER0_0 << (pin<<1));       // output mode
    port->OTYPER &= ~(GPIO_OTYPER_OT_0 << pin);             // push-pull
    port->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR0 << (pin<<1));   // high speed
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setHigh()
// Description:
//! \brief      Set pin to '1'
//------------------------------------------------------------------------------
void GPIO_setHigh(GPIO_TypeDef* port, uint8_t pin)
{
    port->BSRR |= (GPIO_BSRR_BS_0 << pin);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setLow()
// Description:
//! \brief      Set pin to '0'
//------------------------------------------------------------------------------
void GPIO_setLow(GPIO_TypeDef* port, uint8_t pin)
{
    port->BSRR |= (GPIO_BSRR_BR_0 << pin);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullUp()
// Description:
//! \brief      Pull-up pin 
//------------------------------------------------------------------------------
void GPIO_pullUp(GPIO_TypeDef* port, uint8_t pin)
{
    port->PUPDR &= ~(GPIO_PUPDR_PUPDR0_0 << (pin<<1));
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullDown()
// Description:
//! \brief      Pull-down pin
//------------------------------------------------------------------------------
void GPIO_pullDown(GPIO_TypeDef* port, uint8_t pin)
{
    port->PUPDR &= ~(GPIO_PUPDR_PUPDR0_1 << (pin<<1));
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullDown()
// Description:
//! \brief      Disable pull resistor
//------------------------------------------------------------------------------
void GPIO_pullDisable(GPIO_TypeDef* port, uint8_t pin)
{
    port->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (pin<<1)); // no pull-up, pull-down
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func0()
// Description:
//! \brief      Select pin primary function (as GPIO)
//------------------------------------------------------------------------------
void GPIO_func0(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func1()
// Description:
//! \brief      Select pin secondary function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func1(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func2()
// Description:
//! \brief      Select pin third function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func2(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func3()
// Description:
//! \brief      Select pin fourth function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func3(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}
//------------------------------------------------------------------------------
// Function:	
//				GPIO_interruptEnable()
// Description:
//! \brief      Enable pin interrupt
//------------------------------------------------------------------------------
void GPIO_interruptEnable(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_interruptDisable()
// Description:
//! \brief      Disable pin interrupt
//------------------------------------------------------------------------------
void GPIO_interruptDisable(GPIO_TypeDef* port, uint8_t pin)
{
    // to do...
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_test()
// Description:
//! \brief      Get pin state
//------------------------------------------------------------------------------
bool GPIO_test(GPIO_TypeDef* port, uint8_t pin)
{
    return (port->IDR & (GPIO_IDR_0 << pin)) ? true : false;
}

#endif  // _STM32F0X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_gpio  MSP430F5x GPIO APIs
//! \brief MSP430F5x GPIO APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/gpio.h
//! \brief  MSP430F5x GPIO handling macro definitions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  28/06/2013 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning GPIO: Unknown MCU core, check HAL configuration!
#else

// Include configurations
#include "gpio_config.h"    

//! Default state initialization
void GPIO_init(void);

//------------------------------------------------------------------------------  
// GPIO access via macros (faster than functions)  
  
//! Set pin direction to output
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SetPinOut(x, y) P##x##DIR |= (1 << y)

//! Set pin direction to input
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SetPinIn(x, y)  P##x##DIR &= ~(1 << y)

//! Set pin state '1'
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SetPin(x, y)    P##x##OUT |= (1 << y)

//! Set pin state '0'
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define ClearPin(x, y)  P##x##OUT &= ~(1 << y)

//! Toggle pin state
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define TogglePin(x, y) P##x##OUT ^= (1 << y)

//! Enable port interrupt
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define EnablePinInterrupt(x, y)        P##x##IE |= (1 << y)  

//! Disable port interrupt
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define DisablePinInterrupt(x, y)       P##x##IE &= ~(1 << y)  

//! Clear interrupt flag
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define ClearPinInterruptFlag(x, y)     P##x##IFG &= ~(1 << y)  

//! Set interrupt High/Low
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SetPinInterruptHighLow(x, y)    P##x##IES |= (1 << y)  

//! Set interrupt Low/High
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SetPinInterruptLowHigh(x, y)    P##x##IES &= ~(1 << y)  
  
//! Toggle pin state
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define TestPin(x, y)  (P##x##IN & (1 << y))

//! Enable pin pull-up resistor
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define EnablePullUp(x, y)  (P##x##REN) |= (1 << y); (P##x##OUT) |= (1 << y)

//! Enable pin pull-down resistor
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define EnablePullDown(x, y)  (P##x##REN) |= (1 << y); (P##x##OUT) &= ~(1 << y)

//! Disable pin pull-up resistor
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define DisablePullUp(x, y)  (P##x##REN) &= ~(1 << y)

//! Disable pin pull-down resistor
//!
//! Same as DisablePullUp()
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define DisablePullDown(x, y)  (P##x##REN) &= ~(1 << y)    
    
//! Select pin function 1
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SelectPinFunc1(x, y)    P##x##SEL &= ~(1 << y)

//! Select pin function 2
//!
//! \param  x GPIO port
//! \param  y GPIO pin
//! \hideinitializer
#define SelectPinFunc2(x, y)    P##x##SEL |= (1 << y)		

//------------------------------------------------------------------------------  
// GPIO access via function (slower than macros)  

//! Set pin direction to input
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_setIn(uint8_t x, uint8_t y);

//! Set pin direction to output
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_setOut(uint8_t x, uint8_t y);

//! Set pin to '1'
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_setHigh(uint8_t x, uint8_t y);

//! Set pin to '0'
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_setLow(uint8_t x, uint8_t y);

//! Pull-up pin
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_pullUp(uint8_t x, uint8_t y);

//! Pull-down pin
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_pullDown(uint8_t x, uint8_t y);

//! Disable pull resistor
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_pullDisable(uint8_t x, uint8_t y);

//! Select pin primary function (as GPIO)
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_func1(uint8_t x, uint8_t y);

//! Select pin secondary function (depends on MCU peripherals)
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_func2(uint8_t x, uint8_t y);

//! Enable pin interrupt
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_interruptEnable(uint8_t x, uint8_t y);

//! Disable pin interrupt
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_interruptDisable(uint8_t x, uint8_t y);

//! Get pin state
//!
//! \param  x GPIO port
//! \param  y GPIO pin
bool GPIO_test(uint8_t x, uint8_t y);

#endif // _MSP430F5x_HAL_

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

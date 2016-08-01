//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_gpio  MSP430G2x GPIO APIs
//! \brief MSP430G2x GPIO APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/gpio.h
//! \brief      MSP430G2x GPIO handling macro definitions
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  28/06/2013 | Bogdan Kokotenko | Initial draft
//!  15/06/2015 | Bogdan Kokotenko | Comments improved
//!  13/02/2016 | Bogdan Kokotenko | Added new APIs (GPIO access via functions)
//
//******************************************************************************
#ifndef __GPIO_H_
#define __GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning GPIO: Unknown MCU core, check HAL configuration!
#else

// Include configurations
#include "gpio_config.h"    
    
//------------------------------------------------------------------------------  
// GPIO access via macros (faster than functions)  
  
//! Set pin direction to output
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SetPinOut(x, y) P##x##DIR |= (1 << y)

//! Set pin direction to input
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SetPinIn(x, y)  P##x##DIR &= ~(1 << y)

//! Set pin state '1'
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SetPin(x, y)    P##x##OUT |= (1 << y)

//! Set pin state '0'
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define ClearPin(x, y)  P##x##OUT &= ~(1 << y)

//! Toggle pin state
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define TogglePin(x, y) P##x##OUT ^= (1 << y)

//! Enable port interrupt
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define EnablePinInterrupt(x, y)        P##x##IE |= (1 << y)  

//! Disable port interrupt
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define DisablePinInterrupt(x, y)       P##x##IE &= ~(1 << y)  

//! Clear interrupt flag
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define ClearPinInterruptFlag(x, y)     P##x##IFG &= ~(1 << y)  

//! Set interrupt High/Low
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SetPinInterruptHighLow(x, y)    P##x##IES |= (1 << y)  

//! Set interrupt Low/High
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SetPinInterruptLowHigh(x, y)    P##x##IES &= ~(1 << y)  
  
//! Toggle pin state
//! \param x port name
//! \param y pin name
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

//! Select pin function 0
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc0(x, y)    \
    P##x##SEL  &= ~(1 << y);    \
    P##x##SEL2 &= ~(1 << y)

//! Select pin function 1
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc1(x, y)    \
    P##x##SEL |= (1 << y);      \
    P##x##SEL2 &= ~(1 << y)        

//! Select pin function 2
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc2(x, y)    \
    P##x##SEL &= ~(1 << y);     \
    P##x##SEL2 |= (1 << y)  

//! Select pin function 3
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc3(x, y)    \
    P##x##SEL |= (1 << y);      \
    P##x##SEL2 |= (1 << y)

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
void GPIO_func0(uint8_t x, uint8_t y);

//! Select pin secondary function (depends on MCU peripherals)
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_func1(uint8_t x, uint8_t y);

//! Select pin third function (as GPIO)
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_func2(uint8_t x, uint8_t y);

//! Select pin fourth function (depends on MCU peripherals)
//!
//! \param  x GPIO port
//! \param  y GPIO pin
void GPIO_func3(uint8_t x, uint8_t y);

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

#endif // _MSP430G2X_HAL_

#ifdef __cplusplus
}
#endif

#endif // __GPIO_H_
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

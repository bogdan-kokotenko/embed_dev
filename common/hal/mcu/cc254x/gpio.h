//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup cc254x_hal
//! @{
//! \defgroup cc254x_gpio  CC254x GPIO APIs
//! \brief CC254x GPIO APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/cc254x/gpio.h
//! \brief      CC254x GPIO handling macro definitions
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
#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Warn of inappropriate MCU core selection
#if ( !defined (_CC254x_HAL_) )
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
#define SetPin(x, y)    P##x##_##y = 1

//! Set pin state '0'
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define ClearPin(x, y)  P##x##_##y = 0

//! Toggle pin state
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define TogglePin(x, y) P##x##_##y ^= 1
/*
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
  */
//! Toggle pin state
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define TestPin(x, y)  (P##x##IN & (1 << y))

//! Select pin function 0
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc0(x, y)    \
    P##x##SEL  &= ~(1 << y)

//! Select pin function 1
//
//! \param x port name
//! \param y pin name
//! \hideinitializer
#define SelectPinFunc1(x, y)    \
    P##x##SEL |= (1 << y)        

#endif // _CC254x_HAL_

#ifdef __cplusplus
}
#endif

#endif // __GPIO_H_
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

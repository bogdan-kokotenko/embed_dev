//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup stm32f0x_hal
//! @{
//! \defgroup stm32_adc  STM32F0x ADC APIs
//! \brief STM32F0x ADC APIs
//! @{
//******************************************************************************
//	File description:
//! \file       hal/mcu/stm32f0x/adc.h
//! \brief     	STM32F0x ADC APIs			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  23/05/2016 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "adc_config.h"

//! ADC internal oscillator
#define ADC_HSI_FREQ    14000000L    // Hz
    
//! Enable internal Vref
#define VREF_INT            2500    // mV   
    
// ADC input selection options
#define ADC_IN0      1  // PA0
#define ADC_IN1      2  // PA1
#define ADC_TEMP    16  // Internal temp. sensor
#define ADC_VREFINT 17  // Internal reference

//! ADC initialization
void ADC_init(void);

#ifdef __cplusplus
}
#endif

#endif // ADC_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

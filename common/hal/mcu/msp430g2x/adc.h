//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_adc  MSP430G2x ADC APIs
//! \brief MSP430G2x ADC APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/adc.h
//! \brief      MSP430G2x ADC APIs			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  16/10/2015 | Bogdan Kokotenko | Initial draft
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
#define ADC10OSC_FREQ   5000000L    // Hz
    
// ADC input selection options
#define ADC_A0      1   // P1.1
#define ADC_A1      2   // P1.1
#define ADC_A2      3   // P1.2
#define ADC_A3      4   // P1.3    
#define ADC_A4      5   // P1.4
#define ADC_A5      6   // P1.5
#define ADC_A6      7   // P1.6    
#define ADC_A7      8   // P1.7  
#define ADC_TEMP    9   // Internal temp. sensor
#define ADC_VCC     10  // (AVcc-AVss)/2

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

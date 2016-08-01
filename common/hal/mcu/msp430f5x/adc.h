//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_adc  MSP430F5x ADC APIs
//! \brief MSP430F5x ADC APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/adc.h
//! \brief  MSP430F5x ADC APIs			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  02/02/2015 | Bogdan Kokotenko | Initial draft
//!  15/04/2015 | Bogdan Kokotenko | Improved clock selection
//!  03/05/2015 | Bogdan Kokotenko | Added Vref selection
//!
//******************************************************************************
#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

//! ADC internal oscillator
#define ADC12OSC    4800000L    // Hz
    
// Include configurations
#include "adc_config.h"      
    
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

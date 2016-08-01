//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup cc254x_hal
//! @{
//! \defgroup cc254x_clock  CC254x Clocks APIs
//! \brief CC254x clock sources APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/cc254x/clocks.h
//! \brief      CC254x clock sources API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_H
#define CLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "clocks_config.h"

//! Internal RCOSC oscillator frequency
#define RCOSC_FREQ          16000000L   // Hz

//! 32kHz oscillator frequency
#define OSC32K              32768       // Hz
    
//! Software delay (ms)
void __delay_ms(uint16_t msec);

//! Software delay (us)
void __delay_us(uint16_t sec);
  
//! Initialize system clocks
void CLK_init(void);

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_H_
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

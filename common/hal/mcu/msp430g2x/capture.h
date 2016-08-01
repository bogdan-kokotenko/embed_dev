//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_capture  MSP430G2x Capture Unit APIs
//! \brief MSP430G2x capture Unit APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/capture.h
//! \brief      MSP430G2x capture unit API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------
//!  26/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CAPTURE_H
#define CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "capture_config.h"

//! Get capture unit source clock frequency
#define CAPTURE_clk()   CAPTURE_FREQ 

// Capture mode options
#define CAPTURE_RISING          (CM0)
#define CAPTURE_FALING          (CM1)
#define CAPTURE_RISING_FALING   (CM0+CM1)
    
//! TIMER1 based capture unit clock initialization
void CAPTURE_init(void);    

//! Wait external trigger on CCI0A
void CAPTURE_waitCCI0A(void);
//! Wait external trigger on CCI0B
void CAPTURE_waitCCI0B(void);
//! Stop capturing external triggers on CCI0
void CAPTURE_stopCCI0(void);

//! Wait external trigger on CCI1A
void CAPTURE_waitCCI1A(void);
//! Wait external trigger on CCI1B
void CAPTURE_waitCCI1B(void);
//! Stop capturing external triggers on CCI1
void CAPTURE_stopCCI1(void);

//! Wait external trigger on CCI2A
void CAPTURE_waitCCI2A(void);
//! Wait external trigger on CCI2B
void CAPTURE_waitCCI2B(void);
//! Stop capturing external triggers on CCI2
void CAPTURE_stopCCI2(void);

//! Get period of PWM
uint32_t CAPTURE_interval0(void);
//! Get duty cycle of PWM ch1
uint32_t CAPTURE_interval1(void);
//! Get duty cycle of PWM ch2
uint32_t CAPTURE_interval2(void);

//! Returns the number of capture unit ticks since overflow
uint32_t CAPTURE_clock(void);

#ifdef __cplusplus
}
#endif

#endif	//CAPTURE_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

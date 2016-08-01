//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430g2x_config
//! @{
//******************************************************************************
//	File description:
//! \file       example01/IAR/msp430g2553/config/capture_config.h 
//! \brief      MSP430G2x Capture unit configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  26/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CAPTURE_CONFIG_H
#define CAPTURE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Enable capture unit 
//#define CAPTURE_ENABLED
    
//! Capture unit clock frequency
//! Supported options: SMCLK_FREQ, ACLK_FREQ  with dividers 2,4,8
#define CAPTURE_FREQ    SMCLK_FREQ    

//------------------------------------------------------------------------------
//! CCI0 capture mode selection
//! Supported options: CAPTURE_RISING, CAPTURE_FALING, CAPTURE_RISING_FALING
//#define CAPTURE_MODE_CCI0       CAPTURE_FALING
    
// CCI0 capture inputs selection (CCI0A or CCI0B)
//#define CAPTURE_CCI0A_P2_0        //!< P2.0 -> CCI0A
//#define CAPTURE_CCI0B_P2_3        //!< P2.3 -> CCI0B

// Handler for CCI0 interrupt
//#define CAPTURE_HandlerCCI0(Value)    
    
//------------------------------------------------------------------------------
//! CCI1 capture mode selection
//! Supported options: CAPTURE_RISING, CAPTURE_FALING, CAPTURE_RISING_FALING
//#define CAPTURE_MODE_CCI1       CAPTURE_FALING

// CCI1 capture inputs selection (CCI1A or CCI1B)
//#define CAPTURE_CCI1A_P2_1        //!< P2.1 <- CCI1A
//#define CAPTURE_CCI1B_P2_2        //!< P2.2 <- CCI1B

// Handler for CCI1 interrupt
//#define CAPTURE_HandlerCCI1(Value)
    
//------------------------------------------------------------------------------
//! CCI2 capture mode selection
//! Supported options: CAPTURE_RISING, CAPTURE_FALING, CAPTURE_RISING_FALING
//#define CAPTURE_MODE_CCI2       CAPTURE_FALING

// CCI2 capture inputs selection (CCI2A or CCI2B)
//#define CAPTURE_CCI2A_P2_4        //!< P2.4 <- CCI2A
//#define CAPTURE_CCI2B_P2_5        //!< P2.5 <- CCI2B

// Handler for CCI2 interrupt
//#define CAPTURE_HandlerCCI2(Value)

#ifdef __cplusplus
}
#endif

#endif	//CAPTURE_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

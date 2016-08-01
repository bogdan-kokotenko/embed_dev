//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430g2x_config
//! @{
//******************************************************************************
//	File description:
//! \file       example01/IAR/msp430g2553/config/pwm_config.h
//! \brief      MSP430G2x PWM configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  11/04/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Enable PWM 
//#define PWM_ENABLED
    
//! PMW clock frequency
//! Supported options: SMCLK_FREQ, ACLK_FREQ  with dividers 2,4,8
#define PWM_FREQ    SMCLK_FREQ    
    
// Timer1 output initializations
//#define TA1_CCR0_P2_0_OUT     //!< P2.0 <- CCR0
//#define TA1_CCR0_P2_3_OUT     //!< P2.3 <- CCR0
//#define TA1_CCR0_P3_1_OUT     //!< P3.1 <- CCR0

//#define TA1_CCR1_P2_1_OUT       //!< P2.1 <- CCR1
//#define TA1_CCR1_P2_2_OUT     //!< P2.2 <- CCR1
//#define TA1_CCR1_P3_2_OUT     //!< P3.2 <- CCR1

//#define TA1_CCR2_P2_4_OUT     //!< P2.4 <- CCR2
//#define TA1_CCR2_P2_5_OUT     //!< P2.5 <- CCR2
//#define TA1_CCR2_P3_3_OUT     //!< P3.3 <- CCR2
   
#ifdef __cplusplus
}
#endif

#endif	//PWM_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

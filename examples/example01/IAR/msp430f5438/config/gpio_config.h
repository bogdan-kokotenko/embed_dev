//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430f5x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/msp430f5438/config/gpio_config.h
//! \brief  MSP430F5438A GPIO configuration		
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------------
//!  30/11/2015 | Bogdan Kokotenko | Initial draft
//!  16/01/2016 | Bogdan Kokotenko | Added decoupler handler. Fixed typo
//
//******************************************************************************
#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// P1 default state
//#define P1DIR_DEFAULT      
//#define P1OUT_DEFAULT      

// P2 default state
//#define P2DIR_DEFAULT        
//#define P2OUT_DEFAULT        
    
// P3 default state
//#define P3DIR_DEFAULT        (BIT0+BIT1+BIT2+BIT3+BIT5+BIT6+BIT7)    
//#define P3OUT_DEFAULT        ~(BIT0+BIT1+BIT2+BIT3+BIT5+BIT6+BIT7)  

// P4 default state
//#define P4DIR_DEFAULT        (BIT0+BIT1+BIT2+BIT3+BIT7)    
//#define P4OUT_DEFAULT        ~(BIT0+BIT1+BIT2+BIT3+BIT7)  

// P5 default state
//#define P5DIR_DEFAULT        (BIT0+BIT1+BIT2+BIT3+BIT5)    
//#define P5OUT_DEFAULT        ~(BIT0+BIT1+BIT2+BIT3+BIT5)  

// P7 default state
//#define P7DIR_DEFAULT        (BIT2+BIT3+BIT7)    
//#define P7OUT_DEFAULT        ~(BIT2+BIT3+BIT7)      
    
// P1.0 interrupt handler
//#define P1_0_Handler()

// P1.1 interrupt handler
//#define P1_1_Handler()

//! P1.1 interrupt handler
//#define P1_2_Handler()

//! P1.2 interrupt handler
//#define P1_2_Handler()
  
//! P2.0 interrupt handler
//#define P2_0_Handler()

//! P2.1 interrupt handler
//#define P2_1_Handler()

//! P2.1 interrupt handler
//#define P2_2_Handler()

#ifdef __cplusplus
}
#endif

#endif // GPIO_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

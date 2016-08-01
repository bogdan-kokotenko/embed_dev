//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430g2x/adc.c
//! \brief  MSP430G2x ADC functions implementation			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  16/10/2015 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "analog.h"
#include "timers.h"
#include "adc.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning ADC: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              ADC_init()
// Description:
//! \brief      ADC initialization
//------------------------------------------------------------------------------
void ADC_init(void)
{
#if ( defined(VREF_INT)  &&  defined(VREF_EXT) ) || \
    ( !defined(VREF_INT)  &&  !defined(VREF_EXT) )
    #error ADC: Wrong Vref selection
#endif    

#if ( defined(VREF_EXT) )
    ADC10CTL0 |= SREF_2;                        // Vref <- ext 
#endif // VREF_EXT
    
#if ( defined(VREF_INT) )
    ADC10CTL0 |= REFON+SREF_1;                  // Vref <- int
    #if (VREF_INT == 2500)
        ADC10CTL0 |= REF2_5V;                   // Vref = 2500 mV 
    #elif (VREF_INT != 1500)                    // Vref = 1500 mV
        #warning ADC: Wrong internal Vref level selection
    #endif
#endif // VREF_INT

#if ( defined(VREF_PIN_ENABLED) )
    ADC10CTL0 |= REFOUT;                        // Vref -> output (P1.4)
#endif

#if (ADC_CLK_FREQ == ADC10OSC_FREQ)
    ADC10CTL1   |= ADC10SSEL_0;                 // ADC_CLK <- ADC10OSC
#elif (ADC_CLK_FREQ == ADC10OSC_FREQ/2)
    ADC10CTL1   |= ADC10SSEL_0;                 // ADC_CLK <- ADC10OSC/2
    ADC10CTL1   |= ADC10DIV_1;
#elif (ADC_CLK_FREQ == ADC10OSC_FREQ/4)
    ADC10CTL1   |= ADC10SSEL_0;                 // ADC_CLK <- ADC10OSC/4
    ADC10CTL1   |= ADC10DIV_3;
#elif (ADC_CLK_FREQ == ADC10OSC_FREQ/8)
    ADC10CTL1   |= ADC10SSEL_0;                 // ADC_CLK <- ADC10OSC/8
    ADC10CTL1   |= ADC10DIV_7;
#elif (ADC_CLK_FREQ == SMCLK_FREQ)
    ADC10CTL1   |= ADC10SSEL_3;                 // ADC_CLK <- SMCLK
#elif (ADC_CLK_FREQ == SMCLK_FREQ/2)
    ADC10CTL1   |= ADC10SSEL_3;                 // ADC_CLK <- SMCLK/2
    ADC10CTL1   |= ADC10DIV_1;
#elif (ADC_CLK_FREQ == SMCLK_FREQ/4)
    ADC10CTL1   |= ADC10SSEL_3;                 // ADC_CLK <- SMCLK/4
    ADC10CTL1   |= ADC10DIV_3;
#elif (ADC_CLK_FREQ == SMCLK_FREQ/8)
    ADC10CTL1   |= ADC10SSEL_3;                 // ADC_CLK <- SMCLK/8
    ADC10CTL1   |= ADC10DIV_7;
#elif (ADC_CLK_FREQ == ACLK_FREQ)
    SD24CTL     |= ADC10SSEL_1;                 // ADC_CLK <- ACLK
#elif (ADC_CLK_FREQ == ACLK_FREQ/2)
    ADC10CTL1   |= ADC10SSEL_1;                 // ADC_CLK <- ACLK/2
    ADC10CTL1   |= ADC10DIV_1;
#elif (ADC_CLK_FREQ == ACLK_FREQ/4)
    ADC10CTL1   |= ADC10SSEL_1;                 // ADC_CLK <- ACLK/4
    ADC10CTL1   |= ADC10DIV_3;
#elif (ADC_CLK_FREQ == ACLK_FREQ/8)
    ADC10CTL1   |= ADC10SSEL_1;                 // ADC_CLK <- ACLK/8
    ADC10CTL1   |= ADC10DIV_7;
#else
    #error ADC: Unsupported ADC clock frequency
#endif

    ADC10CTL0 |= ADC10ON;                       // enable ADC

#if (defined(ADC_BIPOLAR))    
    ADC10CTL1   |= ADC10DF;                     // 2s complement
#endif

#if (ADC_CHANNEL == ADC_A0)
    ADC10CTL1 |= INCH_0;                        // ADC <- A0
    ADC10AE0  |= BIT0;                          // A0  <- P1.0
#elif (ADC_CHANNEL == ADC_A1)
    ADC10CTL1 |= INCH_1;                        // ADC <- A1
    ADC10AE0  |= BIT1;                          // A1  <- P1.1
#elif (ADC_CHANNEL == ADC_A2)
    ADC10CTL1 |= INCH_2;                        // ADC <- A2
    ADC10AE0  |= BIT2;                          // A2  <- P1.2
#elif (ADC_CHANNEL == ADC_A3)
    ADC10CTL1 |= INCH_3;                        // ADC <- A3
    ADC10AE0  |= BIT3;                          // A3  <- P1.3
#elif (ADC_CHANNEL == ADC_A4)
    ADC10CTL1 |= INCH_4;                        // ADC <- A4
    ADC10AE0  |= BIT4;                          // A4  <- P1.4
#elif (ADC_CHANNEL == ADC_A5)
    ADC10CTL1 |= INCH_5;                        // ADC <- A5
    ADC10AE0  |= BIT5;                          // A5  <- P1.5
#elif (ADC_CHANNEL == ADC_A6)
    ADC10CTL1 |= INCH_6;                        // ADC <- A6
    ADC10AE0  |= BIT6;                          // A6  <- P1.6
#elif (ADC_CHANNEL == ADC_A7)
    ADC10CTL1 |= INCH_7;                        // ADC <- A7
    ADC10AE0  |= BIT7;                          // A7  <- P1.7
#elif (ADC_CHANNEL == ADC_TEMP)
    ADC10CTL1 |= INCH_10;                       // ADC <- Temp
#elif (ADC_CHANNEL == ADC_VCC)
    ADC10CTL1 |= INCH_11;                       // ADC <- VCC/2
#else
    #error ADC: Wrong ADC channel selection
#endif
    
    //ADC10CTL0   |= MSC;                       // sequence sampling
    //ADC10CTL1   |= CONSEQ_2;                  // repeat-single channel
    ADC10CTL1   |= SHS_3;                       // ADC_SOC <- TA_CCR2
    
    ADC10CTL0 &= ~ADC10IFG;                     // clear ADC IFG
    ADC10CTL0 |= ADC10IE;                       // enable ADC interrupt

    ADC10CTL0 |= ENC;                           // triger conversion
    
    #ifdef ADC_SOC_init
        ADC_SOC_init();                         // set ADC SOC
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//              ADC_isr()
// Description:
//!             ADC interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=ADC10_VECTOR
__interrupt void ADC_isr(void)
{
    #ifdef ADC_Handler
        ADC_Handler(ADC10MEM);
    #else
        ADC10CTL0 &= ~ADC10IFG;                 // clear ADC IFG
    #endif //ADC_Handler

    ADC10CTL0 |= ENC;                           // enable conversion

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // wake-up MCU
    #endif 
}

#endif // _MSP430_HAL_

//******************************************************************************
// End of file
//******************************************************************************

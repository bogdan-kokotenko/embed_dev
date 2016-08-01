//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430g2x/adc.c
//! \brief  MSP430G2x ADC functions implementation			
//!
//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/adc.c
//! \brief  MSP430F5x ADC functions implementation			
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
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "time.h"
#include "clocks.h"
#include "timers.h"
#include "adc.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning ADC: Unknown MCU core, check HAL configuration!
#else

// Analog input pins
#define A0_PIN          6,0
#define A1_PIN          6,1
#define A2_PIN          6,2
#define A3_PIN          6,3
#define A4_PIN          6,4
#define A5_PIN          6,5
#define A6_PIN          6,6
#define A7_PIN          6,7
#if ( defined (__MSP430F5438A__) )
#define A12_PIN         7,4
#define A13_PIN         7,5
#define A14_PIN         7,6
#define A15_PIN         7,7
#elif ( defined (__MSP430F5329__))
#define A12_PIN         7,0
#define A13_PIN         7,1
#define A14_PIN         7,2
#define A15_PIN         7,3
#endif

//------------------------------------------------------------------------------
// Function:	
//              ADC_init()
// Description:
//! \brief      ADC initialization
//------------------------------------------------------------------------------
void ADC_init(void)
{
#ifdef ADC_A0
    GPIO_func2(A0_PIN);                         // A0_PIN -> A0
#endif // ADC_A0
#ifdef ADC_A1
    GPIO_func2(A1_PIN);                         // A1_PIN -> A1
#endif // ADC_A1
#ifdef ADC_A2
    GPIO_func2(A2_PIN);                         // A2_PIN -> A2
#endif // ADC_A2
#ifdef ADC_A3
    GPIO_func2(A3_PIN);                         // A3_PIN -> A3
#endif // ADC_A3
#ifdef ADC_A4
    GPIO_func2(A4_PIN);                         // A4_PIN -> A4
#endif // ADC_A4
#ifdef ADC_A5
    GPIO_func2(A5_PIN);                         // A5_PIN -> A5
#endif // ADC_A5
#ifdef ADC_A6
    GPIO_func2(A6_PIN);                         // A6_PIN -> A6
#endif // ADC_A6
#ifdef ADC_A7
    GPIO_func2(A7_PIN);                         // A7_PIN -> A7
#endif // ADC_A7
#ifdef ADC_A12
    GPIO_func2(A12_PIN);                        // A12_PIN -> A12
#endif // ADC_A12
#ifdef ADC_A13
    GPIO_func2(A13_PIN);                        // A13_PIN -> A13
#endif // ADC_A13
#ifdef ADC_A14
    GPIO_func2(A14_PIN);                        // A14_PIN -> A14
#endif // ADC_A14
#ifdef ADC_A15
    GPIO_func2(A15_PIN);                        // A15_PIN -> A15
#endif // ADC_A15
    
#if ( defined(VREF_INT)  &&  defined(VREF_EXT) ) || \
    ( !defined(VREF_INT)  &&  !defined(VREF_EXT) )
    #error Wrong Vref selection
#endif    
    
#if ( defined(VREF_INT) )
    #if ( defined (__MSP430F6638__)  \
       || defined (__MSP430F5438A__) \
       || defined (__MSP430F5329__)  )          // Reset REFMSTR to hand over
        REFCTL0 &= ~REFMSTR;                    // control to ADC12_A ref
    #endif
    ADC12CTL0   |= ADC12REFON;                  // enable int Vref
    #if (VREF_INT == 2500)
        ADC12CTL0   |= ADC12REF2_5V;            // Vref = 2.5V
    #elif (VREF_INT != 1500)                    // Vref = 1.5V
        #error ADC: Unsupported internal Vref level
    #endif
#endif // VREF_INT

#if ( defined(VREF_PIN_ENABLED) )
    SelectPinFunc2(5,0);                        // P5.0 <- VREF+
    ADC12CTL2   |= ADC12REFOUT;                 // VREF+ <- Vref
#endif
    
    ADC12CTL0   |= ADC12ON ;                    // ADC on
    ADC12CTL2   |= ADC12RES_2;                  // 12 bit
    ADC12CTL2   |= ADC12DF;                     // signed result

#if (ADC_CLK_FREQ == SMCLK_FREQ)
    ADC12CTL1   |= ADC12SSEL_3;                 // ADC_CLK <- SMCLK
#elif (ADC_CLK_FREQ == ACLK_FREQ)
    ADC12CTL1   |= ADC12SSEL_1;                 // ADC_CLK <- ACLK
#elif (ADC_CLK_FREQ == ADC12OSC)
    //ADC12CTL1   |= ADC12SSEL_0;               // ADC_CLK <- ADC12OSC
#else                                           // silicon bugfix ?!
#error ADC: Unsupported clock frequency
#endif

#if (ADC_CLK_DIV == 1)
    ADC12CTL1   |= ADC12DIV_0;                  // ADC_CLK/1 
#elif (ADC_CLK_DIV == 2)
    ADC12CTL1   |= ADC12DIV_1;                  // ADC_CLK/2 
#elif (ADC_CLK_DIV == 3)
    ADC12CTL1   |= ADC12DIV_2;                  // ADC_CLK/3 
#elif (ADC_CLK_DIV == 4)
    ADC12CTL1   |= ADC12DIV_3;                  // ADC_CLK/4 
#elif (ADC_CLK_DIV == 5)
    ADC12CTL1   |= ADC12DIV_4;                  // ADC_CLK/5 
#elif (ADC_CLK_DIV == 6)
    ADC12CTL1   |= ADC12DIV_5;                  // ADC_CLK/6 
#elif (ADC_CLK_DIV == 7)
    ADC12CTL1   |= ADC12DIV_6;                  // ADC_CLK/7 
#elif (ADC_CLK_DIV == 8)
    ADC12CTL1   |= ADC12DIV_7;                  // ADC_CLK/8 
#else
#error ADC: Unsupported clock divider
#endif
    
    ADC12CTL0   |= ADC12SHT0_2+ADC12SHT1_2;     // S/H = 16*ADCCLK 
    ADC12CTL1   |= ADC12SHP;                    // Use sampling timer

    ADC12CTL0   |= ADC12MSC;                    // sequence sampling
    ADC12CTL1   |= ADC12CONSEQ_1;               // sequence of channels
    ADC12CTL1   |= ADC12SHS_3;                  // ADC_SOC <- TB0_CCR1

#ifdef ADC_CH0
    #if defined(VREF_INT)
        ADC12MCTL0  |= ADC12SREF_1;             // ADC_ch0_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL0  |= ADC12SREF_2;             // ADC_ch0_ref = Vref ext
    #endif    
    ADC12MCTL0  |= ADC_CH0;                     // ADC_ch0
#endif
#if ( defined(ADC_CH0) && !defined(ADC_CH1) )
    ADC12MCTL0  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE0;                    // ADC_ch0 interrupt enabled
    ADC12IFG    &= ~ADC12IFG0;                  // clear ADC_ch0 IFG
#endif

#ifdef ADC_CH1
    #if defined(VREF_INT)
        ADC12MCTL1  |= ADC12SREF_1;             // ADC_ch1_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL1  |= ADC12SREF_2;             // ADC_ch1_ref = Vref ext
    #endif  
    ADC12MCTL1  |= ADC_CH1;                     // ADC_ch1
#endif
#if ( defined(ADC_CH1) && !defined(ADC_CH2) )
    ADC12MCTL1  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE1;                    // ADC_ch1 interrupt enabled
    ADC12IFG    &= ~ADC12IFG1;                  // clear ADC_ch1 IFG
#endif

#ifdef ADC_CH2
    #if defined(VREF_INT)
        ADC12MCTL2  |= ADC12SREF_1;             // ADC_ch2_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL2  |= ADC12SREF_2;             // ADC_ch2_ref = Vref ext
    #endif  
    ADC12MCTL2  |= ADC_CH2;                     // ADC_ch2
#endif
#if ( defined(ADC_CH2) && !defined(ADC_CH3) )
    ADC12MCTL2  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE2;                    // ADC_ch2 interrupt enabled
    ADC12IFG    &= ~ADC12IFG2;                  // clear ADC_ch2 IFG
#endif

#ifdef ADC_CH3
    #if defined(VREF_INT)
        ADC12MCTL3  |= ADC12SREF_1;             // ADC_ch3_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL3  |= ADC12SREF_2;             // ADC_ch3_ref = Vref ext
    #endif
    ADC12MCTL3  |= ADC_CH3;                     // ADC_ch4
#endif
#if ( defined(ADC_CH3) && !defined(ADC_CH4) )
    ADC12MCTL3  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE3;                    // ADC_ch3 interrupt enabled
    ADC12IFG    &= ~ADC12IFG3;                  // clear ADC_ch3 IFG
#endif

#ifdef ADC_CH4
    #if defined(VREF_INT)
        ADC12MCTL4  |= ADC12SREF_1;             // ADC_ch4_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL4  |= ADC12SREF_2;             // ADC_ch4_ref = Vref ext
    #endif
    ADC12MCTL4  |= ADC_CH4;                     // ADC_ch4
#endif
#if ( defined(ADC_CH4) && !defined(ADC_CH5) )
    ADC12MCTL4  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE4;                    // ADC_ch4 interrupt enabled
    ADC12IFG    &= ~ADC12IFG4;                  // clear ADC_ch4 IFG
#endif

#ifdef ADC_CH5
    #if defined(VREF_INT)
        ADC12MCTL5  |= ADC12SREF_1;             // ADC_ch5_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL5  |= ADC12SREF_2;             // ADC_ch5_ref = Vref ext
    #endif
    ADC12MCTL5  |= ADC_CH5;                     // ADC_ch5
#endif
#if ( defined(ADC_CH5) && !defined(ADC_CH6) )
    ADC12MCTL5  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE5;                    // ADC_ch5 interrupt enabled
    ADC12IFG    &= ~ADC12IFG5;                  // clear ADC_ch5 IFG
#endif

#ifdef ADC_CH6
    #if defined(VREF_INT)
        ADC12MCTL6  |= ADC12SREF_1;             // ADC_ch6_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL6  |= ADC12SREF_2;             // ADC_ch6_ref = Vref ext
    #endif
    ADC12MCTL6  |= ADC_CH6;                     // ADC_ch6
#endif
#if ( defined(ADC_CH6) && !defined(ADC_CH7) )
    ADC12MCTL6  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE6;                    // ADC_ch6 interrupt enabled
    ADC12IFG    &= ~ADC12IFG6;                  // clear ADC_ch6 IFG
#endif

#ifdef ADC_CH7
    #if defined(VREF_INT)
        ADC12MCTL7  |= ADC12SREF_1;             // ADC_ch7_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL7  |= ADC12SREF_2;             // ADC_ch7_ref = Vref ext
    #endif
    ADC12MCTL7  |= ADC_CH7;                     // ADC_ch7
#endif
#if ( defined(ADC_CH7) && !defined(ADC_CH8) ) 
    ADC12MCTL7  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE7;                    // ADC_ch7 interrupt enabled
    ADC12IFG    &= ~ADC12IFG7;                  // clear ADC_ch7 IFG
#endif

#ifdef ADC_CH8
    #if defined(VREF_INT)
        ADC12MCTL8  |= ADC12SREF_1;             // ADC_ch8_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL8  |= ADC12SREF_2;             // ADC_ch8_ref = Vref ext
    #endif
    ADC12MCTL8  |= ADC_CH8;                     // ADC_ch8
#endif
#if ( defined(ADC_CH8) && !defined(ADC_CH9) )  
    ADC12MCTL8  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE8;                    // ADC_ch8 interrupt enabled
    ADC12IFG    &= ~ADC12IFG8;                  // clear ADC_ch8 IFG
#endif

#ifdef ADC_CH9
    #if defined(VREF_INT)
        ADC12MCTL9  |= ADC12SREF_1;             // ADC_ch9_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL9  |= ADC12SREF_2;             // ADC_ch9_ref = Vref ext
    #endif
    ADC12MCTL9  |= ADC_CH9;                     // ADC_ch9
#endif
#if ( defined(ADC_CH9) && !defined(ADC_CH10) )  
    ADC12MCTL9  |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE9;                    // ADC_ch9 interrupt enabled
    ADC12IFG    &= ~ADC12IFG9;                  // clear ADC_ch9 IFG
#endif

#ifdef ADC_CH10
    #if defined(VREF_INT)
        ADC12MCTL10  |= ADC12SREF_1;            // ADC_ch10_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL10  |= ADC12SREF_2;            // ADC_ch10_ref = Vref ext
    #endif
    ADC12MCTL10  |= ADC_CH10;                   // ADC_ch10
#endif
#if ( defined(ADC_CH10) && !defined(ADC_CH11) ) 
    ADC12MCTL10 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE10;                   // ADC_ch10 interrupt enabled
    ADC12IFG    &= ~ADC12IFG10;                 // clear ADC_ch10 IFG
#endif    
    
#ifdef ADC_CH11
    #if defined(VREF_INT)
        ADC12MCTL11  |= ADC12SREF_1;            // ADC_ch11_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL11  |= ADC12SREF_2;            // ADC_ch11_ref = Vref ext
    #endif
    ADC12MCTL11  |= ADC_CH11;                   // ADC_ch111
#endif
#if ( defined(ADC_CH11) && !defined(ADC_CH12) )
    ADC12MCTL11 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE11;                   // ADC_ch11 interrupt enabled
    ADC12IFG    &= ~ADC12IFG11;                 // clear ADC_ch11 IFG
#endif    

#ifdef ADC_CH12
    #if defined(VREF_INT)
        ADC12MCTL12  |= ADC12SREF_1;            // ADC_ch12_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL12  |= ADC12SREF_2;            // ADC_ch12_ref = Vref ext
    #endif
    ADC12MCTL12  |= ADC_CH12;                   // ADC_ch12
#endif
#if ( defined(ADC_CH12) && !defined(ADC_CH13) ) 
    ADC12MCTL12 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE12;                   // ADC_ch12 interrupt enabled
    ADC12IFG    &= ~ADC12IFG12;                 // clear ADC_ch12 IFG
#endif

#ifdef ADC_CH13
    #if defined(VREF_INT)
        ADC12MCTL13  |= ADC12SREF_1;            // ADC_ch13_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL13  |= ADC12SREF_2;            // ADC_ch13_ref = Vref ext
    #endif
    ADC12MCTL13  |= ADC_CH13;                   // ADC_ch13
#endif
#if ( defined(ADC_CH13) && !defined(ADC_CH14) )  
    ADC12MCTL13 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE13;                   // ADC_ch13 interrupt enabled
    ADC12IFG    &= ~ADC12IFG13;                 // clear ADC_ch13 IFG
#endif    
    
#ifdef ADC_CH14
    #if defined(VREF_INT)
        ADC12MCTL14  |= ADC12SREF_1;            // ADC_ch14_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL14  |= ADC12SREF_2;            // ADC_ch14_ref = Vref ext
    #endif
    ADC12MCTL14  |= ADC_CH14;                   // ADC_ch14
#endif
#if ( defined(ADC_CH14) && !defined(ADC_CH15) )
    ADC12MCTL14 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE14;                   // ADC_ch14 interrupt enabled
    ADC12IFG    &= ~ADC12IFG14;                 // clear ADC_ch14 IFG
#endif

#ifdef ADC_CH15
    #if defined(VREF_INT)
        ADC12MCTL15  |= ADC12SREF_1;            // ADC_ch15_ref = Vref int
    #elif defined(VREF_EXT)
        ADC12MCTL15  |= ADC12SREF_2;            // ADC_ch15_ref = Vref ext
    #endif
    ADC12MCTL15  |= ADC_CH15;                   // ADC_ch15
#endif
#if ( defined(ADC_CH15) )
    ADC12MCTL15 |= ADC12EOS;                    // end of sequence
    ADC12IE     |= ADC12IE15;                   // ADC_ch15 interrupt enabled
    ADC12IFG    &= ~ADC12IFG15;                 // clear ADC_ch15 IFG
#endif
    
    ADC12CTL0 |= ADC12OVIE + ADC12TOVIE;      //ADC overflow interrupts
    
    ADC12CTL0   |= ADC12ENC;                    // Enable conversions
    
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
#pragma vector=ADC12_VECTOR
__interrupt void ADC_isr(void)
{
    static bool ovf = false;                // Detect ADC overflow

    switch(__even_in_range(ADC12IV,34))
    {
        case  0: break;                     // Vector  0:  No interrupt
        case  2:                            // Vector  2:  ADC overflow
            ovf = true;                     // set overflow flag
            break;                         
        case  4:                            // Vector  4:  ADC timing overflow
            ovf = true;                     // set overflow flag
            break;                         
        case  6:                            // Vector  6:  ADC12IFG0
        case  8:                            // Vector  8:  ADC12IFG1
        case 10:                            // Vector 10:  ADC12IFG2
        case 12:                            // Vector 12:  ADC12IFG3
        case 14:                            // Vector 14:  ADC12IFG4
        case 16:                            // Vector 16:  ADC12IFG5
        case 18:                            // Vector 18:  ADC12IFG6
        case 20:                            // Vector 20:  ADC12IFG7
        case 22:                            // Vector 22:  ADC12IFG8
        case 24:                            // Vector 24:  ADC12IFG9
        case 26:                            // Vector 26:  ADC12IFG10
        case 28:                            // Vector 28:  ADC12IFG11
        case 30:                            // Vector 30:  ADC12IFG12
        case 32:                            // Vector 32:  ADC12IFG13
        case 34:                            // Vector 34:  ADC12IFG14
        default:
        #ifdef ADC_Handler
            ADC_Handler((int16_t *)&ADC12MEM0, ovf);
        #endif //ADC_Handler
            ovf = false;
            break; 
    }

    ADC12IFG    = 0x00;                         // Clear all ADC_IFG
    ADC12CTL0   |= ADC12ENC;                    // Enable conversions

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

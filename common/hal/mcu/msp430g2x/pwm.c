//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/pwm.c
//! \brief  MSP430G2x PWM functions implementation			
//! 
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  11/04/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"
#include "devtime.h"
#include "timers.h"
#include "capture.h"
#include "pwm.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning PWM: Unknown MCU core, check HAL configuration!
#else

// Check if PWM is enabled and available
#if (defined(PWM_ENABLED) && defined(__MSP430G2553))

// Check if TIMER or CAPTURE mode selected
#if ( defined(TIMER1_EANBLED) || defined(CAPTURE_ENABLED) )
    #error PWM: Unit is not avaliable if TIMER or CAPTURE mode selected!
#endif

//------------------------------------------------------------------------------
// Function:	
//              PWM_init()
// Description:
//! \brief      TIMER1 based PWM initialization
//------------------------------------------------------------------------------
void PWM_init(void)
{
    // Initialize timer
    TA1CTL  |= TACLR;                           // clear

#if (PWM_FREQ == SMCLK_FREQ)
    TA1CTL  |= TASSEL_2;                        // SMCLK
    TA1CTL  |= ID_0;
#elif (PWM_FREQ == SMCLK_FREQ/2)
    TA1CTL  |= TASSEL_2;                        // SMCLK/2
    TA1CTL  |= ID_1;
#elif (PWM_FREQ == SMCLK_FREQ/4)
    TA1CTL  |= TASSEL_2;                        // SMCLK/4
    TA1CTL  |= ID_2;
#elif (PWM_FREQ == SMCLK_FREQ/8)
    TA1CTL  |= TASSEL_2;                        // SMCLK/8
    TA1CTL  |= ID_3;
#elif (PWM_FREQ == ACLK_FREQ)
    TA1CTL  |= TASSEL_1;                        // ACLK
    TA1CTL  |= ID_0;
#elif (PWM_FREQ == ACLK_FREQ/2)
    TA1CTL  |= TASSEL_1;                        // ACLK/2
    TA1CTL  |= ID_1;
#elif (PWM_FREQ == ACLK_FREQ/4)
    TA1CTL  |= TASSEL_1;                        // ACLK/4
    TA1CTL  |= ID_2;
#elif (PWM_FREQ == ACLK_FREQ/8)
    TA1CTL  |= TASSEL_1;                        // ACLK/8
    TA1CTL  |= ID_3;
#else
    #error PWM: Unsupported TIMER1 clock frequency
#endif
    
#ifdef TA1_CCR0_P2_0_OUT
    SelectPinFunc1(2,0);                        // TA1.0 -> P2.0
#endif
#ifdef TA1_CCR0_P2_3_OUT
    SelectPinFunc1(2,3);                        // TA1.0 -> P2.3
#endif
#ifdef TA1_CCR0_P3_1_OUT
    SelectPinFunc1(3,1);                        // TA1.0 -> P3.1
#endif
#if (defined(TA1_CCR0_P2_0_OUT) ||  \
    defined(TA1_CCR0_P2_3_OUT) ||   \
    defined(TA1_CCR0_P3_1_OUT))
    TA1CCTL0 |= OUTMOD_4;                       // toggle Timer1 out
#endif

#ifdef TA1_CCR1_P2_1_OUT
    SelectPinFunc1(2,1);                        // TA1.1 -> P2.1
#endif
#ifdef TA1_CCR1_P2_2_OUT
    SelectPinFunc1(2,2);                        // TA1.1 -> P2.2
#endif
#ifdef TA1_CCR1_P3_2_OUT
    SelectPinFunc1(3,2);                        // TA1.1 -> P3.2
#endif
#if (defined(TA1_CCR1_P2_1_OUT) ||  \
    defined(TA1_CCR1_P2_3_OUT) ||   \
    defined(TA1_CCR1_P3_2_OUT))
    TA1CCTL1 |= OUTMOD_7;						// PWM reset/set
#endif
   
#ifdef TA1_CCR2_P2_4_OUT
    SelectPinFunc1(2,4);                        // TA1.2 -> P2.4
#endif
#ifdef TA1_CCR2_P2_5_OUT
    SelectPinFunc1(2,5);                        // TA1.2 -> P2.5
#endif
#ifdef TA1_CCR2_P3_3_OUT
    SelectPinFunc1(3,3);                        // TA1.2 -> P3.3
#endif
#if (defined(TA1_CCR2_P2_4_OUT) ||  \
    defined(TA1_CCR2_P2_5_OUT) ||   \
    defined(TA1_CCR2_P3_3_OUT))
	TA1CCTL2 |= OUTMOD_7;						// PWM reset/set
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_start()
// Description:
//! \brief      Start PWM generation (based on TIMER1)
//------------------------------------------------------------------------------
void PWM_start(void)
{
#ifdef TA1_CCR0_P2_0_OUT
    SetPinOut(2,0);                             // enable output P2.0 
#endif
#ifdef TA1_CCR0_P2_3_OUT
    SetPinOut(2,3);                             // enable output P2.3 
#endif
#ifdef TA1_CCR0_P3_1_OUT
    SetPinOut(3,1);                             // enable output P3.1
#endif

#ifdef TA1_CCR1_P2_1_OUT
    SetPinOut(2,1);                             // enable output P2.1
#endif
#ifdef TA1_CCR1_P2_2_OUT
    SetPinOut(2,2);                             // enable output P2.2 
#endif
#ifdef TA1_CCR1_P3_2_OUT
    SetPinOut(3,2);                             // enable output P3.2
#endif
   
#ifdef TA1_CCR2_P2_4_OUT
    SetPinOut(2,4);                             // enable output P2.4
#endif
#ifdef TA1_CCR2_P2_5_OUT
    SetPinOut(2,5);                             // enable output P2.5
#endif
#ifdef TA1_CCR2_P3_3_OUT
    SetPinOut(3,3);                             // enable output P3.3
#endif
       
    // Start TIMER1
    TA1CTL |= MC_1;                             // up to CCR0
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_stop()
// Description:
//! \brief      Stop PWM generation (based on TIMER1)
//------------------------------------------------------------------------------
void PWM_stop(void)
{
#ifdef TA1_CCR0_P2_0_OUT
    SetPinIn(2,0);                              // disable output P2.0
#endif
#ifdef TA1_CCR0_P2_3_OUT
    SetPinIn(2,3);                              // disable output P2.3 
#endif
#ifdef TA1_CCR0_P3_1_OUT
    SetPinIn(3,1);                             // disable output P3.1
#endif

#ifdef TA1_CCR1_P2_1_OUT
    SetPinIn(2,1);                             // disable output P2.1
#endif
#ifdef TA1_CCR1_P2_2_OUT
    SetPinIn(2,2);                             // disable output P2.2 
#endif
#ifdef TA1_CCR1_P3_2_OUT
    SetPinIn(3,2);                             // disable output P3.2
#endif
   
#ifdef TA1_CCR2_P2_4_OUT
    SetPinIn(2,4);                             // disable output P2.4
#endif
#ifdef TA1_CCR2_P2_5_OUT
    SetPinIn(2,5);                             // disable output P2.5
#endif
#ifdef TA1_CCR2_P3_3_OUT
    SetPinIn(3,3);                             // disable output P3.3
#endif

    // Stop TIMER1
    TA1CTL &= ~MC_3;                            // stop
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_set()
// Description:
//! \brief      PWM set CCR0, CCR1, CCR2
//------------------------------------------------------------------------------
void PWM_set(uint16_t period, uint16_t duty1, uint16_t duty2)
{
    TA1CCR0 = period - 1;
    TA1CCR1 = duty1;
    TA1CCR2 = duty2;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_isEnabled()
// Description:
//! \brief      Get output state of PWM
//------------------------------------------------------------------------------
bool PWM_isEnabled(void)
{
    return (TA1CTL & MC_3)? true: false;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_period()
// Description:
//! \brief      Get period of PWM
//------------------------------------------------------------------------------
uint16_t PWM_period(void)
{
    return (TA1CCR0+1);
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_duty1()
// Description:
//! \brief      Get duty cycle of PWM ch1
//------------------------------------------------------------------------------
uint16_t PWM_duty1(void)
{
    return TA1CCR1;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_duty2()
// Description:
//! \brief      Get duty cycle of PWM ch2
//------------------------------------------------------------------------------
uint16_t PWM_duty2(void)
{
    return TA1CCR2;
}

#endif  // PWM_ENABLED

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

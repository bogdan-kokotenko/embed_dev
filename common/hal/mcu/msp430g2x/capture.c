//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//	File description:
//! \file   hal/mcu/msp430g2x/capture.c
//! \brief  MSP430G2x capture unit implementation			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  26/05/2016 | Bogdan Kokotenko | Initial draft
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

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning PWM: Unknown MCU core, check HAL configuration!
#else

// Check if capture unit is enabled and available
#if (defined(CAPTURE_ENABLED) && defined(__MSP430G2553))

// Check if TIMER or PWM mode selected
#if ( defined(TIMER1_EANBLED) || defined(PWM_EANABLED) )
    #error CAPTURE: Unit is not avaliable if TIMER or PWM mode selected!
#endif

// Keeps higher part of capture values;
static uint16_t CAPTURE_ovfCounter = 0;

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_init()
// Description:
//! \brief      TIMER1 based capture unit initialization
//------------------------------------------------------------------------------
void CAPTURE_init(void)
{
    // Initialize timer
    TA1CTL  |= TACLR;                           // clear

#if (CAPTURE_FREQ == SMCLK_FREQ)
    TA1CTL  |= TASSEL_2;                        // SMCLK
    TA1CTL  |= ID_0;
#elif (CAPTURE_FREQ == SMCLK_FREQ/2)
    TA1CTL  |= TASSEL_2;                        // SMCLK/2
    TA1CTL  |= ID_1;
#elif (CAPTURE_FREQ == SMCLK_FREQ/4)
    TA1CTL  |= TASSEL_2;                        // SMCLK/4
    TA1CTL  |= ID_2;
#elif (CAPTURE_FREQ == SMCLK_FREQ/8)
    TA1CTL  |= TASSEL_2;                        // SMCLK/8
    TA1CTL  |= ID_3;
#elif (CAPTURE_FREQ == ACLK_FREQ)
    TA1CTL  |= TASSEL_1;                        // ACLK
    TA1CTL  |= ID_0;
#elif (CAPTURE_FREQ == ACLK_FREQ/2)
    TA1CTL  |= TASSEL_1;                        // ACLK/2
    TA1CTL  |= ID_1;
#elif (CAPTURE_FREQ == ACLK_FREQ/4)
    TA1CTL  |= TASSEL_1;                        // ACLK/4
    TA1CTL  |= ID_2;
#elif (CAPTURE_FREQ == ACLK_FREQ/8)
    TA1CTL  |= TASSEL_1;                        // ACLK/8
    TA1CTL  |= ID_3;
#else
    #error CAPTURE: Unsupported TIMER1 clock frequency
#endif

    TA1CCTL0 |= CAP;                            // capture mode
#if (defined(CAPTURE_CCI0A_P2_0) ||  defined(CAPTURE_CCI0B_P2_3))
    TA1CCTL0 &= ~(CM0+CM1);                     // clear capture mode
#endif
#ifdef CAPTURE_CCI0A_P2_0
    SetPinIn(2,0);                              // enable input P2.0
    #if (CAPTURE_MODE_CCI0 == CAPTURE_RISING)
        EnablePullDown(2,0);
    #elif (CAPTURE_MODE_CCI0 == CAPTURE_FALING)
        EnablePullUp(2,0);
    #else
        DisablePullUp(2,0);
    #endif
    SelectPinFunc1(2,0);                        // TA1.0 <- P2.0
#endif
#ifdef CAPTURE_CCI0B_P2_3
    SetPinIn(2,3);                              // enable input P2.3 
    #if (CAPTURE_MODE_CCI0 == CAPTURE_RISING)
        EnablePullDown(2,3);
    #elif (CAPTURE_MODE_CCI0 == CAPTURE_FALING)
        EnablePullUp(2,3);
    #else
        DisablePullUp(2,3);
    #endif
    SelectPinFunc1(2,3);                        // TA1.0 <- P2.3
#endif

    TA1CCTL1 |= CAP;                            // capture mode
#if (defined(CAPTURE_CCI1A_P2_1) || defined(CAPTURE_CCI1B_P2_2))
    TA1CCTL1 &= ~(CM0+CM1);                     // clear capture mode
#endif
#ifdef CAPTURE_CCI1A_P2_1
    SetPinIn(2,1);                              // enable input P2.1
    #if (CAPTURE_MODE_CCI1 == CAPTURE_RISING)
        EnablePullDown(2,1);
    #elif (CAPTURE_MODE_CCI1 == CAPTURE_FALING)
        EnablePullUp(2,1);
    #else
        DisablePullUp(2,1);
    #endif
    SelectPinFunc1(2,1);                        // TA1.1 <- P2.1
#endif
#ifdef CAPTURE_CCI1B_P2_2
    SetPinIn(2,2);                              // enable input P2.2 
    #if (CAPTURE_MODE_CCI1 == CAPTURE_RISING)
        EnablePullDown(2,2);
    #elif (CAPTURE_MODE_CCI1 == CAPTURE_FALING)
        EnablePullUp(2,2);
    #else
        DisablePullUp(2,2);
    #endif
    SelectPinFunc1(2,2);                        // TA1.1 <- P2.2
#endif

    TA1CCTL2 |= CAP;                            // capture mode
#if (defined(CAPTURE_CCI2A_P2_4) || defined(CAPTURE_CCI2B_P2_5) )
    TA1CCTL2 &= ~(CM0+CM1);                     // clear capture mode
#endif
#ifdef CAPTURE_CCI2A_P2_4
    SetPinIn(2,4);                              // enable input P2.4
    #if (CAPTURE_MODE_CCI2 == CAPTURE_RISING)
        EnablePullDown(2,4);
    #elif (CAPTURE_MODE_CCI2 == CAPTURE_FALING)
        EnablePullUp(2,4);
    #else
        DisablePullUp(2,4);
    #endif
    SelectPinFunc1(2,4);                        // TA1.2 <- P2.4
#endif
#ifdef CAPTURE_CCI2B_P2_5
    SetPinIn(2,5);                              // enable input P2.5 
    #if (CAPTURE_MODE_CCI2 == CAPTURE_RISING)
        EnablePullDown(2,5);
    #elif (CAPTURE_MODE_CCI2 == CAPTURE_FALING)
        EnablePullUp(2,5);
    #else
        DisablePullUp(2,5);
    #endif
    SelectPinFunc1(2,5);                        // TA1.2 <- P2.5
#endif

    TA1CTL &= ~TAIFG;                           // clear OVF IFG
    TA1CTL |=  TAIE;                            // enable OVF interrupt
    
    // Start TIMER1
    TA1CTL |= MC_2;                             // continuous mode
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI0A()
// Description:
//! \brief      Wait external trigger on CCI0A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI0A(void)
{
    TA1CCTL0 &= ~CCIFG;                         // clear CCI0 IFG
    TA1CCTL0 &= ~(CCIS0 + CCIS1);               // select CCI0A
    TA1CCTL0 |= CAPTURE_MODE_CCI0;              // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI0B()
// Description:
//! \brief      Wait external trigger on CCI0B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI0B(void)
{
    TA1CCTL0 &= ~CCIFG;                         // clear CCI0 IFG
    TA1CCTL0 |= CCIS0;                          // select CCI0B
    TA1CCTL0 |= CAPTURE_MODE_CCI0;              // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI0()
// Description:
//! \brief      Stop capturing external triggers on CCI0 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI0(void)
{
    TA1CCTL0 &= ~(CM0+CM1);                     // no capture
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI1A()
// Description:
//! \brief      Wait external trigger on CCI1A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI1A(void)
{
    TA1CCTL1 &= ~CCIFG;                         // clear CCI1 IFG
    TA1CCTL1 &= ~(CCIS0 + CCIS1);               // select CCI1A
    TA1CCTL1 |= CAPTURE_MODE_CCI1;              // set capture mode 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI1B()
// Description:
//! \brief      Wait external trigger on CCI1B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI1B(void)
{
    TA1CCTL1 &= ~CCIFG;                         // clear CCI1 IFG
    TA1CCTL1 |= CCIS0;                          // select CCI1B
    TA1CCTL1 |= CAPTURE_MODE_CCI1;              // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI1()
// Description:
//! \brief      Stop capturing external triggers on CCI1 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI1(void)
{
    TA1CCTL1 &= ~(CM0+CM1);                     // no capture
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI2A()
// Description:
//! \brief      Wait external trigger on CCI2A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI2A(void)
{
    TA1CCTL2 &= ~CCIFG;                         // clear CCI2 IFG
    TA1CCTL2 &= ~(CCIS0 + CCIS1);               // select CCI2A
    TA1CCTL2 |= CAPTURE_MODE_CCI2;              // set capture mode 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI2B()
// Description:
//! \brief      Wait external trigger on CCI2B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI2B(void)
{
    TA1CCTL2 &= ~CCIFG;                         // clear CCI2 IFG
    TA1CCTL2 |= CCIS0;                          // select CCI2B
    TA1CCTL2 |= CAPTURE_MODE_CCI2;              // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI2()
// Description:
//! \brief      Stop capturing external triggers on CCI2 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI2(void)
{
    TA1CCTL2 &= ~(CM0+CM1);                     // no capture
}

//------------------------------------------------------------------------------
// Function:	
//				CAPTURE_isr0()
// Description:
//! \brief      CAPTURE interrupt service routine (CCI0)
//------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void CAPTURE_isr0(void)
{
    TA1CCTL0 &= ~CCIFG;                         // clear CCI0 IFG
        
    #ifdef CAPTURE_HandlerCCI0
        uint16_t highWord = (CAPTURE_ovfCounter+((TA1CTL & TAIFG)?1:0));
        CAPTURE_HandlerCCI0(((uint32_t)highWord<<16) | TA1CCR0);
    #endif //CAPTURE_HandlerCCI0

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                              // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_isr1() 
// Description:
//! \brief      CAPTURE interrupt service routine (CCI1,CCI2)
//------------------------------------------------------------------------------
#pragma vector=TIMER1_A1_VECTOR
__interrupt void CAPTURE_isr1(void)
{
    if(TA1CCTL1 & CCIFG)
    {
        TA1CCTL1 &= ~CCIFG;                     // clear CCR IFG
    
        #ifdef CAPTURE_HandlerCCI1
            uint16_t highWord = (CAPTURE_ovfCounter+((TA1CTL & TAIFG)?1:0));
            CAPTURE_HandlerCCI1(((uint32_t)highWord<<16) | TA1CCR1);
        #endif //CAPTURE_HandlerCCI1
    }
    if(TA1CCTL2 & CCIFG)
    {
        TA1CCTL2 &= ~CCIFG;                     // clear CCR IFG
    
        #ifdef CAPTURE_HandlerCCI2
            uint16_t highWord = (CAPTURE_ovfCounter+((TA1CTL & TAIFG)?1:0));
            CAPTURE_HandlerCCI2(((uint32_t)highWord<<16) | TA1CCR2);
        #endif //CAPTURE_HandlerCCI2
    }
    if(TA1CTL & TAIFG)
    {
        TA1CTL &= ~TAIFG;                       // clear OVF IFG
    
        // Update overflow counter
        CAPTURE_ovfCounter++;
    }
   
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                           // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//				CAPTURE_clock()
// Description:
//! \brief      Returns the number of capture unit ticks since overflow
//------------------------------------------------------------------------------
uint32_t CAPTURE_clock(void)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);
    
    uint16_t tmValue = TA1R;
    uint16_t ovfValue = (TA1CTL & TAIFG) ? 1 : 0;
    
    if(tmValue > TA1R)   // check low part overflow
    {
        tmValue = TA1R;
        ovfValue = (TA1CTL & TAIFG) ? 1 : 0;
    }
        
    uint32_t clocks = (((uint32_t)(CAPTURE_ovfCounter+ovfValue)<<16) | tmValue);
    
    LeaveCriticalSection(intState);                   
                       
    return clocks;
}

#endif  // CAPTURE_ENABLED

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

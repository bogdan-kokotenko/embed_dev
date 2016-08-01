//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/capture.c
//! \brief  MSP430F5x capture unit implementation			
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
#if ( !defined (_MSP430F5x_HAL_) )
#warning PWM: Unknown MCU core, check HAL configuration!
#else

// Check if capture unit is enabled and available
#if (defined(CAPTURE_ENABLED))

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
    CAPTURE_REG(CTL)  |= TACLR;                 // clear
    
#ifdef CAPTURE_TACLK
    GPIO_setIn(CAPTURE_TACLK_PIN);
    GPIO_func2(CAPTURE_TACLK_PIN);
#endif // CAPTURE_TACLK

#if (CAPTURE_FREQ == CAPTURE_TACLK)
    CAPTURE_REG(CTL) |= TASSEL__TACLK;          // TACLK
    CAPTURE_REG(CTL) &= ~ID_3;
#elif (CAPTURE_FREQ == ACLK_FREQ)
    CAPTURE_REG(CTL) |= TASSEL_1;               // ACLK
    CAPTURE_REG(CTL) |= ID_0;
#elif (CAPTURE_FREQ == ACLK_FREQ/2)
    CAPTURE_REG(CTL) |= TASSEL_1;               // ACLK/2
    CAPTURE_REG(CTL) |= ID_1;
#elif (CAPTURE_FREQ == ACLK_FREQ/4)
    CAPTURE_REG(CTL) |= TASSEL_1;               // ACLK/4
    CAPTURE_REG(CTL) |= ID_2;
#elif (CAPTURE_FREQ == ACLK_FREQ/8)
    CAPTURE_REG(CTL) |= TASSEL_1;               // ACLK/8
    CAPTURE_REG(CTL) |= ID_3;
#elif (CAPTURE_FREQ == SMCLK_FREQ)
    CAPTURE_REG(CTL) |= TASSEL_2;               // SMCLK
    CAPTURE_REG(CTL) |= ID_0;
#elif (CAPTURE_FREQ == SMCLK_FREQ/2)
    CAPTURE_REG(CTL) |= TASSEL_2;               // SMCLK/2
    CAPTURE_REG(CTL) |= ID_1;
#elif (CAPTURE_FREQ == SMCLK_FREQ/4)
    CAPTURE_REG(CTL) |= TASSEL_2;               // SMCLK/4
    CAPTURE_REG(CTL) |= ID_2;
#elif (CAPTURE_FREQ == SMCLK_FREQ/8)
    CAPTURE_REG(CTL) |= TASSEL_2;               // SMCLK/8
    CAPTURE_REG(CTL) |= ID_3;
#else
    #error CAPTURE: Unsupported TIMER clock frequency
#endif

    CAPTURE_REG(CCTL0) |= CAP;                  // capture mode
#if (defined(CAPTURE_CCI0A_PIN) ||  defined(CAPTURE_CCI0B_PIN))
    CAPTURE_REG(CCTL0) &= ~(CM0+CM1);           // clear capture mode
#endif
#ifdef CAPTURE_CCI0A_PIN
    GPIO_setIn(CAPTURE_CCI0A_PIN);                // enable input CCI0A
    #if (CAPTURE_MODE_CCI0 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI0A_PIN);
    #elif (CAPTURE_MODE_CCI0 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI0A_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI0A_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI0A_PIN);          // CAPTURE_0 <- CCI0A
#endif
#ifdef CAPTURE_CCI0B_PIN
    GPIO_setIn(CAPTURE_CCI0B_PIN);                // enable input CCI0B 
    #if (CAPTURE_MODE_CCI0 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI0B_PIN);
    #elif (CAPTURE_MODE_CCI0 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI0B_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI0B_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI0B_PIN);          // CAPTURE_1 <- CCI0B
#endif

    CAPTURE_REG(CCTL1) |= CAP;                  // capture mode
#if (defined(CAPTURE_CCI1A_PIN) || defined(CAPTURE_CCI1B_PIN))
    CAPTURE_REG(CCTL1) &= ~(CM0+CM1);           // clear capture mode
#endif
#ifdef CAPTURE_CCI1A_PIN
    GPIO_setIn(CAPTURE_CCI1A_PIN);                // enable input CCI1A
    #if (CAPTURE_MODE_CCI1 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI1A_PIN);
    #elif (CAPTURE_MODE_CCI1 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI1A_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI1A_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI1A_PIN);          // CAPTURE_1 <- CCIA
#endif
#ifdef CAPTURE_CCI1B_PIN
    GPIO_setIn(CAPTURE_CCI1B_PIN);                // enable input CCI1B 
    #if (CAPTURE_MODE_CCI1 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI1B_PIN);
    #elif (CAPTURE_MODE_CCI1 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI1B_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI1B_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI1B_PIN);          // CAPTURE_1 <- CCI1B
#endif

    CAPTURE_REG(CCTL2) |= CAP;                  // capture mode
#if (defined(CAPTURE_CCI2A_PIN) || defined(CAPTURE_CCI2B_PIN))
    CAPTURE_REG(CCTL2) &= ~(CM0+CM1);           // clear capture mode
#endif
#ifdef CAPTURE_CCI2A_PIN
    GPIO_setIn(CAPTURE_CCI2A_PIN);                // enable input CCI2A
    #if (CAPTURE_MODE_CCI2 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI2A_PIN);
    #elif (CAPTURE_MODE_CCI2 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI2A_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI2A_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI2A_PIN);          // CAPTURE_2 <- CCI2A
#endif
#ifdef CAPTURE_CCI2B_PIN
    GPIO_setIn(CAPTURE_CCI2B_PIN);                // enable input CCI2B 
    #if (CAPTURE_MODE_CCI2 == CAPTURE_RISING)
        GPIO_pullDown(CAPTURE_CCI2B_PIN);
    #elif (CAPTURE_MODE_CCI2 == CAPTURE_FALING)
        GPIO_pullUp(CAPTURE_CCI2B_PIN);
    #else
        GPIO_pullDisable(CAPTURE_CCI2B_PIN);
    #endif
    GPIO_func2(CAPTURE_CCI2B_PIN);          // CAPTURE_2 <- CCI2B
#endif

    CAPTURE_REG(CTL) &= ~TAIFG;                 // clear OVF IFG
    CAPTURE_REG(CTL) |=  TAIE;                  // enable OVF interrupt
    
    // Start TIMER
    CAPTURE_REG(CTL) |= MC_2;                   // continuous mode
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI0A()
// Description:
//! \brief      Wait external trigger on CCI0A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI0A(void)
{
    CAPTURE_REG(CCTL0) &= ~CCIFG;               // clear CCI0 IFG
    CAPTURE_REG(CCTL0) &= ~(CCIS0 + CCIS1);     // select CCI0A
    CAPTURE_REG(CCTL0) |= CAPTURE_MODE_CCI0;    // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI0B()
// Description:
//! \brief      Wait external trigger on CCI0B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI0B(void)
{
    CAPTURE_REG(CCTL0) &= ~CCIFG;               // clear CCI0 IFG
    CAPTURE_REG(CCTL0) |= CCIS0;                // select CCI0B
    CAPTURE_REG(CCTL0) |= CAPTURE_MODE_CCI0;    // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI0()
// Description:
//! \brief      Stop capturing external triggers on CCI0 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI0(void)
{
    CAPTURE_REG(CCTL0) &= ~(CM0+CM1);           // no capture
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI1A()
// Description:
//! \brief      Wait external trigger on CCI1A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI1A(void)
{
    CAPTURE_REG(CCTL1) &= ~CCIFG;               // clear CCI1 IFG
    CAPTURE_REG(CCTL1) &= ~(CCIS0 + CCIS1);     // select CCI1A
    CAPTURE_REG(CCTL1) |= CAPTURE_MODE_CCI1;    // set capture mode 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI1B()
// Description:
//! \brief      Wait external trigger on CCI1B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI1B(void)
{
    CAPTURE_REG(CCTL1) &= ~CCIFG;               // clear CCI1 IFG
    CAPTURE_REG(CCTL1) |= CCIS0;                // select CCI1B
    CAPTURE_REG(CCTL1) |= CAPTURE_MODE_CCI1;    // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI1()
// Description:
//! \brief      Stop capturing external triggers on CCI1 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI1(void)
{
    CAPTURE_REG(CCTL1) &= ~(CM0+CM1);           // no capture
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI2A()
// Description:
//! \brief      Wait external trigger on CCI2A
//------------------------------------------------------------------------------
void CAPTURE_waitCCI2A(void)
{
    CAPTURE_REG(CCTL2) &= ~CCIFG;               // clear CCI2 IFG
    CAPTURE_REG(CCTL2) &= ~(CCIS0 + CCIS1);     // select CCI2A
    CAPTURE_REG(CCTL2) |= CAPTURE_MODE_CCI2;    // set capture mode 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_waitCCI2B()
// Description:
//! \brief      Wait external trigger on CCI2B
//------------------------------------------------------------------------------
void CAPTURE_waitCCI2B(void)
{
    CAPTURE_REG(CCTL2) &= ~CCIFG;               // clear CCI2 IFG
    CAPTURE_REG(CCTL2) |= CCIS0;                // select CCI2B
    CAPTURE_REG(CCTL2) |= CAPTURE_MODE_CCI2;    // set capture mode    
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_stopCCI2()
// Description:
//! \brief      Stop capturing external triggers on CCI2 (stop TIMER1)
//------------------------------------------------------------------------------
void CAPTURE_stopCCI2(void)
{
    CAPTURE_REG(CCTL2) &= ~(CM0+CM1);           // no capture
}

//------------------------------------------------------------------------------
// Function:	
//				CAPTURE_isr0()
// Description:
//! \brief      CAPTURE interrupt service routine (CCI0)
//------------------------------------------------------------------------------
#pragma vector=CAPTURE_ISR_VECTOR(0)
__interrupt void CAPTURE_isr0(void)
{
    CAPTURE_REG(CCTL0) &= ~CCIFG;               // clear CCI0 IFG
        
    #ifdef CAPTURE_HandlerCCI0
        uint16_t highWord = (CAPTURE_ovfCounter+((CAPTURE_REG(CTL)&TAIFG)?1:0));
        CAPTURE_HandlerCCI0(((uint32_t)highWord<<16) | CAPTURE_REG(CCR0));
    #endif //CAPTURE_HandlerCCI0

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              CAPTURE_isr1() 
// Description:
//! \brief      CAPTURE interrupt service routine (CCI1,CCI2)
//------------------------------------------------------------------------------
#pragma vector=CAPTURE_ISR_VECTOR(1)
__interrupt void CAPTURE_isr1(void)
{
    if(CAPTURE_REG(CCTL1) & CCIFG)
    {
        CAPTURE_REG(CCTL1) &= ~CCIFG;           // clear CCR IFG
    
        #ifdef CAPTURE_HandlerCCI1
            uint16_t highWord = (CAPTURE_ovfCounter+((CAPTURE_REG(CTL)&TAIFG)?1:0));
            CAPTURE_HandlerCCI1(((uint32_t)highWord<<16) | CAPTURE_REG(CCR1));
        #endif //CAPTURE_HandlerCCI1
    }
    if(CAPTURE_REG(CCTL2) & CCIFG)
    {
        CAPTURE_REG(CCTL2) &= ~CCIFG;           // clear CCR IFG
    
        #ifdef CAPTURE_HandlerCCI2
            uint16_t highWord = (CAPTURE_ovfCounter+((CAPTURE_REG(CTL) & TAIFG)?1:0));
            CAPTURE_HandlerCCI2(((uint32_t)highWord<<16) | CAPTURE_REG(CCR2));
        #endif //CAPTURE_HandlerCCI2
    }
    if(CAPTURE_REG(CTL) & TAIFG)
    {
        CAPTURE_REG(CTL) &= ~TAIFG;             // clear OVF IFG
    
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
    
    uint16_t tmValue = CAPTURE_REG(R);
    uint16_t ovfValue = (CAPTURE_REG(CTL) & TAIFG) ? 1 : 0;
    
    if(tmValue > CAPTURE_REG(R))   // check low part overflow
    {
        tmValue = CAPTURE_REG(R);
        ovfValue = (CAPTURE_REG(CTL) & TAIFG) ? 1 : 0;
    }
        
    uint32_t clocks = (((uint32_t)(CAPTURE_ovfCounter+ovfValue)<<16) | tmValue);
    
    LeaveCriticalSection(intState);                   
                       
    return clocks;
}

#endif  // CAPTURE_ENABLED

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

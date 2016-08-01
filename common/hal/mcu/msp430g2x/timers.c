//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/timers.c
//! \brief  MSP430G2x Timers functions implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  24/01/2015 | Bogdan Kokotenko | Initial draft
//!  25/04/2015 | Bogdan Kokotenko | Fixed issue with timer dividers
//!   2/04/2015 | Bogdan Kokotenko | Fixed wrong timer interrupt handling
//!  10/11/2015 | Bogdan Kokotenko | Improved timer settings API
//!  25/05/2016 | Bogdan Kokotenko | Proper usage of TACCR0 and OVF interrupt
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"
#include "devtime.h"
#include "pwm.h"
#include "capture.h"
#include "timers.h"
#include "thread.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

//! Keeps high part of clock_t
static int16_t CLOCK_counter = 0;

//! TIMER context
struct _TIMER_context{
    uint16_t compare;
    void (*handler)(void);
}TIMER0_context[3], TIMER1_context[3];

//------------------------------------------------------------------------------
// Function:	
//              WDT_init()
// Description:
//! \brief      Watchdog timer initialization
//------------------------------------------------------------------------------
void WDT_init(void)
{
#if ( defined (WDT_RST) )
    // Clear WDT and prepare for reset mode
    WDT_feedWatchdog();    
    
#elif ( defined (WDT_INTERVAL) )
  
    // Interval mode
    WDTCTL =  (WDTPW + WDTTMSEL + WDTCNTCL +
    #if ( (WDT_INTERVAL == 1000) && (ACLK_FREQ == 32768))   // 1.0 s          
            WDTSSEL);                                       // ACLK selected
    #elif ( (WDT_INTERVAL == 16) && (ACLK_FREQ == 32768))   // ~0.016 s          
            WDTSSEL + WDTIS1);                              // ACLK selected
    #elif ( (WDT_INTERVAL == 33) && (SMCLK_FREQ == 1000000L))// ~0.033 s          
            );                                              // SMCLK selected
    #elif ( (WDT_INTERVAL == 16) && (SMCLK_FREQ == 2000000L))// ~0.016 s          
            );                                              // SMCLK selected
    #else
        #error TIMERS: Unsupported WDT interval
    #endif

    IE1 |= WDTIE;								    // Enable WDT interrupt
#else
    // Stop watchdog timer to prevent time out reset
    WDTCTL = (WDTPW + WDTHOLD);
#endif
}

//------------------------------------------------------------------------------
// Function:	
//				WDT_feedWatchdog()
// Description:
//! \brief      Clear watchdog timer to prevent time out reset
//------------------------------------------------------------------------------
void WDT_feedWatchdog(void)
{
#if (defined (ACLK_FREQ))    
    // Reset mode, clocked by independent source ACLK
    WDTCTL =  (WDTPW + WDTSSEL +                        // ACLK selected
               WDTCNTCL + WDTIS0);                      // 0.25~0.8 s 
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              WDT_isr()
// Description:
//! \brief      Watchdog Timer interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=WDT_VECTOR
__interrupt void WDT_isr(void)
{
    #ifdef WDT_Handler
        WDT_Handler();
    #endif //WDT_Handler
  
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                              // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              SysTick_init()
// Description:
//! \brief      SysTick (TIMER0) initialization (TA0, CCR0)
//------------------------------------------------------------------------------
void SysTick_init(uint16_t period)
{
    TA0CTL  |= TACLR;                           // clear

#if (T0CLK_FREQ == SMCLK_FREQ)
    TA0CTL  |= TASSEL_2;                        // SMCLK
    TA0CTL  |= ID_0;
#elif (T0CLK_FREQ == SMCLK_FREQ/2)
    TA0CTL  |= TASSEL_2;                        // SMCLK/2
    TA0CTL  |= ID_1;
#elif (T0CLK_FREQ == SMCLK_FREQ/4)
    TA0CTL  |= TASSEL_2;                        // SMCLK/4
    TA0CTL  |= ID_2;
#elif (T0CLK_FREQ == SMCLK_FREQ/8)
    TA0CTL  |= TASSEL_2;                        // SMCLK/8
    TA0CTL  |= ID_3;
#elif (T0CLK_FREQ == ACLK_FREQ)
    TA0CTL  |= TASSEL_1;                        // ACLK
    TA0CTL  |= ID_0;
#elif (T0CLK_FREQ == ACLK_FREQ/2)
    TA0CTL  |= TASSEL_1;                        // ACLK/2
    TA0CTL  |= ID_1;
#elif (T0CLK_FREQ == ACLK_FREQ/4)
    TA0CTL  |= TASSEL_1;                        // ACLK/4
    TA0CTL  |= ID_2;
#elif (T0CLK_FREQ == ACLK_FREQ/8)
    TA0CTL  |= TASSEL_1;                        // ACLK/8
    TA0CTL  |= ID_3;
#else
    #error TIMERS: Unsupported TIMER0 clock frequency
#endif

#if (T0CLK_FREQ != CLOCKS_PER_SEC)
    #error TIMERS: Unsupported TIMER0 settings
#endif
   
    TA0CTL |=  TAIE;                            // enable OVF interrupt
    TA0CTL &= ~TAIFG;                           // clear OVF IFG
    
    // Save period to timer context
    TIMER0_context[0].compare = period;
    TIMER0_context[0].handler = NULL;
    
    // Apply new settings
    TA0CCR0 = TA0R + TIMER0_context[0].compare;
    //TA0CCTL0 |= OUTMOD_4;                     // toggle mode
    TA0CCTL0 |= CCIE;                           // enable CCR0 interrupt
    TA0CCTL0 &= ~CCIFG;                         // clear CCR0 IFG

	TA0CTL |=   MC_2;                           // continuous mode
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_init1()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR1)
//------------------------------------------------------------------------------
void TIMER0_init1(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER0_context[1].compare = period;
    TIMER0_context[1].handler = handler;
    
    // Apply new settings
    TA0CCR1 = TA0R + TIMER0_context[1].compare;
    //TA0CCTL1 |= OUTMOD_4;                     // toggle mode
    TA0CCTL1 |= CCIE;                           // enable CCR1 interrupt
    TA0CCTL1 &= ~CCIFG;                         // clear CCR1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_stop1()
// Description:
//! \brief      TIMER0 stop (TA0, CCR1)
//------------------------------------------------------------------------------
void TIMER0_stop1(void)
{
    // Clear handler in timer context
    TIMER0_context[1].handler = NULL;
    
    // Apply new settings
    TA0CCTL1 &= ~OUTMOD_7;                      // claer output mode
    TA0CCTL1 &= ~CCIE;                          // disable CCR1 interrupt
    TA0CCTL1 &= ~CCIFG;                         // clear CCR1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_init2()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR2)
//------------------------------------------------------------------------------
void TIMER0_init2(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER0_context[2].compare = period;
    TIMER0_context[2].handler = handler;
    
    // Apply new settings
    TA0CCR2 = TA0R + TIMER0_context[2].compare;
    TA0CCTL2 |= OUTMOD_4;                       // toggle mode
	TA0CCTL2 |= CCIE;                           // enable CCR2 interrupt
    TA0CCTL2 &= ~CCIFG;                         // clear CCR2 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_stop2()
// Description:
//! \brief      TIMER0 stop (TA0, CCR2)
//------------------------------------------------------------------------------
void TIMER0_stop2(void)
{
    // Clear handler in timer context
    TIMER0_context[2].handler = NULL;
    
    // Apply new settings
    TA0CCTL2 &= ~OUTMOD_7;                      // claer output mode
    TA0CCTL2 &= ~CCIE;                          // disable CCR2 interrupt
    TA0CCTL2 &= ~CCIFG;                         // clear CCR2 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_isr0() 
// Description:
//! \brief      TIMER0 interrupt service routine (TA0, CCR0)
//------------------------------------------------------------------------------
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_isr0(void)
{
    TA0CCTL0 &= ~CCIFG;                         // clear CCR0 IFG
    TA0CCR0 += TIMER0_context[0].compare;       // update CCR
        
    #ifdef SysTick_Handler0
        SysTick_Handler0();
    #endif //SysTick_Handler0
        
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                              // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_isr1() 
// Description:
//! \brief      TIMER0 interrupt service routine (CCR1-CCR4)
//------------------------------------------------------------------------------
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_isr1(void)
{
    if(TA0CCTL1 & CCIFG)
    {
        TA0CCTL1 &= ~CCIFG;                     // clear CCR IFG
        TA0CCR1 += TIMER0_context[1].compare;   // update CCR
    
        if(TIMER0_context[1].handler)
            TIMER0_context[1].handler();
    }
    if(TA0CCTL2 & CCIFG)
    {
        TA0CCTL2 &= ~CCIFG;                     // clear CCR IFG
        TA0CCR2 += TIMER0_context[2].compare;   // update CCR
    
        if(TIMER0_context[2].handler)
            TIMER0_context[2].handler();
    }
    if(TA0CTL & TAIFG)
    {
        TA0CTL &= ~TAIFG;                       // clear OVF IFG
    
        // Update clock counter
        if(++CLOCK_counter < 0)
            CLOCK_counter = 0;
        
        #ifdef TIMER0_Overflow
            TIMER0_Overflow();
        #endif //TIMER0_Overflow
    }
   
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                           // Wake-up MCU
    #endif 
}

// Check if TIMER1 is enabled and avaliable
#if (defined(TIMER1_ENABLED) && defined(__MSP430G2553))

// Check if PWM or CAPTURE mode selected
#if ( defined(PWM_EANBLED) || defined(CAPTURE_ENABLED) )
    #error TIMER1: Unit is not avaliable if PWM or CAPTURE mode selected!
#endif

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_init()
// Description:
//! \brief      TIMER1 initialization (TA1)
//------------------------------------------------------------------------------
void TIMER1_init(void)
{
    TA1CTL  |= TACLR;                           // clear

#if (T1CLK_FREQ == SMCLK_FREQ)
    TA1CTL  |= TASSEL_2;                        // SMCLK
    TA1CTL  |= ID_0;
#elif (T1CLK_FREQ == SMCLK_FREQ/2)
    TA1CTL  |= TASSEL_2;                        // SMCLK/2
    TA1CTL  |= ID_1;
#elif (T1CLK_FREQ == SMCLK_FREQ/4)
    TA1CTL  |= TASSEL_2;                        // SMCLK/4
    TA1CTL  |= ID_2;
#elif (T1CLK_FREQ == SMCLK_FREQ/8)
    TA1CTL  |= TASSEL_2;                        // SMCLK/8
    TA1CTL  |= ID_3;
#elif (T1CLK_FREQ == ACLK_FREQ)
    TA1CTL  |= TASSEL_1;                        // ACLK
    TA1CTL  |= ID_0;
#elif (T1CLK_FREQ == ACLK_FREQ/2)
    TA1CTL  |= TASSEL_1;                        // ACLK/2
    TA1CTL  |= ID_1;
#elif (T1CLK_FREQ == ACLK_FREQ/4)
    TA1CTL  |= TASSEL_1;                        // ACLK/4
    TA1CTL  |= ID_2;
#elif (T1CLK_FREQ == ACLK_FREQ/8)
    TA1CTL  |= TASSEL_1;                        // ACLK/8
    TA1CTL  |= ID_3;
#else
    #error TIMERS: Unsupported TIMER1 clock frequency
#endif
    
    #ifdef TIMER1_OvfHandler
    TA1CTL &= ~TAIFG;                           // clear OVF IFG
    TA1CTL |=  TAIE;                            // enable OVF interrupt
    #endif
    
	TA1CTL |=   MC_2;                           // continuous up
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_stop0()
// Description:
//! \brief      TIMER1 stop (TA1, CCR0)
//------------------------------------------------------------------------------
void TIMER1_stop0(void)
{
    // Clear handler in timer context
    TIMER1_context[0].handler = NULL;
    
    // Apply new settings
    TA1CCTL0 &= ~OUTMOD_7;                      // claer output mode
    TA1CCTL0 &= ~CCIE;                          // disable CCR0 interrupt
    TA1CCTL0 &= ~CCIFG;                         // clear CCR0 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_init1()
// Description:
//! \brief      TIMER1 initialization (TA1, CCR1)
//------------------------------------------------------------------------------
void TIMER1_init1(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER1_context[1].compare = period;
    TIMER1_context[1].handler = handler;
    
    // Apply new settings
    TA1CCR1 = TA1R + TIMER1_context[1].compare;
    //TA1CCTL1 |= OUTMOD_4;                     // toggle mode
    TA1CCTL1 |= CCIE;                           // enable CCR1 interrupt
    TA1CCTL1 &= ~CCIFG;                         // clear CCR1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_stop1()
// Description:
//! \brief      TIMER1 stop (TA1, CCR1)
//------------------------------------------------------------------------------
void TIMER1_stop1(void)
{
    // Clear handler in timer context
    TIMER1_context[1].handler = NULL;
    
    // Apply new settings
    TA1CCTL1 &= ~OUTMOD_7;                      // claer output mode
    TA1CCTL1 &= ~CCIE;                          // disable CCR1 interrupt
    TA1CCTL1 &= ~CCIFG;                         // clear CCR1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_init2()
// Description:
//! \brief      TIMER1 initialization (TA1, CCR2)
//------------------------------------------------------------------------------
void TIMER1_init2(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER1_context[2].compare = period;
    TIMER1_context[2].handler = handler;
    
    // Apply new settings
    TA1CCR2 = TA1R + TIMER1_context[2].compare;
    TA1CCTL2 |= OUTMOD_4;                       // toggle mode
	TA1CCTL2 |= CCIE;                           // enable CCR2 interrupt
    TA1CCTL2 &= ~CCIFG;                         // clear CCR2 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_stop2()
// Description:
//! \brief      TIMER1 stop (TA1, CCR2)
//------------------------------------------------------------------------------
void TIMER1_stop2(void)
{
    // Clear handler in timer context
    TIMER1_context[2].handler = NULL;
    
    // Apply new settings
    TA1CCTL2 &= ~OUTMOD_7;                      // claer output mode
    TA1CCTL2 &= ~CCIE;                          // disable CCR2 interrupt
    TA1CCTL2 &= ~CCIFG;                         // clear CCR2 IFG
}

//------------------------------------------------------------------------------
// Function:	
//				TIMER1_isr0()
// Description:
//! \brief      TIMER1 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_isr0(void)
{
    TA1CCTL0 &= ~CCIFG;                         // clear CCR0 IFG
    TA1CCR0 += TIMER1_context[0].compare;       // update CCR
        
    #ifdef TIMER1_Handler0
        TIMER1_Handler0();
    #endif //TIMER0_Handler0

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                              // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_isr1() 
// Description:
//! \brief      TIMER1 interrupt service routine (CCR1-CCR4)
//------------------------------------------------------------------------------
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_isr1(void)
{
    if(TA1CCTL1 & CCIFG)
    {
        TA1CCTL1 &= ~CCIFG;                     // clear CCR IFG
        TA1CCR1 += TIMER1_context[1].compare;   // update CCR
    
        if(TIMER1_context[1].handler)
            TIMER1_context[1].handler();
    }
    if(TA1CCTL2 & CCIFG)
    {
        TA1CCTL2 &= ~CCIFG;                     // clear CCR IFG
        TA1CCR2 += TIMER1_context[2].compare;   // update CCR
    
        if(TIMER1_context[2].handler)
            TIMER1_context[2].handler();
    }
    if(TA1CTL & TAIFG)
    {
        TA1CTL &= ~TAIFG;                       // clear OVF IFG
    
        // Update clock counter
        if(++CLOCK_counter < 0)
            CLOCK_counter = 0;
        
        #ifdef TIMER1_Overflow
            TIMER1_Overflow();
        #endif //TIMER1_Overflow
    }
   
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                           // Wake-up MCU
    #endif 
}
#endif  // TIMER1_ENABLED

//------------------------------------------------------------------------------
// Function:	
//				clock()
// Description:
//! \brief      Returns the approximate processor time since the restart
//!
//! \note       The value returned by clock() wraps around.
//!             For 16-bit clock_t, it wraps after 32.768 msec. 
//!             For 32-bit clock_t, it wraps after 2147 sec (36 min). 
//------------------------------------------------------------------------------
clock_t clock(void)
{
    EnterCriticalSection();
    
    uint16_t tmValue = TA0R;
    uint16_t ovfValue = (TA0CTL & TAIFG) ? 1 : 0;
    
    if(tmValue > TA0R)   // check low part overflow
    {
        tmValue = TA0R;
        ovfValue = (TA0CTL & TAIFG) ? 1 : 0;
    }
        
    clock_t clocks =   (((int32_t)(CLOCK_counter+ovfValue)<<16) | tmValue);
    
    LeaveCriticalSection();                   
                       
    // Check overflow
    if(clocks < 0)
        clocks = 0;
      
    return clocks;
}

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

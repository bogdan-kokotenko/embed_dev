//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/timers.c
//! \brief  MSP430F5x Timers functions implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  24/01/2015 | Bogdan Kokotenko | Initial draft
//!  25/04/2015 | Bogdan Kokotenko | Fixed issue with timer dividers
//!   2/04/2015 | Bogdan Kokotenko | Fixed wrong timer interrupt handling
//!  05/10/2015 | Bogdan Kokotenko | Fixed clock() timer overflow issue
//!  13/01/2016 | Bogdan Kokotenko | Improved timers settings
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"
#include "devtime.h"
#include "timers.h"
#include "thread.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

//! Keeps high part of clock_t
static int16_t CLOCK_counter = 0;

//! TIMER context
struct _TIMER_context{
    uint16_t compare;
    void (*handler)(void);
}TIMER0_context[5], TIMER1_context[3];

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
    #if ( (WDT_INTERVAL == 1000) && (ACLK == 32768))    // 1.0 s          
               WDTSSEL__ACLK + WDTIS__32K);             // ACLK selected
    #elif ( (WDT_INTERVAL == 16) && (ACLK == 32768))    // ~0.016 s          
               WDTSSEL__ACLK + WDTIS__32K);             // ACLK selected
    #elif ( (WDT_INTERVAL == 33) && (SMCLK == 1000000)) // ~0.033 s          
               WDTSSEL__SMCLK + WDTIS__32K);            // SMCLK selected
    #elif ( (WDT_INTERVAL == 33) && (SMCLK == 16000000))// ~0.033 s          
               WDTSSEL__SMCLK + WDTIS__512K);           // SMCLK selected
    #else
        #error TIMERS: Unsupported WDT interval
    #endif
 
    SFRIE1 |= WDTIE;								    // Enable WDT interrupt
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
    // Reset mode, clocked by independent source VLO
    WDTCTL =  (WDTPW + WDTSSEL__VLO +                   // VLO selected
               WDTCNTCL + WDTIS__8192);                 // 0.4~1.6 s 
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

#if (T0CLK_FREQ == ACLK_FREQ)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK
    TA0CTL  |= ID_0;
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == ACLK_FREQ/2)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK
    TA0CTL  |= ID_1;                            // []/2
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == ACLK_FREQ/4)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == ACLK_FREQ/8)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == ACLK_FREQ/10)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK
    TA0CTL  |= ID_1;                            // []/2
    TA0EX0  = TAIDEX_4;                         // []/5
#elif (T0CLK_FREQ == ACLK_FREQ/12)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_2;                         // []/3
#elif (T0CLK_FREQ == ACLK_FREQ/16)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_3;                         // []/4
#elif (T0CLK_FREQ == ACLK_FREQ/20)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_4;                         // []/5
#elif (T0CLK_FREQ == ACLK_FREQ/24)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK 
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_2;                         // []/3    
#elif (T0CLK_FREQ == ACLK_FREQ/32)
    TA0CTL  |= TASSEL__ACLK;                    // ACLK 
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_3;                         // []/4    
#elif (T0CLK_FREQ == SMCLK_FREQ)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA0CTL  |= ID_0;
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == SMCLK_FREQ/2)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA0CTL  |= ID_1;                            // []/2
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == SMCLK_FREQ/4)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == SMCLK_FREQ/8)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_0;
#elif (T0CLK_FREQ == SMCLK_FREQ/10)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA0CTL  |= ID_1;                            // []/2
    TA0EX0  = TAIDEX_4;                         // []/5
#elif (T0CLK_FREQ == SMCLK_FREQ/12)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_2;                         // []/3
#elif (T0CLK_FREQ == SMCLK_FREQ/16)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_3;                         // []/4
#elif (T0CLK_FREQ == SMCLK_FREQ/20)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK 
    TA0CTL  |= ID_2;                            // []/4
    TA0EX0  = TAIDEX_4;                         // []/5
#elif (T0CLK_FREQ == SMCLK_FREQ/24)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK 
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_2;                         // []/3    
#elif (T0CLK_FREQ == SMCLK_FREQ/32)
    TA0CTL  |= TASSEL__SMCLK;                   // SMCLK 
    TA0CTL  |= ID_3;                            // []/8
    TA0EX0  = TAIDEX_3;                         // []/4  
#else
    #error TIMERS: Unsupported TIMER0 clock frequency
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
    
	TA0CTL |=   MC_2;                           // continuous up
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
//              TIMER0_init3()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR3)
//------------------------------------------------------------------------------
void TIMER0_init3(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER0_context[3].compare = period;
    TIMER0_context[3].handler = handler;
    
    // Apply new settings
    TA0CCR3 = TA0R + TIMER0_context[3].compare;
    TA0CCTL3 |= OUTMOD_4;                       // toggle mode
	TA0CCTL3 |= CCIE;                           // enable CCR3 interrupt
    TA0CCTL3 &= ~CCIFG;                         // clear CCR3 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_stop3()
// Description:
//! \brief      TIMER0 stop (TA0, CCR3)
//------------------------------------------------------------------------------
void TIMER0_stop3(void)
{
    // Clear handler in timer context
    TIMER0_context[2].handler = NULL;
    
    // Apply new settings
    TA0CCTL3 &= ~OUTMOD_7;                      // claer output mode
    TA0CCTL3 &= ~CCIE;                          // disable CCR3 interrupt
    TA0CCTL3 &= ~CCIFG;                         // clear CCR3 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_init4()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR4)
//------------------------------------------------------------------------------
void TIMER0_init4(uint16_t period, void (*handler)(void))
{
    // Save period and handler to timer context
    TIMER0_context[4].compare = period;
    TIMER0_context[4].handler = handler;
    
    // Apply new settings
    TA0CCR4 = TA0R + TIMER0_context[4].compare;
    TA0CCTL4 |= OUTMOD_4;                       // toggle mode
	TA0CCTL4 |= CCIE;                           // enable CCR4 interrupt
    TA0CCTL4 &= ~CCIFG;                         // clear CCR4 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_stop4()
// Description:
//! \brief      TIMER0 stop (TA0, CCR4)
//------------------------------------------------------------------------------
void TIMER0_stop4(void)
{
    // Clear handler in timer context
    TIMER0_context[4].handler = NULL;
    
    // Apply new settings
    TA0CCTL4 &= ~OUTMOD_7;                      // claer output mode
    TA0CCTL4 &= ~CCIE;                          // disable CCR4 interrupt
    TA0CCTL4 &= ~CCIFG;                         // clear CCR4 IFG
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
        
    #ifdef SysTick_Handler
        SysTick_Handler();
    #endif //SysTick_Handler
        
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
    if(TA0CCTL3 & CCIFG)
    {
        TA0CCTL3 &= ~CCIFG;                     // clear CCR IFG
        TA0CCR3 += TIMER0_context[3].compare;   // update CCR
    
        if(TIMER0_context[3].handler)
            TIMER0_context[3].handler();
    }
    if(TA0CCTL4 & CCIFG)
    {
        TA0CCTL4 &= ~CCIFG;                     // clear CCR IFG
        TA0CCR4 += TIMER0_context[4].compare;   // update CCR
    
        if(TIMER0_context[4].handler)
            TIMER0_context[4].handler();
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
#if (defined(TIMER1_ENABLED))

// Check if PWM or CAPTURE mode selected
//#if ( defined(PWM_EANBLED) || defined(CAPTURE_ENABLED) )
//    #error TIMER1: Unit is not avaliable if PWM or CAPTURE mode selected!
//#endif

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_init()
// Description:
//! \brief      TIMER1 initialization (TA1)
//------------------------------------------------------------------------------
void TIMER1_init(uint16_t period)
{
    TA1CTL  |= TACLR;                           // clear

#if (T1CLK_FREQ == ACLK_FREQ)
    TA1CTL  |= TASSEL__ACLK;                    // ACLK
    TA1CTL  |= ID_0;
#elif (T1CLK_FREQ == ACLK_FREQ/2)
    TA1CTL  |= TASSEL__ACLK;                    // ACLK/2
    TA1CTL  |= ID_1;
#elif (T1CLK_FREQ == ACLK_FREQ/4)
    TA1CTL  |= TASSEL__ACLK;                    // ACLK/4
    TA1CTL  |= ID_2;
#elif (T1CLK_FREQ == ACLK_FREQ/8)
    TA1CTL  |= TASSEL__ACLK;                    // ACLK/8
    TA1CTL  |= ID_3;
#elif (T1CLK_FREQ == SMCLK_FREQ)
    TA1CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA1CTL  |= ID_0;
#elif (T1CLK_FREQ == SMCLK_FREQ/2)
    TA1CTL  |= TASSEL_2;                        // SMCLK/2
    TA1CTL  |= ID_1;
#elif (T1CLK_FREQ == SMCLK_FREQ/4)
    TA1CTL  |= TASSEL__SMCLK;                   // SMCLK/4
    TA1CTL  |= ID_2;
#elif (T1CLK_FREQ == SMCLK_FREQ/8)
    TA1CTL  |= TASSEL__SMCLK;                   // SMCLK/8
    TA1CTL  |= ID_3;
#else
    #error TIMERS: Unsupported TIMER1 clock frequency
#endif

    #ifdef TIMER1_OvfHandler
    TA1CTL &= ~TAIFG;                           // clear OVF IFG
    TA1CTL |=  TAIE;                            // enable OVF interrupt
    #endif
    
    TA1CTL |= MC_1;                             // up mode
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

#ifdef TIMER2_EANBLED

//------------------------------------------------------------------------------
// Function:	
//              TIMER2_init()
// Description:
//! \brief      TIMER2 initialization (TB0)
//------------------------------------------------------------------------------
void TIMER2_init(uint16_t period)
{
    TB0CTL  |= TBCLR;                           // clear

#if (T2CLK_FREQ == ACLK_FREQ)
    TB0CTL  |= TASSEL__ACLK;                    // ACLK
    TB0CTL  |= ID_0;
#elif (T2CLK_FREQ == ACLK_FREQ/2)
    TB0CTL  |= TASSEL__ACLK;                    // ACLK/2
    TA1CTL  |= ID_1;
#elif (T2CLK_FREQ == ACLK_FREQ/4)
    TB0CTL  |= TASSEL__ACLK;                    // ACLK/4
    TB0CTL  |= ID_2;
#elif (T2CLK_FREQ == ACLK_FREQ/8)
    TB0CTL  |= TASSEL__ACLK;                    // ACLK/8
    TB0CTL  |= ID_3;
#elif (T2CLK_FREQ == SMCLK_FREQ)
    TB0CTL  |= TASSEL__SMCLK;                   // SMCLK
    TB0CTL  |= ID_0;
#elif (T2CLK_FREQ == SMCLK_FREQ/2)
    TB0CTL  |= TASSEL_2;                        // SMCLK/2
    TB0CTL  |= ID_1;
#elif (T2CLK_FREQ == SMCLK_FREQ/4)
    TB0CTL  |= TASSEL__SMCLK;                   // SMCLK/4
    TB0CTL  |= ID_2;
#elif (T2CLK_FREQ == SMCLK_FREQ/8)
    TB0CTL  |= TASSEL__SMCLK;                   // SMCLK/8
    TB0CTL  |= ID_3;
#else
    #error TIMERS: Unsupported TIMER2 clock frequency
#endif

    // TO DO: define select options
    TB0CCR0 = period;                           // set period
    TB0CCR1 = period/2;                         // set duty cycle
    TB0CCR2 = period/2 - 200;                   // set duty cycle
    TB0CCR3 = period/2 - 400;                   // set duty cycle
    //TB0CCR4 = 0;                              // set duty cycle
    //TB0CCR5 = 0;                              // set duty cycle
    //TB0CCR6 = 0;                              // set duty cycle
    
    #ifdef TIMER2_Handler
    TB0CCTL1 |= CCIE;                           // CCR1 interrupt enable
    TB0CCTL1 &= ~CCIFG;                         // clear CCR1 IFG
    #endif //TIMER2_Handler
    
    TB0CTL |= MC_1;                             // up mode
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER2_isr()
// Description:
//! \brief      TIMER2 interrupt service routine    (CCR1)
//------------------------------------------------------------------------------
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER2_isr(void)
{
    TB0CCTL1 &= ~CCIFG;                         // clear CCR1 IFG

    #ifdef TIMER2_Handler
        TIMER2_Handler();
    #endif //TIMER2_Handler
  
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

#endif // TIMER2

#if (defined(__MSP430F5329__))
//------------------------------------------------------------------------------
// Function:	
//              TIMER3_init()
// Description:
//! \brief      TIMER3 initialization (TA2)
//------------------------------------------------------------------------------
void TIMER3_init(uint16_t period)
{
    TA2CTL  |= TBCLR;                           // clear

#if (T3CLK_FREQ == ACLK_FREQ)
    TA2CTL  |= TASSEL__ACLK;                    // ACLK
    TA2CTL  |= ID_0;
#elif (T3CLK_FREQ == ACLK_FREQ/2)
    TA2CTL  |= TASSEL__ACLK;                    // ACLK/2
    TA2CTL  |= ID_1;
#elif (T3CLK_FREQ == ACLK_FREQ/4)
    TA2CTL  |= TASSEL__ACLK;                    // ACLK/4
    TA2CTL  |= ID_2;
#elif (T3CLK_FREQ == ACLK_FREQ/8)
    TA2CTL  |= TASSEL__ACLK;                    // ACLK/8
    TA2CTL  |= ID_3;
#elif (T3CLK_FREQ == SMCLK_FREQ)
    TA2CTL  |= TASSEL__SMCLK;                   // SMCLK
    TA2CTL  |= ID_0;
#elif (T3CLK_FREQ == SMCLK_FREQ/2)
    TA2CTL  |= TASSEL_2;                        // SMCLK/2
    TA2CTL  |= ID_1;
#elif (T3CLK_FREQ == SMCLK_FREQ/4)
    TA2CTL  |= TASSEL__SMCLK;                   // SMCLK/4
    TA2CTL  |= ID_2;
#elif (T3CLK_FREQ == SMCLK_FREQ/8)
    TA2CTL  |= TASSEL__SMCLK;                   // SMCLK/8
    TA2CTL  |= ID_3;
#else
    #error TIMERS: Unsupported TIMER3 clock frequency
#endif

    // TO DO... define select options
    TA2CCR0 = period;                           // set period
    TA2CCR1 = period/2;                         // set duty cycle 50%
    TA2CCR2 = period/2;                         // set duty cycle 50%
     
    TA2CTL |= MC_1;                             // up mode
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER3_isr()
// Description:
//! \brief      TIMER3 interrupt service routine    (CCR1)
//------------------------------------------------------------------------------
#pragma vector=TIMER2_A1_VECTOR
__interrupt void TIMER3_isr(void)
{
    TA2CCTL1 &= ~CCIFG;                         // clear CCR1 IFG

    #ifdef TIMER3_Handler
        TIMER3_Handler();
    #endif //TIMER2_Handler
  
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}
#endif

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


#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

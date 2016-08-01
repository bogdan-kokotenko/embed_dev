//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/cc254x/timers.c
//! \brief  CC254x Timers functions implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
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
#include "macro.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_CC254x_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              WDT_init()
// Description:
//! \brief      Watchdog timer initialization
//------------------------------------------------------------------------------
void WDT_init(void)
{
#if ( defined (WDT_RST) )
    
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
#if (defined (WDT_RST))    

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
    // Clear Timer 1 interrupt flags.
    T1STAT = ~(T1STAT_OVFIF+T1STAT_CH4IF+T1STAT_CH3IF+
               T1STAT_CH2IF+T1STAT_CH1IF+T1STAT_CH0IF);

    // Set individual interrupt enable bits in the peripherals SFRs.
    T1CCTL0 |= T1CCTLn_IM;      // Enable interrupt on channel 0.
    T1CCTL1 &= ~T1CCTLn_IM;     // Disable interrupt on channel 1.
    T1CCTL2 &= ~T1CCTLn_IM;     // Disable interrupt on channel 2.
    T1CCTL3 &= ~T1CCTLn_IM;     // Disable interrupt on channel 3.
    T1CCTL4 &= ~T1CCTLn_IM;     // Disable interrupt on channel 4.
    T1OVFIM = 0;                // Disable overflow interrupt.
    
    // Enable Timer 1 interrupts by setting [IEN1.T1IE=1].
    T1IE = 1;
    
    // Set compare register of channel 0
    T1CC0H = highByte(period);
    T1CC0L = lowByte(period);
    T1CCTL0 |= T1CCTLn_MODE;
    
    // Set prescalar divider value to 1
    T1CTL = (T1CTL & ~(T1CTL_DIV));
    // Set Timer 1 to modulo mode
    T1CTL = (T1CTL & ~(T1CTL_MODE)) | T1CTL_MODE_MODULO;

}

//------------------------------------------------------------------------------
// Function:	
//              TIMER1_isr()
// Description:
//! \brief      SysTick ISR
//------------------------------------------------------------------------------
#pragma vector = T1_VECTOR
__interrupt void TIMER1_isr(void)
{
    // Clear Timer 1 channel 0 interrupt flag.
    T1STAT = ~T1STAT_CH0IF;
 
    #ifdef SysTick_Handler0
        SysTick_Handler0();
    #endif
}

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
/*clock_t clock(void)
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
*/
#endif // _CC254x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

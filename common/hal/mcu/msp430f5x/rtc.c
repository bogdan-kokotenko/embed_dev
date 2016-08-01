//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/rtc.c
//! \brief  MSP430F5x RTC functions implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  08/10/2015 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "time.h"
#include "clocks.h"
#include "timers.h"
#include "rtc.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning RTC: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              RTC_init()
// Description:
//! \brief      RTC initialization
//------------------------------------------------------------------------------
void RTC_init(void)
{
#if ( defined(RTC_COUNTER)  &&  defined(RTC_CALENDAR) ) || \
    ( !defined(RTC_COUNTER)  &&  !defined(RTC_CALENDAR) )
    #error RTC: Wrong RTC mode selection
#endif    
    
#if ( defined(RTC_CALENDAR) )
    #error RTC: Calendare mode is not implemented!
#endif    
    
    // Prescale timer 0
    RTCPS0CTL |= RT0PSHOLD;                     // Prescale timer 0 hold
#if (RTC_CLK_FREQ == SMCLK_FREQ)
    RTCPS0CTL |= RT0SSEL;                       // Prescale_0_CLK <- SMCLK
#elif (RTC_CLK_FREQ == ACLK_FREQ)
    RTCPS0CTL &= ~RT0SSEL;                      // Prescale_0_CLK <- ACLK
#else
    #error Unsupported RTC clock frequency
#endif
    RTCPS0CTL |= RT0PSDIV_2;                    // RTC_CLK_FREQ/8
    RTCPS0CTL |= RT0IP_2;                       // RTC_CLK_FREQ/8 
    RTCPS0CTL &= ~RT0PSIFG;                     // Prescale 0 IFG clear
    //RTCPS0CTL |= RT0PSIE;                     // Prescale 0 INT enable

    // Prescale timer 1
    RTCPS1CTL |= RT1SSEL1;                      // Prescale_1_CLK <- Prescale_0
    RTCPS1CTL |= RT1PSDIV_2;                    // Prescale_1_CLK/8 
    RTCPS1CTL |= RT1IP_2;                       // Prescale_1_CLK/8 
    RTCPS1CTL &= ~RT1PSIFG;                     // Prescale 1 IFG clear
    //RTCPS1CTL |= RT1PSIE;                     // Prescale 1 INT enable

    // RTC
    RTCCTL01 |= RTCHOLD;                        // RTC hold
    RTCCTL01 |= RTCSSEL__RT1PS;                 // RTC_CLK <- Prescale_1
//#if (RTC_CLK_FREQ == SMCLK_FREQ)
//    RTCCTL01 |= RTCSSEL__SMCLK;                 // RTC <- SMCLK
//#elif (RTC_CLK_FREQ == ACLK_FREQ)
//    RTCCTL01 |= RTCSSEL__ACLK;                  // RTC <- ACLK
//#else
//    #error RTC: Unsupported RTC clock frequency
//#endif
    RTCCTL01 &= ~(RTCMODE+RTCBCD);              // counter mode, HEX
    RTCNT12 = 0x0000;                           // clear registers
    RTCNT34 = 0x0000;
    
    RTCCTL01 &= ~(RTCHOLD);                     // Start RTC

    RTCPS1CTL &= ~RT1PSHOLD;                  // Prescale timer 1 start
    RTCPS0CTL &= ~RT0PSHOLD;                    // Prescale timer 0 start
}

//------------------------------------------------------------------------------
// Function:	
//              RTC_reset()
// Description:
//!             RTC reset
//------------------------------------------------------------------------------
void RTC_reset(void)
{
    RTCCTL01 |= RTCHOLD;                        // RTC hold

    RTCNT12 = 0x0000;                           // clear registers
    RTCNT34 = 0x0000;
    
    RTCCTL01 &= ~(RTCHOLD);                     // Start RTC
}

//------------------------------------------------------------------------------
// Function:	
//              RTC_getTicks()
// Description:
//! \brief      RTC get ticks
//------------------------------------------------------------------------------
uint32_t RTC_getTicks(void)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);

    uint16_t counterH = RTCNT34;
    uint16_t counterL = RTCNT12;
    if(counterH > RTCNT34)  // check low part overflow
    {
        counterH = RTCNT34;
        counterL = RTCNT12;
    }
    
    LeaveCriticalSection(intState);   
    
    return (((uint32_t)counterH<<16) | counterL);
}

//------------------------------------------------------------------------------
// Function:	
//              RTC_isr()
// Description:
//! \brief      RTC interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=RTC_VECTOR
__interrupt void RTC_isr(void)
{
    switch(__even_in_range(RTCIV,14))
    {
        case  0: break;                         // Vector  0:  No interrupt
        case  2:                                // Vector  2:  RTCRDYIFG
            __no_operation();
            break;
        case  4: break;                         // Vector  4:  RTCEVIFG
        case  6: break;                         // Vector  6:  RTCAIFG
        case  8: break;                         // Vector  8:  RT0PSIFG
        case 10: break;                         // Vector 10:  RT1PSIFG
        case 12: break;                         // Vector 12:  RTCOFIFG
        case 14: break;                         // Vector 14:  Reserved
        default: break;
    }

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

#endif // _MSP430_HAL_

//******************************************************************************
// End of file
//******************************************************************************

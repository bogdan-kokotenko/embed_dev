//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/gpio.c
//! \brief  MSP430G2x GPIO access functions and ISRs
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------
//!  28/06/2013 | Bogdan Kokotenko | Initial draft
//!  15/06/2013 | Bogdan Kokotenko | Commnts improved
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else
//! Pin dirrection registers array
uint8_t* const PDIRARR[] =
{
    (uint8_t*) &P1DIR,
    (uint8_t*) &P2DIR,
    (uint8_t*) &P3DIR
};

//! Pin dirrection registers array
uint8_t* const POUTARR[] =
{
    (uint8_t*) &P1OUT,
    (uint8_t*) &P2OUT,
    (uint8_t*) &P3OUT
};

//! Pin dirrection registers array
uint8_t* const PINARR[] =
{
    (uint8_t*) &P1IN,
    (uint8_t*) &P2IN,
    (uint8_t*) &P3IN
};

//! Pin dirrection registers array
uint8_t* const PRENARR[] =
{
    (uint8_t*) &P1REN,
    (uint8_t*) &P2REN,
    (uint8_t*) &P3REN
};

//! Pin function selection registers array
uint8_t* const PSELARR[] =
{
    (uint8_t*) &P1SEL,
    (uint8_t*) &P2SEL,
    (uint8_t*) &P3SEL
};

//! Pin function selection registers array
uint8_t* const PSEL2ARR[] =
{
    (uint8_t*) &P1SEL2,
    (uint8_t*) &P2SEL2,
    (uint8_t*) &P3SEL2
};

//! Pin interrupt enable registers array
uint8_t* const PIEARR[] =
{
    (uint8_t*) &P1IE,
    (uint8_t*) &P2IE
};

//! Pin interrupt edge selection registers array
uint8_t* const PIESARR[] =
{
    (uint8_t*) &P1IES,
    (uint8_t*) &P2IES
};

//! Pin interrupt flag registers array
uint8_t* const PIFGARR[] =
{
    (uint8_t*) &P1IFG,
    (uint8_t*) &P2IFG
};

//! Enable check function parameters 
//! May be disabled in release version
#define GPIO_PARAM_CHECK    

// Maximal number of available GPIO for MSP430G2x
#define GPIO_MAX    3

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setOut()
// Description:
//! \brief      Set pin direction to input
//------------------------------------------------------------------------------
void GPIO_setIn(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
  
    *(PDIRARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setOut()
// Description:
//! \brief      Set pin direction to output
//------------------------------------------------------------------------------
void GPIO_setOut(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(PDIRARR[x-1]) |=  (1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setHigh()
// Description:
//! \brief      Set pin to '1'
//------------------------------------------------------------------------------
void GPIO_setHigh(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(POUTARR[x-1]) |=  (1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_setLow()
// Description:
//! \brief      Set pin to '0'
//------------------------------------------------------------------------------
void GPIO_setLow(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(POUTARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullUp()
// Description:
//! \brief      Pull-up pin 
//------------------------------------------------------------------------------
void GPIO_pullUp(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(PRENARR[x-1]) |= (1 << y);
    *(POUTARR[x-1]) |= (1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullDown()
// Description:
//! \brief      Pull-down pin
//------------------------------------------------------------------------------
void GPIO_pullDown(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(PRENARR[x-1]) |= (1 << y);
    *(POUTARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_pullDown()
// Description:
//! \brief      Disable pull resistor
//------------------------------------------------------------------------------
void GPIO_pullDisable(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
      
    *(PRENARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func0()
// Description:
//! \brief      Select pin primary function (as GPIO)
//------------------------------------------------------------------------------
void GPIO_func0(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif

    *(PSELARR[x-1]) &= ~(1 << y);
    *(PSEL2ARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func1()
// Description:
//! \brief      Select pin secondary function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func1(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
    
    *(PSELARR[x-1]) |= (1 << y);
    *(PSEL2ARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func2()
// Description:
//! \brief      Select pin third function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func2(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
    
    *(PSELARR[x-1]) &= ~(1 << y);
    *(PSEL2ARR[x-1]) |= (1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func3()
// Description:
//! \brief      Select pin fourth function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func3(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
    
    *(PSELARR[x-1]) |= (1 << y);
    *(PSEL2ARR[x-1]) |= (1 << y);
}
//------------------------------------------------------------------------------
// Function:	
//				GPIO_interruptEnable()
// Description:
//! \brief      Enable pin interrupt
//------------------------------------------------------------------------------
void GPIO_interruptEnable(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > 2 || y > 7)
        return;
    #endif

    // TO DO: add option for edge selection
    //*(PIESARR[x-1]) |= (1 << y);
    *(PIFGARR[x-1]) &= ~(1 << y);
    *(PIEARR[x-1]) |= (1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_interruptDisable()
// Description:
//! \brief      Disable pin interrupt
//------------------------------------------------------------------------------
void GPIO_interruptDisable(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > 2 || y > 7)
        return;
    #endif
      
    *(PIEARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_test()
// Description:
//! \brief      Get pin state
//------------------------------------------------------------------------------
bool GPIO_test(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return false;
    #endif
      
    return (*(PINARR[x-1]) & (1 << y))? true: false;
}

//------------------------------------------------------------------------------
// Function:	
//              GPIO_P1_isr()
// Description:
//!             PORT1 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=PORT1_VECTOR
__interrupt void GPIO_P1_isr(void)
{
/*  switch(__even_in_range(P1IV,16))
    {
        case  0: break;                         // Vector  0:  No interrupt
        case  2:                                // Vector  2:  BIT0
            #ifdef P1_0_Handler
                P1_0_Handler();
            #endif //P1_0_Handler
            P1IFG &= ~BIT0;
            break;
        case  4:                                // Vector  4:  BIT1
            #ifdef P1_1_Handler
                P1_1_Handler();
            #endif //P1_1_Handler
            P1IFG &= ~BIT1;
            break;
        case  6:                                // Vector  6:  BIT2
            #ifdef P1_2_Handler
                P1_2_Handler();
            #endif //P1_2_Handler
            P1IFG &= ~BIT2;
            break;                         
        case  8:                                // Vector  8:  BIT3
            #ifdef P1_3_Handler
                P1_3_Handler();
            #endif //P1_3_Handler
            P1IFG &= ~BIT3;
            break;
        case 10: break;                         // Vector 10:  BIT4
        case 12: break;                         // Vector 12:  BIT5
        case 14: break;                         // Vector 14:  BIT6
        case 16: break;                         // Vector 16:  BIT7
        default:
            break;
    }
*/
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              GPIO_P2_isr()
// Description:
//!             PORT2 interrupt service routine
//------------------------------------------------------------------------------
#pragma vector=PORT2_VECTOR
__interrupt void GPIO_P2_isr(void)
{
// Only one IFG is handled, IFG switch commented    
/*  switch(__even_in_range(P2IV,16))
    {
        case  0: break;                         // Vector  0:  No interrupt
        case  2:                                // Vector  2:  BIT0
            #ifdef P2_0_Handler
                P2_0_Handler();
            #endif //P2_0_Handler
            P2IFG &= ~BIT0;
            break;
        case  4:                                // Vector  4:  BIT1
            #ifdef P2_1_Handler
                P2_1_Handler();
            #endif //P2_1_Handler
            P2IFG &= ~BIT1;
            break;
        case  6:                                // Vector  6:  BIT2
*/          #ifdef P2_2_Handler
                P2_2_Handler();
            #endif //P2_2_Handler
            P2IFG &= ~BIT2;
/*          break;                         
        case  8:                                // Vector  8:  BIT3
            #ifdef P2_3_Handler
                P2_3_Handler();
            #endif //P2_3_Handler
            P2IFG &= ~BIT3;
            break;
        case 10: break;                         // Vector 10:  BIT4
        case 12: break;                         // Vector 12:  BIT5
        case 14: break;                         // Vector 14:  BIT6
        case 16: break;                         // Vector 16:  BIT7
        default:
            break;
    }
*/
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                          // Wake-up MCU
    #endif 
}

#endif  // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

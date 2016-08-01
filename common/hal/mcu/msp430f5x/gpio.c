//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/gpio.c
//! \brief  MSP430F5x GPIO handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------------
//!  21/07/2013 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning GPIO: Unknown MCU core, check HAL configuration!
#else

//! Pin dirrection registers array
uint8_t* const PDIRARR[] =
{
    (uint8_t*) &P1DIR,
    (uint8_t*) &P2DIR,
    (uint8_t*) &P3DIR,
    (uint8_t*) &P4DIR,
    (uint8_t*) &P5DIR,
    (uint8_t*) &P6DIR,
    (uint8_t*) &P7DIR
    #if ( defined (__MSP430F5329__) \
       || defined (__MSP430F5529__) )
    ,(uint8_t*) &P8DIR
    #elif ( defined (__MSP430F6638__) )
    ,(uint8_t*) &P8DIR,
    (uint8_t*) &P9DIR
    #elif ( defined (__MSP430F5438A__) )
    ,(uint8_t*) &P8DIR,
    (uint8_t*) &P9DIR,
    (uint8_t*) &P10DIR,
    (uint8_t*) &P11DIR
    #endif
};

//! Pin dirrection registers array
uint8_t* const POUTARR[] =
{
    (uint8_t*) &P1OUT,
    (uint8_t*) &P2OUT,
    (uint8_t*) &P3OUT,
    (uint8_t*) &P4OUT,
    (uint8_t*) &P5OUT,
    (uint8_t*) &P6OUT,
    (uint8_t*) &P7OUT
    #if ( defined (__MSP430F5329__) \
       || defined (__MSP430F5529__) )
    ,(uint8_t*) &P8OUT
    #elif ( defined (__MSP430F6638__) )
    ,(uint8_t*) &P8OUT,
    (uint8_t*) &P9OUT
    #elif ( defined (__MSP430F5438A__) )
    ,(uint8_t*) &P8OUT,
    (uint8_t*) &P9OUT,
    (uint8_t*) &P10OUT,
    (uint8_t*) &P11OUT
    #endif
};

//! Pin dirrection registers array
uint8_t* const PINARR[] =
{
    (uint8_t*) &P1IN,
    (uint8_t*) &P2IN,
    (uint8_t*) &P3IN,
    (uint8_t*) &P4IN,
    (uint8_t*) &P5IN,
    (uint8_t*) &P6IN,
    (uint8_t*) &P7IN
    #if ( defined (__MSP430F5329__) \
       || defined (__MSP430F5529__) )
    ,(uint8_t*) &P8IN
    #elif ( defined (__MSP430F6638__) )
    ,(uint8_t*) &P8IN,
    (uint8_t*) &P9IN
    #elif ( defined (__MSP430F5438A__) )
    ,(uint8_t*) &P8IN,
    (uint8_t*) &P9IN,
    (uint8_t*) &P10IN,
    (uint8_t*) &P11IN
    #endif
};

//! Pin dirrection registers array
uint8_t* const PRENARR[] =
{
    (uint8_t*) &P1REN,
    (uint8_t*) &P2REN,
    (uint8_t*) &P3REN,
    (uint8_t*) &P4REN,
    (uint8_t*) &P5REN,
    (uint8_t*) &P6REN,
    (uint8_t*) &P7REN
    #if ( defined (__MSP430F5329__) \
       || defined (__MSP430F5529__) )
    ,(uint8_t*) &P8REN
    #elif ( defined (__MSP430F6638__) )
    ,(uint8_t*) &P8REN,
    (uint8_t*) &P9REN
    #elif ( defined (__MSP430F5438A__) )
    ,(uint8_t*) &P8REN,
    (uint8_t*) &P9REN,
    (uint8_t*) &P10REN,
    (uint8_t*) &P11REN
    #endif
};

//! Pin function selection registers array
uint8_t* const PSELARR[] =
{
    (uint8_t*) &P1SEL,
    (uint8_t*) &P2SEL,
    (uint8_t*) &P3SEL,
    (uint8_t*) &P4SEL,
    (uint8_t*) &P5SEL,
    (uint8_t*) &P6SEL,
    (uint8_t*) &P7SEL
    #if ( defined (__MSP430F5329__) \
       || defined (__MSP430F5529__) )
    ,(uint8_t*) &P8SEL
    #elif ( defined (__MSP430F6638__) )
    ,(uint8_t*) &P8SEL,
    (uint8_t*) &P9SEL
    #elif ( defined (__MSP430F5438A__) )
    ,(uint8_t*) &P8SEL,
    (uint8_t*) &P9SEL,
    (uint8_t*) &P10SEL,
    (uint8_t*) &P11SEL
    #endif
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

// Maximal number of available GPIO
#if ( defined (__MSP430F6638) )
#define GPIO_MAX    11
#elif ( defined (__MSP430F5438A__) )
#define GPIO_MAX    9
#elif ( defined (__MSP430F5329__) \
     || defined (__MSP430F5529__) )
#define GPIO_MAX    8
#else
#define GPIO_MAX    7
#endif

//------------------------------------------------------------------------------
// Function:	
//				GPIO_init()
// Description:
//! \brief      Default state initialization
//------------------------------------------------------------------------------
void GPIO_init(void)
{
    //P1DIR = P1DIR_DEFAULT;
    //P1OUT = P1OUT_DEFAULT;
    //P2DIR = P2DIR_DEFAULT;
    //P2OUT = P2OUT_DEFAULT;
    //P3DIR = (P3DIR_DEFAULT)&0xFF;
    //P3OUT = (P3OUT_DEFAULT)&0xFF;
    //P4DIR = (P4DIR_DEFAULT)&0xFF;
    //P4OUT = (P4OUT_DEFAULT)&0xFF;
    //P5DIR = (P5DIR_DEFAULT)&0xFF;
    //P5OUT = (P5OUT_DEFAULT)&0xFF;
    //P6DIR = P6DIR_DEFAULT;
    //P6OUT = P6OUT_DEFAULT;
    //P7DIR = (P7DIR_DEFAULT)&0xFF;
    //P7OUT = (P7OUT_DEFAULT)&0xFF;
    
    //__delay_ms(4);
}

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
//				GPIO_func1()
// Description:
//! \brief      Select pin primary function (as GPIO)
//------------------------------------------------------------------------------
void GPIO_func1(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif

    *(PSELARR[x-1]) &= ~(1 << y);
}

//------------------------------------------------------------------------------
// Function:	
//				GPIO_func2()
// Description:
//! \brief      Select pin secondary function (depends on MCU peripherals)
//------------------------------------------------------------------------------
void GPIO_func2(uint8_t x, uint8_t y)
{
    #if defined(GPIO_PARAM_CHECK)
    if(x == 0 || x > GPIO_MAX || y > 7)
        return;
    #endif
    
    *(PSELARR[x-1]) |= (1 << y);
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
    *(PIFGARR[x-1]) &= (1 << y);
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
  // Only one IFG is handled, IFG switch commented    
/*  switch(__even_in_range(P1IV,16))
    {
        case  0: break;                         // Vector  0:  No interrupt
        case  2:                                // Vector  2:  BIT0
*/          #ifdef P1_0_Handler
                P1_0_Handler();
            #endif //P1_0_Handler
            P1IFG &= ~BIT0;
/*            break;
        case  4:                                // Vector  4:  BIT1
*/          #ifdef P1_1_Handler
                P1_1_Handler();
            #endif //P1_1_Handler
            P1IFG &= ~BIT1;
/*          break;
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
            #ifdef P2_2_Handler
                P2_2_Handler();
            #endif //P2_2_Handler
            P2IFG &= ~BIT2;
            break;                         
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

#endif  // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

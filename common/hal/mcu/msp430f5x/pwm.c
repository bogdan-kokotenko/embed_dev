//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/pwm.c
//! \brief  MSP430F5x PWM functions implementation
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
#if ( !defined (_MSP430F5x_HAL_) )
#warning PWM: Unknown MCU core, check HAL configuration!
#else

// Check if PWM is enabled and available
#if (defined(PWM0_ENABLED))

// Check if TIMER or CAPTURE mode selected
//#if ( defined(TIMER1_EANBLED) || defined(CAPTURE0_ENABLED) )
//    #error PWM: Unit is not avaliable if TIMER or CAPTURE mode selected!
//#endif

//------------------------------------------------------------------------------
// Function:	
//              PWM0_init()
// Description:
//! \brief      TIMER based PWM initialization
//------------------------------------------------------------------------------
void PWM0_init(void)
{
    // Initialize timer
    PMW0_REG(CTL) |= TACLR;                         // clear

#if (PWM_FREQ == SMCLK_FREQ)
    PMW0_REG(CTL)   |= TASSEL_2;                    // SMCLK
    PMW0_REG(CTL)   |= ID_0;
#elif (PWM_FREQ == SMCLK_FREQ/2)
    PMW0_REG(CTL)   |= TASSEL_2;                    // SMCLK/2
    PMW0_REG(CTL)   |= ID_1;
#elif (PWM_FREQ == SMCLK_FREQ/4)
    PMW0_REG(CTL)   |= TASSEL_2;                    // SMCLK/4
    PMW0_REG(CTL)   |= ID_2;
#elif (PWM_FREQ == SMCLK_FREQ/8)
    PMW0_REG(CTL)   |= TASSEL_2;                    // SMCLK/8
    PMW0_REG(CTL)   |= ID_3;
#elif (PWM_FREQ == ACLK_FREQ)
    PMW0_REG(CTL)   |= TASSEL_1;                    // ACLK
    PMW0_REG(CTL)   |= ID_0;
#elif (PWM_FREQ == ACLK_FREQ/2)
    PMW0_REG(CTL)   |= TASSEL_1;                    // ACLK/2
    PMW0_REG(CTL)   |= ID_1;
#elif (PWM_FREQ == ACLK_FREQ/4)
    PMW0_REG(CTL)   |= TASSEL_1;                    // ACLK/4
    PMW0_REG(CTL)   |= ID_2;
#elif (PWM_FREQ == ACLK_FREQ/8)
    PMW0_REG(CTL)   |= TASSEL_1;                    // ACLK/8
    PMW0_REG(CTL)   |= ID_3;
#else
    #error PWM: Unsupported TIMER1 clock frequency
#endif

#ifdef PWM0_0_OUT_PIN
    GPIO_func2(PWM0_0_OUT_PIN);                     // PWM0_0 -> output
    PMW0_REG(CCTL0) |= OUTMOD_4;                    // toggle mode
#endif

#ifdef PWM0_1_OUT_PIN
    GPIO_func2(PWM0_1_OUT_PIN);                     // PWM0_1 -> output
    PMW0_REG(CCTL1) |= PWM0_CCR1_MODE;
#endif
   
#ifdef PWM0_2_OUT_PIN
    GPIO_func2(PWM0_2_OUT_PIN);                     // PWM0_2 -> output
    PMW0_REG(CCTL2) |= PWM0_CCR2_MODE;
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_start()
// Description:
//! \brief      Start PWM generation (based on TIMER1)
//------------------------------------------------------------------------------
void PWM0_start(void)
{
#ifdef PWM0_0_OUT_PIN
    GPIO_setOut(PWM0_0_OUT_PIN);                    // enable output PWM0_0
#endif

#ifdef PWM0_1_OUT_PIN
    GPIO_setOut(PWM0_1_OUT_PIN);                    // enable output PWM0_1
#endif
   
#ifdef PWM0_2_OUT_PIN
    GPIO_setOut(PWM0_2_OUT_PIN);                    // enable output PWM0_2
#endif
       
    // Start PWM0
    PMW0_REG(CTL) |= MC_1;                          // up to CCR0
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_stop()
// Description:
//! \brief      Stop PWM generation (based on TIMER1)
//------------------------------------------------------------------------------
void PWM0_stop(void)
{
#ifdef PWM0_0_OUT_PIN
    GPIO_setIn(PWM0_0_OUT_PIN);                     // disable output PWM_0
#endif

#ifdef PWM0_1_OUT_PIN
    GPIO_setIn(PWM0_1_OUT_PIN);                     // disable output PWM0_1
#endif
   
#ifdef PWM0_2_OUT_PIN
    GPIO_setIn(PWM0_2_OUT_PIN);                     // disable output PWM0_2
#endif

    // Stop TIMER1
    TA1CTL &= ~MC_3;                                // stop
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_set()
// Description:
//! \brief      PWM set CCR0, CCR1, CCR2
//------------------------------------------------------------------------------
void PWM0_set(uint16_t period, uint16_t duty1, uint16_t duty2)
{
    PMW0_REG(CCR0) = period - 1;
    PMW0_REG(CCR1) = duty1;
    PMW0_REG(CCR2) = duty2;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM_isEnabled()
// Description:
//! \brief      Get output state of PWM
//------------------------------------------------------------------------------
bool PWM0_isEnabled(void)
{
    return (PMW0_REG(CTL) & MC_3)? true: false;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_period()
// Description:
//! \brief      Get period of PWM
//------------------------------------------------------------------------------
uint16_t PWM0_period(void)
{
    return (PMW0_REG(CCR0)+1);
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_duty1()
// Description:
//! \brief      Get duty cycle of PWM ch1
//------------------------------------------------------------------------------
uint16_t PWM0_duty1(void)
{
    return PMW0_REG(CCR1);
}

//------------------------------------------------------------------------------
// Function:	
//              PWM0_duty2()
// Description:
//! \brief      Get duty cycle of PWM ch2
//------------------------------------------------------------------------------
uint16_t PWM0_duty2(void)
{
    return PMW0_REG(CCR2);
}

#endif  // PWM0_ENABLED

// Check if PWM is enabled and available
#if (defined(PWM1_ENABLED))

// Check if TIMER or CAPTURE mode selected
//#if ( defined(TIMER2_EANBLED) || defined(CAPTURE1_ENABLED) )
//    #error PWM: Unit is not avaliable if TIMER or CAPTURE mode selected!
//#endif

//------------------------------------------------------------------------------
// Function:	
//              PWM1_init()
// Description:
//! \brief      TIMER based PWM initialization
//------------------------------------------------------------------------------
void PWM1_init(void)
{
    // Initialize timer
    PMW1_REG(CTL)  |= TACLR;                    // clear

#if (PWM1_FREQ == ACLK_FREQ)
    PMW1_REG(CTL)  |= TASSEL__ACLK;             // ACLK
    PMW1_REG(CTL)  |= ID_0;
#elif (PWM1_FREQ == ACLK_FREQ/2)
    PMW1_REG(CTL)  |= TASSEL__ACLK;             // ACLK/2
    PMW1_REG(CTL)  |= ID_1;
#elif (PWM1_FREQ == ACLK_FREQ/4)
    PMW1_REG(CTL)  |= TASSEL__ACLK;             // ACLK/4
    PMW1_REG(CTL)  |= ID_2;
#elif (PWM1_FREQ == ACLK_FREQ/8)
    PMW1_REG(CTL)  |= TASSEL__ACLK;             // ACLK/8
    PMW1_REG(CTL)  |= ID_3;
#elif (PWM1_FREQ == ACLK_FREQ/64)
    PMW1_REG(CTL)  |= TASSEL__ACLK;             // ACLK/64
    PMW1_REG(CTL)  |= ID_3;
    PMW1_REG(EX0)  = TAIDEX_7;                         
#elif (PWM1_FREQ == SMCLK_FREQ)
    PMW1_REG(CTL)  |= TASSEL__SMCLK;            // SMCLK
    PMW1_REG(CTL)  |= ID_0;
#elif (PWM1_FREQ == SMCLK_FREQ/2)
    PMW1_REG(CTL)  |= TASSEL__SMCLK;            // SMCLK/2
    PMW1_REG(CTL)  |= ID_1;
#elif (PWM1_FREQ == SMCLK_FREQ/4)
    PMW1_REG(CTL)  |= TASSEL__SMCLK;            // SMCLK/4
    PMW1_REG(CTL)  |= ID_2;
#elif (PWM1_FREQ == SMCLK_FREQ/8)
    PMW1_REG(CTL)  |= TASSEL__SMCLK;            // SMCLK/8
    PMW1_REG(CTL)  |= ID_3;
#elif (PWM1_FREQ == SMCLK_FREQ/64)
    PMW1_REG(CTL)  |= TASSEL__SMCLK;            // SMCLK/64
    PMW1_REG(CTL)  |= ID_3;
    PMW1_REG(EX0)  = TAIDEX_7;                          
#else
    #error PWM: Unsupported TIMER2 clock frequency
#endif

#ifdef PWM1_0_OUT_PIN
    GPIO_func2(PWM1_0_OUT_PIN);                 // PWM1_0 -> output
    PMW1_REG(CCTL0) |= OUTMOD_4;                // toggle mode
#endif

#ifdef PWM1_1_OUT_PIN
    GPIO_func2(PWM1_1_OUT_PIN);                 // PWM1_1 -> output
    PMW1_REG(CCTL1) |= PWM1_CCR1_MODE;
#endif
   
#ifdef PWM1_2_OUT_PIN
    GPIO_func2(PWM1_2_OUT_PIN);                 // PWM1_1 -> output
    PMW1_REG(CCTL2) |= PWM1_CCR2_MODE;
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_start()
// Description:
//! \brief      Start PWM generation (based on TIMER2)
//------------------------------------------------------------------------------
void PWM1_start(void)
{
#ifdef PWM1_0_OUT_PIN
    GPIO_setOut(PWM1_0_OUT_PIN);                // enable output PWM1_0
#endif

#ifdef PWM1_1_OUT_PIN
    GPIO_setOut(PWM1_1_OUT_PIN);                // enable output PWM1_1
#endif
   
#ifdef PWM1_2_OUT_PIN
    GPIO_setOut(TPWM1_2_OUT_PIN);               // enable output PWM1_2
#endif
       
    // Start PWM1
    PMW1_REG(CTL) |= MC_1;                      // up to CCR0
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_stop()
// Description:
//! \brief      Stop PWM1 generation (based on TB0)
//------------------------------------------------------------------------------
void PWM1_stop(void)
{
#ifdef PWM1_0_OUT_PIN
    GPIO_setIn(PWM1_0_OUT_PIN);                 // disable output PWM1_0
#endif

#ifdef PWM1_1_OUT_PIN
    GPIO_setIn(PWM1_1_OUT_PIN);                 // disable output PWM1_1
#endif
   
#ifdef PWM1_2_OUT_PIN
    GPIO_setIn(PWM1_2_OUT_PIN);                 // disable output PWM1_2
#endif

    // Stop PWM1
    PMW1_REG(CTL) &= ~MC_3;                     // stop
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_set()
// Description:
//! \brief      PWM1 set CCR0, CCR1, CCR2
//------------------------------------------------------------------------------
void PWM1_set(uint16_t period, uint16_t duty1, uint16_t duty2)
{
    PMW1_REG(CCR0) = period - 1;
    PMW1_REG(CCR1) = duty1;
    PMW1_REG(CCR2) = duty2;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_isEnabled()
// Description:
//! \brief      Get output state of PWM1
//------------------------------------------------------------------------------
bool PWM1_isEnabled(void)
{
    return (PMW1_REG(CTL) & MC_3)? true: false;
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_period()
// Description:
//! \brief      Get period of PWM1
//------------------------------------------------------------------------------
uint16_t PWM1_period(void)
{
    return (PMW1_REG(CCR0)+1);
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_duty1()
// Description:
//! \brief      Get duty cycle of PWM ch1
//------------------------------------------------------------------------------
uint16_t PWM1_duty1(void)
{
    return PMW1_REG(CCR1);
}

//------------------------------------------------------------------------------
// Function:	
//              PWM1_duty2()
// Description:
//! \brief      Get duty cycle of PWM ch2
//------------------------------------------------------------------------------
uint16_t PWM1_duty2(void)
{
    return PMW1_REG(CCR2);
}

#endif  // PWM1_ENABLED

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

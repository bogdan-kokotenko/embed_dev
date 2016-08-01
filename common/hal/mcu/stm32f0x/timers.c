//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//	File description:
//! \file       hal/mcu/stm32f0x/timers.c
//! \brief      STM32F0x timers initialization and ISRs
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
#include "devtime.h"
#include "timers.h"
#include "task.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_STM32F0X_HAL_) )
#warning TIMERS: Unknown MCU core, check HAL configuration!
#else

//! IWDT refresh mask
#define IWDG_REFRESH      (uint32_t)(0x0000AAAA)
//! IWDT write access mask
#define IWDG_WRITE_ACCESS (uint32_t)(0x00005555)
//! IWDT start mask
#define IWDG_START        (uint32_t)(0x0000CCCC)
//! IWDT reload value
#define IWDG_RELOAD       ((LSI_FREQ/64)*WDT_RST/1000)

//------------------------------------------------------------------------------
// Function:	
//              WDT_init()
// Description:
//! \brief      Watchdog timer initialization
//------------------------------------------------------------------------------
void WDT_init(void)
{
#if ( defined (WDT_RST) && defined (NDEBUG))
    // Clear WDT and prepare for reset mode
    RCC->CSR |= RCC_CSR_LSION;
    while((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY);

    IWDG->KR = IWDG_START;
    IWDG->KR = IWDG_WRITE_ACCESS;
    IWDG->PR = (IWDG_PR_PR_2);                      // 40 kHz/64
    
#if (IWDG_RELOAD > 0x0FFF)
    #error TIMERS: Wrong WDT reload value exceeds maximum!
#endif
    IWDG->RLR = IWDG_RELOAD;
    while(IWDG->SR);
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
    IWDG->KR = IWDG_REFRESH;
}

//------------------------------------------------------------------------------
// Function:	
//				SysTick_init()
// Description:
//! \brief      SysTick timer initialization
//------------------------------------------------------------------------------
void SysTick_init(uint32_t tickInterval)
{
    SysTick->LOAD = tickInterval; 
    SysTick->VAL = 0; 
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

//------------------------------------------------------------------------------
// Function:	
//				SysTick_Handler()
// Description:
//! \brief      SysTick timer handler
//------------------------------------------------------------------------------
void SysTick_Handler(void)
{
#ifdef Systick_OverflowHandler
    Systick_OverflowHandler();
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_init()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR0)
//------------------------------------------------------------------------------
void TIMER0_init(void)
{
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_SetPriority(TIM1_CC_IRQn,0);
    
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->PSC = 1000;
    
    TIM1->ARR = 0xFFFF;
    TIM1->CCR1 = 0x1000;
    TIM1->SR &= ~TIM_SR_CC1IF;
    TIM1->DIER |= TIM_DIER_CC1IE;
    //TIM1->CR1 |= TIM_CR1_CEN;
}

//------------------------------------------------------------------------------
void TIM1_CC_IRQHandler(void)
{
    TIM1->SR &= ~(TIM_SR_CC1OF | TIM_SR_CC1IF);
    __NOP();
    
    //LED_toggle();
}

//------------------------------------------------------------------------------
// Function:	
//              TIMER0_init1()
// Description:
//! \brief      TIMER0 initialization (TA0, CCR0)
//------------------------------------------------------------------------------
void TIMER0_init1(void)
{

}

#endif // _STM32F0X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/stm32f0x/clocks.c
//! \brief  STM32F0x clock sources initialization
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_STM32F0X_HAL_) )
#warning CLOCKS: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              FLASH_init()
// Description:
//! \brief      Initialize flash prefetch unit
//------------------------------------------------------------------------------
void FLASH_init(void)
{
#if (SYSCLK_FREQ < 24000000)
    FLASH->ACR = FLASH_ACR_PRFTBE;
#else
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;  // set 1 cycle latency
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              CLK_init()
// Description:
//! \brief      Initialize system clocks
//------------------------------------------------------------------------------
void CLK_init(void)
{
    // Initialize flash prefetch unit
    FLASH_init();
    
    // Disable PLL
    RCC->CR &= ~RCC_CR_PLLON;

    // Enable external clock if available
#if defined(HSE_FREQ)
    RCC->CR |= RCC_CR_HSEON;
    while(!RCC->CR&RCC_CR_HSERDY);
#endif
    
    // Select PPL source frequency
#if defined(PLL_SRC)
    #if defined(HSE_FREQ)
        RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
        #if (PLL_SRC == (HSE_FREQ))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV1;
        #elif (PLL_SRC == (HSE_FREQ/2))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV2;
        #elif (PLL_SRC == (HSE_FREQ/3))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV3;
        #elif (PLL_SRC == (HSE_FREQ/4))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV4;
        #elif (PLL_SRC == (HSE_FREQ/5))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV5;
        #elif (PLL_SRC == (HSE_FREQ/6))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV6;
        #elif (PLL_SRC == (HSE_FREQ/7))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV7;
        #elif (PLL_SRC == (HSE_FREQ/8))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV8;
        #elif (PLL_SRC == (HSE_FREQ/9))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV9;
        #elif (PLL_SRC == (HSE_FREQ/10))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV10;
        #elif (PLL_SRC == (HSE_FREQ/11))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV11;
        #elif (PLL_SRC == (HSE_FREQ/12))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV12;
        #elif (PLL_SRC == (HSE_FREQ/13))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV13;
        #elif (PLL_SRC == (HSE_FREQ/14))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV14;
        #elif (PLL_SRC == (HSE_FREQ/15))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV15;
        #elif (PLL_SRC == (HSE_FREQ/16))
            RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV16;
        #else
            #pragma error CLOCKS: Wrong PPL input frequency!
        #endif
    #else
        RCC->CFGR &= ~RCC_CFGR_PLLSRC_HSI_DIV2;
    #endif
#endif

    // Select PLL output frequency
#if defined(PLL_FREQ)    
    #if (PLL_FREQ == (PLL_SRC*2))
        RCC->CFGR |= RCC_CFGR_PLLMUL2;
    #elif (PLL_FREQ == (PLL_SRC*3))
        RCC->CFGR |= RCC_CFGR_PLLMUL3;
    #elif (PLL_FREQ == (PLL_SRC*4))
        RCC->CFGR |= RCC_CFGR_PLLMUL4;
    #elif (PLL_FREQ == (PLL_SRC*5))
        RCC->CFGR |= RCC_CFGR_PLLMUL5;
    #elif (PLL_FREQ == (PLL_SRC*6))
        RCC->CFGR |= RCC_CFGR_PLLMUL6;
    #elif (PLL_FREQ == (PLL_SRC*7))
        RCC->CFGR |= RCC_CFGR_PLLMUL7;
    #elif (PLL_FREQ == (PLL_SRC*8))
        RCC->CFGR |= RCC_CFGR_PLLMUL8;
    #elif (PLL_FREQ == (PLL_SRC*9))
        RCC->CFGR |= RCC_CFGR_PLLMUL9;
    #elif (PLL_FREQ == (PLL_SRC*10))
        RCC->CFGR |= RCC_CFGR_PLLMUL10;
    #elif (PLL_FREQ == (PLL_SRC*11))
        RCC->CFGR |= RCC_CFGR_PLLMUL11;
    #elif (PLL_FREQ == (PLL_SRC*12))
        RCC->CFGR |= RCC_CFGR_PLLMUL12;
    #elif (PLL_FREQ == (PLL_SRC*13))
        RCC->CFGR |= RCC_CFGR_PLLMUL13;
    #elif (PLL_FREQ == (PLL_SRC*14))
        RCC->CFGR |= RCC_CFGR_PLLMUL14;
    #elif (PLL_FREQ == (PLL_SRC*15))
        RCC->CFGR |= RCC_CFGR_PLLMUL15;
    #elif (PLL_FREQ == (PLL_SRC*16))
        RCC->CFGR |= RCC_CFGR_PLLMUL16;
    #else
        #error CLOCKS: Wrong PLL output frequency!
    #endif
    RCC->CR |= RCC_CR_PLLON;    
    while(!RCC->CR&RCC_CR_PLLRDY);
#endif

    //Select SYSCLK source
#if (SYSCLK_FREQ == PLL_FREQ)
    RCC->CFGR |= RCC_CFGR_SW_PLL;
#elif (SYSCLK_FREQ == HSE_FREQ)
    RCC->CFGR |= RCC_CFGR_SW_HSE;
#elif (SYSCLK_FREQ == HSI_FREQ)
    RCC->CFGR |= RCC_CFGR_SW_HSI;
#endif

    // Select CPU/bus clock
#if (HCLK_FREQ == SYSCLK_FREQ)
    
#else
    #pragma error CLOCKS: Wrong CPU clock frequency!
#endif

    // Select peripheral clock
#if (PCLK1_FREQ == HCLK_FREQ)
    
#else
    #pragma error CLOCKS: Wrong peripheral clock frequency!
#endif
    // Enable clock security system
    RCC->CR |= RCC_CR_CSSON;
}

//------------------------------------------------------------------------------
// Function:	
//              NMI_isr()
// Description:
//! \brief      Nonmaskable interrupts ISR
//------------------------------------------------------------------------------
void NMI_Handler(void)
{
    if(RCC->CIR & RCC_CIR_CSSF)
    {
        RCC->CIR |= RCC_CIR_CSSC;
        CLK_init();
    }
}

#endif // _STM32F0X_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

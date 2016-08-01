//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//	File description:
//! \file       hal/mcu/stm32f0x/adc.c
//! \brief      STM32F0x ADC initialization and ISR
//!     
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  23/05/2015 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "analog.h"
#include "timers.h"
#include "adc.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_STM32F0X_HAL_) )
#warning ADC: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              ADC_init()
// Description:
//! \brief      ADC initialization
//------------------------------------------------------------------------------
void ADC_init(void)
{
    // Configure GPIO for ADC
#if (ADC_CHANNEL == ADC_IN0)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;      // Enable the clock of GPIOA
    GPIOA->MODER |= GPIO_MODER_MODER0;      // Select analog mode for PA1
#elif (ADC_CHANNEL == ADC_IN1)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;      // Enable the clock of GPIOA
    GPIOA->MODER |= GPIO_MODER_MODER1;      // Select analog mode for PA1
#endif
    
    // Enable the ADC clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;     // Enable the ADC clock
    RCC->CR2 |= RCC_CR2_HSI14ON;            // Start HSI14 RC oscillator
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY)); // Wait HSI14 is ready
    
    // Performs a self-calibration of the ADC
    if ((ADC1->CR & ADC_CR_ADEN))           // Ensure that ADEN = 0
        ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);// Clear ADEN
    ADC1->CR |= ADC_CR_ADCAL;               // Launch the calibration
    while (ADC1->CR & ADC_CR_ADCAL);        // Wait until ADCAL=0
        
    // Enable ADC
    do{
        ADC1->CR |= ADC_CR_ADEN;            // Enable the ADC
    }while (!(ADC1->ISR & ADC_ISR_ADRDY));  // Wait until ADC ready
        
    // Configure ADC
    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;   // Select HSI14 by writing 00 in CKMODE
    ADC1->CFGR1 |= (ADC_CFGR1_EXTEN_0+ADC_CFGR1_EXTSEL_1+ADC_CFGR1_EXTSEL_0); // trigger on TIM3_TRGO
#ifdef ADC_BIPOLAR 
    ADC1->CFGR1 |= ADC_CFGR1_ALIGN;     // Result is left aligned
#endif
#if (ADC_CHANNEL == ADC_IN0)
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;   // Select CHSEL0 for PA0
#elif (ADC_CHANNEL == ADC_IN1)
    ADC1->CHSELR = ADC_CHSELR_CHSEL1;   // Select CHSEL1 for PA1
#elif (ADC_CHANNEL == ADC_IN16)
    ADC1->CHSELR = ADC_CHSELR_CHSEL16;  // Select CHSEL16 for TEMP sensor
#elif (ADC_CHANNEL == ADC_IN17)
    ADC1->CHSELR = ADC_CHSELR_CHSEL17;  // Select CHSEL17 for VREFINT
    ADC->CCR |= ADC_CCR_VREFEN;         // Wake-up the VREFINT 
#else
    #error ADC: Wrong cnhannel selection!
#endif
    ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; // Select a sampling mode

    // Configure TIM15
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;// Enable the peripheral clock of the TIM3
    TIM3->CR2 |= TIM_CR2_MMS_1;        // Configure MMS=010 to output a rising edge
    TIM3->PSC = 479;                   // Select PCLK/480 i.e. 48MHz/480=100kHz 
    TIM3->ARR = (uint16_t)100;          // Set one update event each msec
    TIM3->CR1 |= TIM_CR1_CEN;          // Enable TIM3

    // Configure IT
    ADC1->IER |= (ADC_IER_EOCIE+ADC_IER_OVRIE);

    NVIC_SetPriority(ADC1_COMP_IRQn, 0);     // Set priority for ADC1_IRQn
    NVIC_EnableIRQ(ADC1_COMP_IRQn);          // Enable ADC1_IRQn
    
    // Start the ADC conversion
    ADC1->CR |= ADC_CR_ADSTART; 
}

//------------------------------------------------------------------------------
// Function:	
//              ADC1_IRQHandler()
// Description:
//!             ADC interrupt service routine
//------------------------------------------------------------------------------
void ADC1_COMP_IRQHandler(void)
{
    if((ADC1->ISR & ADC_ISR_EOC)==ADC_ISR_EOC)
    {
        #ifdef ADC_Handler
            #ifdef ADC_BIPOLAR
                ADC_Handler((int16_t)ADC1->DR - 0x8000);
            #else
                ADC_Handler((uint16_t)ADC1->DR);
            #endif
        #else
            ADC1->ISR |= ADC_ISR_EOC;
        #endif
    }
    else
    if((ADC1->ISR & ADC_ISR_OVR)==ADC_ISR_OVR)
    {
       ADC1->ISR |=    ADC_ISR_OVR; 
    }
    else
    {
        ADC1->ISR |=    ADC_ISR_AWD+ADC_ISR_OVR+ADC_ISR_EOSEQ+
                        ADC_ISR_EOSMP+ADC_ISR_ADRDY;
    }
}

#endif // _STM32F0X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

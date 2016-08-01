//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/stm32f0x/uart.c
//! \brief  STM32F0x UART functions implementation.			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  22/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "slip.h"
#include "uart.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_STM32F0X_HAL_) )
#warning UART: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//				UART0_init()
// Description:
//! \brief      Initialize UART0
//------------------------------------------------------------------------------
void UART_init(void)
{
    // Initialize TX/RX pins
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable the peripheral clock of GPIOA

    // Select AF mode (10) on PA2 and PA3
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
    // AF1 for USART1 signals
    //GPIOA->AFR[0] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRL2 | GPIO_AFRH_AFRL3));
    GPIOA->AFR[0] |=  (1 << (2*4))+ (1 << (3*4));
    
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;// Enable the peripheral clock USART1
    USART1->BRR = (PCLK1_FREQ/UART_BAUDRATE);  // set baudrate
    //USART1->CR2 = (USART_CR2_MSBFIRST+USART_CR2_CLKEN+USART_CR2_LBCL);
    USART1->CR1 = (USART_CR1_TE+USART_CR1_RXNEIE+USART_CR1_RE+USART_CR1_UE);

    // Polling idle frame Transmission w/o clock
    while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);

    USART1->ICR |= USART_ICR_TCCF;      // Clear TC flag
    USART1->CR1 |= USART_CR1_TCIE;      // Enable TC interrupt
    
    // Configure IT
    NVIC_SetPriority(USART1_IRQn, 0);   // Set priority for USART1_IRQn
    NVIC_EnableIRQ(USART1_IRQn);        // Enable USART1_IRQn
}

//------------------------------------------------------------------------------
// Function:	
//              UART0_startTx()
// Description:
//! \brief      Force interrupt and start transmission 
//------------------------------------------------------------------------------
void UART_startTx(void)	
{
    //USART1->ISR |= USART_ISR_TC;
    uint8_t txByte = 0x00;
    #ifdef UART_txHandler
    if(UART_txHandler(&txByte))
        USART1->TDR = txByte;
    #endif
}                  
                  
//------------------------------------------------------------------------------
// Function:	
//              USART1_IRQHandler()
// Description:
//! \brief      UART0 Receive/Transmit ISR
//------------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
    if(USART1->ISR & USART_ISR_TC)
    {
        uint8_t txByte = 0x00;
        USART1->ICR |= USART_ICR_TCCF;  // Clear transfer complete flag
        #ifdef UART_txHandler
        if(UART_txHandler(&txByte))
            USART1->TDR = txByte;
        #endif
    }
    else
    if(USART1->ISR & USART_ISR_RXNE)
    {
        uint8_t rxByte = (uint8_t)(USART1->RDR);
        #ifdef UART_rxHandler
            UART_rxHandler(rxByte);
        #endif
    }
    else
    {
        USART1->ICR |= (USART_ICR_PECF+     // Clear Parity Error Flag
                        USART_ICR_FECF+     // Clear Framing Error Flag
                        USART_ICR_NCF+      // Clear Noise detected Flag
                        USART_ICR_ORECF+    // Clear OverRun Error Flag
                        USART_ICR_IDLECF+   // Clear IDLE line detected Flag
                        USART_ICR_CTSCF+    // Clear CTS Interrupt Flag
                        USART_ICR_RTOCF+    // Clear Receiver Time Out Flag
                        USART_ICR_CMCF);    // Clear Character Match Flag
    }
}
            
#endif // _STM32F0X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

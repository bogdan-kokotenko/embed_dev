//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_uart  MSP430G2x UART APIs
//! \brief MSP430G2x UART APIs
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/msp430g2x/uart.h
//! \brief  MSP430G2x UART API			
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  13/10/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "uart_config.h"

//! Initialize UART
void UART_init(void);

//! Start UART transmition 
void UART_startTx(void);	    // for asynchronous mode	

//! Send character via UART
void UART_putchar(byte_t ch);  // for synchronous mode
//! Receive character via UART
uint8_t UART_getchar(void);    // for synchronous mode

//! UART Transmit ISR
void UART_TXISR_handler(void);

//! UART0 Receive ISR
void UART_RXISR_handler(void);

#ifdef __cplusplus
}
#endif

#endif // UART_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

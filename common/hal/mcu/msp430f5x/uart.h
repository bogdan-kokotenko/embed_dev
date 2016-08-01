//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_uart  MSP430F5x UART APIs
//! \brief MSP430F5x UART APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/uart.h
//! \brief  MSP430F5x UART API			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------------
//!  16/08/2014 | Bogdan Kokotenko | Initial draft
//!  16/04/2015 | Bogdan Kokotenko | Added RS845 control macros
//!  02/12/2015 | Bogdan Kokotenko | Added advanced UART configuration
//
//******************************************************************************
#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "uart_config.h"

//! UART baudrate options    
typedef enum _UART_BAUDRATE_t{
    UART_9600 = 0,
    UART_115200 = 1,
    UART_921600 = 2
}UART_BAUDRATE_t;

//------------------------------------------------------------------------------
// Check if UART0 enabled
#ifdef UART0_ENABLED
    
//! Initialize UART0
void UART0_init(UART_BAUDRATE_t baud);

//! Reset UART0 buffers
void UART0_reset(void);

//! Start UART0 transmition 
void UART0_startTx(void);	// for asynchronous mode	

//! Send character via UART0
void UART0_putchar(uint8_t ch);		
//! Receive character via UART0
uint8_t UART0_getchar(void);	// for synchronous mode

//! Send packet via UART0
void UART0_send(const void* packet, uint16_t size, void (*handler)(void));

//! Receive packet via UART0
void UART0_receive(void* packet, uint16_t size, void (*handler)(bool));

//------------------------------------------------------------------------------
// UART0 callbacks 

#define UART0_startTimeoutTimer(Handler, Timeout)   \
                            TASK_SCHEDULE_create(Handler, Timeout)

#define UART0_stopTimeoutTimer(Handler)             \
                            TASK_SCHEDULE_delete(Handler)

#endif // UART0_ENABLED

//------------------------------------------------------------------------------
// Check if UART1 enabled
#ifdef UART1_ENABLED
                                //! Initialize UART1
void UART1_init(UART_BAUDRATE_t baud);

//! Start UART1 transmition 
void UART1_startTx(void);	    // for asynchronous mode	

#endif // UART1_ENABLED
                                
#ifdef __cplusplus
}
#endif

#endif // UART_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

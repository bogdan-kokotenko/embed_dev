//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup stm32f0x_hal
//! @{
//! \defgroup stm32f0x_uart     STM32F0x UART APIs
//! \brief STM32F0x UART APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/stm32f0x/uart.h
//! \brief      STM32F0x UART API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "uart_config.h"

//! Initialize UART0
void UART_init(void);

//! Start UART0 transmition 
void UART_startTx(void);	    // for asynchronous mode	

#ifdef __cplusplus
}
#endif

#endif // UART_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

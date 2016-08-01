//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_uart
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/msp430g2x/usci_isr.h
//! \brief  MSP430G2x USCI shared ISRs handling.					
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  13/02/2016 | Bogdan Kokotenko | Initial draft
//!  11/05/2016 | Bogdan Kokotenko | Added I2C mode chenking for handlers
//
//******************************************************************************
#ifndef USCI_ISR_H
#define USCI_ISR_H

//! USCI RX ISR
#define USCIA0_RX_ISR()         UART_RXISR_handler()

//! USCI TX ISR
#define USCIA0_TX_ISR()         UART_TXISR_handler()

// Check if I2C mode slected
#if ( defined(I2C_MASTER) || defined(I2C_SLAVE) )

//! USCI RX ISR
#define USCIB0_RX_ISR()         I2C_rxHandler()

//! USCI TX ISR
#define USCIB0_TX_ISR()         I2C_txHandler()

//! I2C NACK ISR
#define USCIB0_I2C_NACK_ISR()   I2C_nackHandler()

#ifdef I2C_SLAVE
//! I2C STOP ISR
#define USCIB0_I2C_STOP_ISR()   I2C_stopHandler()
#endif

#endif

#endif // USCI_ISR_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

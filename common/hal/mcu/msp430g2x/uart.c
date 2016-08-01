//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/uart.c
//! \brief  MSP430G2x UART functions implementation.			
//!
//!                     Copyright (C) 2015 Bogdan Kokotenko
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  13/10/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "uart.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
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
    SelectPinFunc3(1,1);
    SelectPinFunc3(1,2);
    
    // Put state machine in reset
    UCA0CTL1 |= UCSWRST;

    // Baudrate initialization
    #if(ACLK_FREQ == 32768 && UART_BAUDRATE == 9600)
        UCA0CTL1 |= UCSSEL_1;       // BRCLK = ACLK
        UCA0BR0 = 0x03;             // 32768Hz/9600 = 3.41
        UCA0BR1 = 0x00;             //    
        UCA0MCTL = UCBRS_3;         // Modulation UCBRSx = 3
    #elif(ACLK_FREQ == 4000000 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_1;       // BRCLK = ACLK
        UCA0BR0 = 0x22;             // 4MHz/115200 = 34.72
        UCA0BR1 = 0x00;             //
        UCA0MCTL = 0;               // 
    #elif (ACLK_FREQ == 8000000 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_1;       // BRCLK = ACLK
        UCA0BR0 = 0x45;             // 8MHz/115200 = 69.444
        UCA0BR1 = 0x00;             //
        UCA0MCTL = 0;               // 
    #elif (SMCLK_FREQ == 1048576 && UART_BAUDRATE == 9600)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x68;             // 1.048MHz/9600 = 109.23
        UCA0BR1 = 0x00;             // 
        UCA0MCTL = UCBRS_1;         // Modulation UCBRSx = 1
    #elif (SMCLK_FREQ == 8000000 && UART_BAUDRATE == 9600)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x41;             // 8MHz/9600 = 833.33
        UCA0BR1 = 0x03;             //
        UCA0MCTL = UCBRS_2;         // Modulation UCBRSx = 2
    #elif (SMCLK_FREQ == 16000000 && UART_BAUDRATE == 9600)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x82;             // 16MHz/9600 = 138,889
        UCA0BR1 = 0x06;             //
        UCA0MCTL = UCBRS_5;         // Modulation UCBRSx = 5
    #elif (SMCLK_FREQ == 1048576 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x09;             // 1.048Hz/115200 = 9.102
        UCA0BR1 = 0x00;             //
        UCA0MCTL = UCBRS_0;         // Modulation UCBRSx = 0
    #elif (SMCLK_FREQ == 8000000 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x45;             // 8MHz/115200 = 69,444
        UCA0BR1 = 0x00;             //
        UCA0MCTL = UCBRS_3;         // Modulation UCBRSx = 3
    #elif (SMCLK_FREQ == 12000000 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x68;             // 12MHz/115200 = 104,167
        UCA0BR1 = 0x00;             //
        UCA0MCTL = UCBRS_1;         // Modulation UCBRSx = 1
    #elif (SMCLK_FREQ == 16000000 && UART_BAUDRATE == 115200)
        UCA0CTL1 |= UCSSEL_2;       // BRCLK = SMCLK
        UCA0BR0 = 0x8A;             // 16MHz/115200 = 138,889
        UCA0BR1 = 0x00;             //
        UCA0MCTL = UCBRS_7;         // Modulation UCBRSx = 7
    #else
        #error UART: Unsupported baudrate
    #endif
        
	UCA0CTL1 &= ~UCSWRST;               // Initialize USCI state machine
    
    IFG2 &= ~(UCA0RXIFG | UCA0TXIFG);	// Clear interrupt flags
    IE2 |= UCA0RXIE;                    // Enable USCI_A0 RX interrupt
    IE2 |= UCA0TXIE;                  	// Enable USCI_A0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              UART_putchar()
// Description:
//! \brief      Send character via UART     
//------------------------------------------------------------------------------
void UART_putchar(byte_t ch)
{
    while (!(IFG2&UCA0TXIFG));          // UART0 TX buffer ready?
    UCA0TXBUF = ch;                     // ch -> TX
}

//------------------------------------------------------------------------------
// Function:	
//              UART_getchar()
// Description:
//! \brief      Receive character via UART     
//------------------------------------------------------------------------------
uint8_t UART_getchar(void)
{
    while (!(IFG2&UCA0RXIFG));          // UART0 TX buffer ready?
    return UCA0RXBUF;                   // <- RX
}

//------------------------------------------------------------------------------
// Function:	
//              UART0_startTx()
// Description:
//! \brief      Force interrupt and start transmission 
//------------------------------------------------------------------------------
void UART_startTx(void)	
{
    IFG2 &= ~(UCA0TXIFG);               // Clear interrupt flags
    IFG2 |= UCA0TXIFG;                  // Set interrupt flags
}                  
                  
//------------------------------------------------------------------------------
// Function:	
//              UART0TXISR_handler()
// Description:
//! \brief      UART0 Transmit ISR
//------------------------------------------------------------------------------
void UART_TXISR_handler(void)
{
    uint8_t txByte = 0x00;
#	ifdef UART_txHandler
    if(UART_txHandler(&txByte))
        UCA0TXBUF = txByte;
    else
#	endif
        IFG2 &= ~(UCA0TXIFG);             // Clear interrupt flag
}

//------------------------------------------------------------------------------
// Function:	
//              UART0RXISR_handler()
// Description:
//! \brief      UART0 Receive ISR
//------------------------------------------------------------------------------
void UART_RXISR_handler(void)
{
    //IFG2 &= ~(UCA0RXIFG);             // Clear interrupt flags
    uint8_t rxByte = UCA0RXBUF;
#	ifdef UART_rxHandler
    UART_rxHandler(rxByte);
#	endif
}
              
#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/usci_isr.c
//! \brief  MSP430G2x USCI shared ISRs handling.			
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------
//!  13/10/2015 | Bogdan Kokotenko | Initial draft
//!  26/02/2016 | Bogdan Kokotenko | Improved IFG selection logic
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "slip.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "usci_isr.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning UART: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              USCI_isr0()
// Description:
//! \brief      USCI ISR for USCIA0_RX, USCIB0_I2C_STATE
//------------------------------------------------------------------------------
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI_isr0(void)
{
    #ifdef USCIA0_RX_ISR
    if(IFG2&UCA0RXIFG && IE2&UCA0RXIE)      // check UART0 RX IFG    
        USCIA0_RX_ISR();
    #endif

    #ifdef USCIB0_I2C_NACK_ISR              // check I2C NACK IFG   
    if(I2C_REG(STAT) & UCNACKIFG && I2C_REG(I2CIE) & UCNACKIE)   
        USCIB0_I2C_NACK_ISR();
    #endif
    
    #ifdef USCIB0_I2C_STOP_ISR              // check I2C STOP IFG   
    if(I2C_REG(STAT) & UCSTPIFG && I2C_REG(I2CIE) & UCSTPIE)    
        USCIB0_I2C_STOP_ISR();
    #endif

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                      // wake-up MCU
    #endif 
}    
    
//------------------------------------------------------------------------------
// Function:	
//              USCI_isr1()
// Description:
//! \brief      USCI ISR for ISR USCIA0_TX, USCIB0_I2C_DATA
//------------------------------------------------------------------------------
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI_isr1(void)
{
    // Workaround for erratum USCI29, USCI30
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    
    // IFG2 direct access causes unpredictable behavior
    uint8_t ifgTmp = IFG2;

    #ifdef USCIA0_TX_ISR
    if(ifgTmp&UCA0TXIFG && IE2&UCA0TXIE)    // check UART0 RX IFG    
        USCIA0_TX_ISR();
    #endif
    
    #ifdef USCIB0_RX_ISR
    if(ifgTmp&UCB0RXIFG && IE2&UCB0RXIE)    // check SPI/I2C RX IFG
        USCIB0_RX_ISR();
    #endif

    #ifdef USCIB0_TX_ISR
    if(ifgTmp&UCB0TXIFG && IE2&UCB0TXIE)    // check SPI/I2C TX IFG
        USCIB0_TX_ISR();
    #endif

    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                      // wake-up MCU
    #endif 
}

//------------------------------------------------------------------------------
// Function:	
//              TRAPINT_isr()
// Description:
//! \brief      Workaround for erratum USCI29
//------------------------------------------------------------------------------
#ifndef TRAPINT_VECTOR
#define TRAPINT_VECTOR (0* 2u)
#endif
#pragma vector= TRAPINT_VECTOR
__interrupt void TRAPINT_isr(void)
{
    __no_operation();
}

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

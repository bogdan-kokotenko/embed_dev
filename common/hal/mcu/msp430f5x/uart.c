//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/uart.c
//! \brief  MSP430F5x UART functions implementation.			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  16/08/2014 | Bogdan Kokotenko | Initial draft
//!  16/04/2015 | Bogdan Kokotenko | Added asynchronous mode using DMA0
//!  18/04/2015 | Bogdan Kokotenko | Added option for MSP430F5438A
//!  26/04/2015 | Bogdan Kokotenko | Fixed issue with RS485 RX/TX switching 
//!  02/10/2015 | Bogdan Kokotenko | Added framing error detection
//!  05/10/2015 | Bogdan Kokotenko | Added RXIFG checking if DMA hang off
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "uart.h"
#include "dma.h"
#include "task.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning CLOCK: Unknown MCU core, check HAL configuration!
#else

//! UART baudrate configuration structure
const struct UART_BaudrateConfig_t{
    uint8_t ctl1Mask;
    uint8_t br0;
    uint8_t br1;
    uint8_t mctl;
}UART_BaudrateConfig[] = {
    #if(SMCLK_FREQ == 1000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x68,               // 1MHz/9600 = 104.166
        .br1 =      0x00,               // 
        .mctl =     UCBRS_1             // Modulation UCBRSx = 1
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0x08,               // 1MHz/115200 = 8.68
        .br1 =      0x00,               //
        .mctl =     UCBRS_5             // Modulation UCBRSx = 5
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x01,               // 1MHz/115200 = 1.085
        .br1 =      0x00,               //
        .mctl =     UCBRS_1             // Modulation UCBRSx = 1  
    }
    #elif (SMCLK_FREQ == 4000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0xA0,               // 4MHz/9600 = 416.66 
        .br1 =      0x01,               // 
        .mctl =     UCBRS_5             // Modulation UCBRSx = 5   
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0x22,               // 4MHz/115200 = 34.72
        .br1 =      0x00,               //
        .mctl =     UCBRS_6             // Modulation UCBRSx = 6
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK;// BRCLK = SMCLK
        .br0 =      0x04,               // 4MHz/921600 = 4.34
        .br1 =      0x00,               //
        .mctl =     UCBRS_3             // Modulation UCBRSx = 3 
    }
    #elif (SMCLK_FREQ == 10000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK 
        .br0 =      0x11,               // 10MHz/9600 = 1041,66 
        .br1 =      0x04,               //  
        .mctl =     UCBRS_5,            // Modulation UCBRSx = 5 
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0x56,               // 10MHz/9600 = 86,806
        .br1 =      0x00,               //
        .mctl =     UCBRS_6             // Modulation UCBRSx = 6
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x0A,               // 10MHz/921600 = 10.851
        .br1 =      0x00,               //
        .mctl =     UCBRS_3             // Modulation UCBRSx = 7  
    }    
    #elif (SMCLK_FREQ == 12000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0xE2,               // 12MHz/9600 = 1250
        .br1 =      0x04,               // 
        .mctl  =    UCBRS_0             // Modulation UCBRSx = 0    
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0x68,               // 12MHz/115200 = 104,167
        .br1 =      0x00,               //
        .mctl =     UCBRS_1             // Modulation UCBRSx = 1
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK 
        .br0 =      0x0D,               // 12MHz/921600 = 13
        .br1 =      0x00,               // 
        .mctl =     UCBRS_0             // Modulation UCBRSx = 0    
    }
    #elif (SMCLK_FREQ == 16000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x82,               // 16MHz/9600 = 1666.66
        .br1 =      0x06,               // 
        .mctl  =    UCBRS_5             // Modulation UCBRSx = 5    
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0x8A,               // 16MHz/115200 = 138,888
        .br1 =      0x00,               //
        .mctl =     UCBRS_7             // Modulation UCBRSx = 7
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK 
        .br0 =      0x11,               // 16MHz/921600 = 17,361
        .br1 =      0x00,               // 
        .mctl =     UCBRS_3             // Modulation UCBRSx = 3    
    }
    #elif (SMCLK_FREQ == 20000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x23,               // 20MHz/9600 = 2083,33
        .br1 =      0x08,               // 
        .mctl  =    UCBRS_2             // Modulation UCBRSx = 2    
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0xAD,               // 20MHz/115200 = 173,611
        .br1 =      0x00,               //
        .mctl =     UCBRS_5             // Modulation UCBRSx = 5
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK 
        .br0 =      0x15,               // 20MHz/921600 = 21,701
        .br1 =      0x00,               // 
        .mctl =     UCBRS_6             // Modulation UCBRSx = 6
    }
    #elif (SMCLK_FREQ == 24000000)
    [UART_9600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0xC4,               // 24MHz/9600 = 2500 
        .br1 =      0x09,               //
        .mctl =     UCBRS_0             // Modulation UCBRSx = 0 
    }
    ,[UART_115200] = {
        .ctl1Mask =  UCSSEL__SMCLK,     // BRCLK = SMCLK
        .br0 =      0xD0,               // 24MHz/115200 = 208,333
        .br1 =      0x00,               //
        .mctl =     UCBRS_3             // Modulation UCBRSx = 3
    }
    ,[UART_921600] = {
        .ctl1Mask = UCSSEL__SMCLK,      // BRCLK = SMCLK
        .br0 =      0x1A,               // 24MHz/921600 = 26,042 
        .br1 =      0x00,               //  
        .mctl =     UCBRS_0             // Modulation UCBRSx = 0 
    }
    #else
        #error UART: Unsupported clock for UART
    #endif
};

// Check if UART0 enabled
#ifdef UART0_ENABLED

//! Initialize TX pin
#define UART0_TX_init()         GPIO_func2(UART0_TX_PIN)
//! Initialize RX pin
#define UART0_RX_init()         GPIO_func2(UART0_RX_PIN)

//! RS485 RDE switch initialization
#define UART0_RS485_init()      GPIO_setOut(UART0_RS485_RDE_PIN)
//! RS485 receive mode switch
#define UART0_RS485_receive()   GPIO_setLow(UART0_RS485_RDE_PIN)
//! RS485 transmit mode switch
#define UART0_RS485_transmit()  GPIO_setHigh(UART0_RS485_RDE_PIN)

//! UART0 last RX frame size
static uint16_t UART0_rxFrameSize = 0; 

//! UART0 RX framing error flag
static bool UART0_rxFramingErrorFlag = false; 

//! UART0 frame received handler
static void (*UART0_frameReceivedHandler)(bool);

//! Handler for frame complete
static void UART0_frameComplete(void);

//! Handler for framing error
void UART0_checkFramingError(void);

//------------------------------------------------------------------------------
// Function:	
//				UART0_init()
// Description:
//! \brief      Initialize UART0
//------------------------------------------------------------------------------
void UART0_init(UART_BAUDRATE_t baud)
{
    // Initialize TX/RX pins
    UART0_TX_init();
    UART0_RX_init();

    // Put state machine in reset
    UART0_REG(CTL1) |= UCSWRST;

    // Baudrate initialization
    UART0_REG(CTL1) |= UART_BaudrateConfig[baud].ctl1Mask;
    UART0_REG(BR0) = UART_BaudrateConfig[baud].br0;
    UART0_REG(BR1) = UART_BaudrateConfig[baud].br1;
    UART0_REG(MCTL) = UART_BaudrateConfig[baud].mctl;
            
    // Initialize GPIO for RS485 RDE control
    #ifdef UART0_RS485_RDE_PIN
        UART0_RS485_init();
    #endif // UART0_RS485_RDE_PIN
    
    // Switch RS485 to receive mode
    #ifdef UART0_RS485_RDE_PIN
        UART0_RS485_receive();
    #endif // UART0_RS485_RDE_PIN

	UART0_REG(CTL1) &= ~UCSWRST;        // Initialize USCI state machine
    
    //UART0_REG(IFG) &= ~(UCRXIFG);     // Clear RX interrupt flags
    //UART0_REG(IE) |= UCRXIE;          // Enable UART0 RX interrupt
    //UART0_REG(IFG) &= ~(UCTXIFG);     // Clear TX interrupt flags
    //UART0_REG(IE) |= UCTXIE;          // Enable UART0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              UART0_putchar()
// Description:
//! \brief      Send character via UART1     
//------------------------------------------------------------------------------
void UART0_putchar(byte_t ch)
{
    while (!(UART0_REG(IFG)&UCTXIFG));  // UART0 TX buffer ready?
    UART0_REG(TXBUF) = ch;              // ch -> TX
}

//------------------------------------------------------------------------------
// Function:	
//              UART1_getchar()
// Description:
//! \brief      Receive character via UART1     
//------------------------------------------------------------------------------
byte_t UART0_getchar(void)
{
    while (!(UART0_REG(IFG)&UCRXIFG));  // UART0 TX buffer ready?
    return UART0_REG(RXBUF);            // <- RX
}

//------------------------------------------------------------------------------
// Function:	
//              UART0_startTx()
// Description:
//! \brief      Force interrupt and start transmission 
//------------------------------------------------------------------------------
void UART0_startTx(void)	
{
    UART0_REG(IFG) &= ~UCTXIFG;         // clear UART1 IFG
    UART0_REG(IFG) |= UCTXIFG;          // set UART1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              UART0_reset()
// Description:
//! \brief      Reset UART0 buffers
//------------------------------------------------------------------------------
void UART0_reset(void)
{
    UART0_stopTimeoutTimer(&UART0_checkFramingError);
    UART0_resetBuffer();
}
                  
//------------------------------------------------------------------------------
// Function:	
//              UART0_send()
// Description:
//! \brief      Send packet via UART0
//------------------------------------------------------------------------------
void UART0_send(const void* packet, uint16_t size, void (*handler)(void))
{
    #ifdef UART0_RS485_RDE_PIN
        UART0_RS485_transmit();
    #endif // UART0_RS485_RDE_PIN
                
    // Prepare UART0 for transmiting new packet
    UART0_reset();
    UART0_dmaWrite(packet, size, handler);
    UART0_startTx();
}

//------------------------------------------------------------------------------
// Function:	
//             UART0_receive()
// Description:
//! \brief     Receive packet via UART1
//------------------------------------------------------------------------------
void UART0_receive(void* packet, uint16_t size, void (*handler)(bool))
{
    // wait till transmission complete
    //while (!(UART0_REG(IFG)&UCTXIFG));    // UART0 TX buffer ready?
    __delay_us((8*1000000L)/UART0_BAUDRATE);
    
    #ifdef UART0_RS485_RDE_PIN
        UART0_RS485_receive();
    #endif // UART0_RS485_RDE_PIN

    // Save size and handler in case of framing error   
    UART0_rxFrameSize = size;
    UART0_frameReceivedHandler = handler;

    UART0_REG(IFG) &= ~UCRXIFG;                 // clear UART0 IFG
    
    // Prepare UART0 for receiving new packet
    UART0_reset();
    UART0_dmaRead(packet, size, UART0_frameComplete);
    
    UART0_rxFramingErrorFlag = false;
    
    // Proper timeout depends on baudrate and packet size
    // For baudrate = 921600, size = 0..128 => timeout = ~1.5 ms OK
    // For baudrate = 921600, size = 0..512 => timeout = ~7 ms OK
    if(UART0_rxFrameSize < 128)
        UART0_startTimeoutTimer(&UART0_checkFramingError, 2);
    else
        UART0_startTimeoutTimer(&UART0_checkFramingError, 7);
}
                  
//------------------------------------------------------------------------------
// Function:	
//             UART0_checkFramingError()
// Description:
//! \brief     Handler for framing error check
//------------------------------------------------------------------------------
void UART0_checkFramingError(void)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);

    if(UART0_rxFramingErrorFlag)
    {
        UART0_frameReceivedHandler(false);
    }
    else
    {
        // Check UART0 buffer
        if(UART0_isRxBufferChanged(UART0_rxFrameSize) ||
           UART0_REG(IFG)&UCRXIFG)      // check if DMA hang off (unknown issue)
        {
            // next timeout is framing error
            UART0_rxFramingErrorFlag = true;
        }

        // Proper timeout depends on baudrate and packet size
        // For baudrate = 921600, size = 0..128 => timeout = ~1.5 ms OK
        // For baudrate = 921600, size = 0..512 => timeout = ~7 ms OK
        if(UART0_rxFrameSize < 128)
            UART0_startTimeoutTimer(&UART0_checkFramingError, 2);
        else
            UART0_startTimeoutTimer(&UART0_checkFramingError, 7);
    }
    
    LeaveCriticalSection(intState);
}
                  
//------------------------------------------------------------------------------
// Function:	
//             UART0_frameComplete()
// Description:
//! \brief     Handler for frame complete
//------------------------------------------------------------------------------
void UART0_frameComplete(void)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);
    
    UART0_rxFramingErrorFlag = false;
    UART0_rxFrameSize = 0;
    
    UART0_stopTimeoutTimer(&UART0_checkFramingError);
    if(UART0_frameReceivedHandler)
    {
        UART0_frameReceivedHandler(true);
        UART0_frameReceivedHandler = NULL;
    }
    
    LeaveCriticalSection(intState);
}

#endif // UART0_ENABLED

//------------------------------------------------------------------------------
// Check if UART1 enabled
#ifdef UART1_ENABLED

//! Initialize TX pin
#define UART1_TX_init()         GPIO_func2(UART1_TX_PIN)
//! Initialize RX pin
#define UART1_RX_init()         GPIO_func2(UART1_RX_PIN)

//------------------------------------------------------------------------------
// Function:	
//				UART1_init()
// Description:
//! \brief      Initialize UART1
//------------------------------------------------------------------------------
void UART1_init(UART_BAUDRATE_t baud)
{
    // Initialize TX/RX pins
    UART1_TX_init();
    UART1_RX_init();

    // Put state machine in reset
    UART1_REG(CTL1) |= UCSWRST;

    // Baudrate initialization
    UART1_REG(CTL1) |= UART_BaudrateConfig[baud].ctl1Mask;
    UART1_REG(BR0) = UART_BaudrateConfig[baud].br0;
    UART1_REG(BR1) = UART_BaudrateConfig[baud].br1;
    UART1_REG(MCTL) = UART_BaudrateConfig[baud].mctl;
        
	UART1_REG(CTL1) &= ~UCSWRST;        // Initialize USCI state machine
    
    UART1_REG(IFG) &= ~(UCRXIFG);     // Clear RX interrupt flags
    UART1_REG(IE) |= UCRXIE;          // Enable UART0 RX interrupt
    UART1_REG(IFG) &= ~(UCTXIFG);     // Clear TX interrupt flags
    UART1_REG(IE) |= UCTXIE;          // Enable UART0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              UART1_startTx()
// Description:
//! \brief      Force interrupt and start transmission 
//------------------------------------------------------------------------------
void UART1_startTx(void)	
{
    UART1_REG(IFG) &= ~UCTXIFG;         // clear UART1 IFG
    UART1_REG(IFG) |= UCTXIFG;          // set UART1 IFG
}

//------------------------------------------------------------------------------
// Function:	
//              UART1_isr()
// Description:
//! \brief      UART1 Transmit/Receive ISR
//------------------------------------------------------------------------------
#pragma vector=UART1_ISR_VECTOR
__interrupt void UART1_isr(void)
{
    switch(__even_in_range(UART1_REG(IV),4))
    {
        case 0:                             // Vector 0 - no interrupt
            break;                          
        case 2:                             // Vector 2 - RXIFG
        {
            uint8_t rxByte = UART1_REG(RXBUF);
            #ifdef UART1_rxHandler
                UART1_rxHandler(rxByte);
            #endif
            break;
        }
        case 4:                             // Vector 4 - TXIFG
        {
            uint8_t txByte = 0x00;
            #ifdef UART1_txHandler
            if(UART1_txHandler(&txByte))
                UART1_REG(TXBUF) = txByte;
            else
            #endif
                UART1_REG(IFG) &= ~UCTXIFG; // clear UART1 TX IFG
            break;                          
        }
        default:
            break;
    }
}

#endif // UART1_ENABLED

#endif // _MSP430_HAL_

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/spi.c
//! \brief  MSP430F5x SPI functions implementation.			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  30/01/2015 | Bogdan Kokotenko | Initial draft
//!   1/03/2015 | Bogdan Kokotenko | Decreased SPI speed for higher reliability
//!  27/12/2015 | Bogdan Kokotenko | Removed obsolete functions.
//!  28/12/2015 | Bogdan Kokotenko | Improved SPI configuraion for async mode.
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "timers.h"
#include "clocks.h"
#include "spi.h"
#include "dma.h"
#include "task.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning SPI: Unknown MCU core, check HAL configuration!
#else

#ifdef SPI0_ASYNCHRONOUS

//! SPI0 operation complete handler 
static void (*SPI0_handler)(void);

//! SPI0 operation complete handler wraper
static void SPI0_operationComplete(void);
#endif // SPI0_ASYNCHRONOUS


//! SPI0 number of chip selects
#define SPI0_NUMBER_OF_CS   (sizeof(SPI0_cs)/sizeof(struct SPI_csSelection_t))
//! SPI1 number of chip selects
//#define SPI1_NUMBER_OF_CS   (sizeof(SPI1_cs)/sizeof(struct SPI_csSelection_t))

// Check selected SPI0 mode
#if ( defined(SPI0_MASTER) && defined(SPI0_SLAVE))
    #error SPI: Select proper SPI0 mode!
#elif ( defined(SPI0_MASTER) || defined(SPI0_SLAVE))

//------------------------------------------------------------------------------
// Function:	
//              SPI0_init()
// Description:
//! \brief      Initialize SPI0
//------------------------------------------------------------------------------
void SPI0_init(void)
{
    // Check selected SPI0 mode
    #if ( defined(SPI0_MASTER) && defined(SPI0_SLAVE))
        #error SPI: Select proper SPI0 mode!
    #endif

    // Initialize SS for slave mode
    #ifdef SPI0_SLAVE
        GPIO_func2(SPI0_STE_PIN);
    #endif
    
    // Initialize SCLK, MISO, MOSI  
    GPIO_func2(SPI0_SCLK_PIN);
    GPIO_func2(SPI0_SIMO_PIN);
    GPIO_func2(SPI0_SOMI_PIN);

	SPI0_REG(CTL1) |= UCSWRST;              // **Put state machine in reset**

    SPI0_REG(CTL0) = UCCKPL+UCSYNC+UCMSB;   // 8-bit SPI, MSB
    #ifdef SPI0_SLAVE
        SPI0_REG(CTL0) |= UCMODE_2;         // SPI slave, SS: active low 
    #endif
    #ifdef SPI0_MASTER
        SPI0_REG(CTL0) |= UCMST;            // SPI master
            
        #if (SPI0_CLK == SMCLK_FREQ)
            SPI0_REG(CTL1) |= UCSSEL__SMCLK;// SPI0_CLK = SMCLK
        #elif (SPI0_CLK == ACLK_FREQ)
            SPI0_REG(CTL1) |= UCSSEL__ACLK; // SPI0_CLK = ACLK
        #else
            #error SPI: Unsupported SPI0 clock frequency
        #endif

        #if ( (SPI0_BAUDRATE) < 1000 || (SPI0_BAUDRATE) > 8000000L)
            #error SPI: Unsupported SPI0 baudrate
        #else
            SPI0_REG(BR0) = (((SPI0_CLK/SPI0_BAUDRATE) - 1)&0xFF);
            SPI0_REG(BR1) = (((SPI0_CLK/SPI0_BAUDRATE) - 1)>>8)&0xFF;
        #endif
    #endif

	SPI0_REG(CTL1) &= ~UCSWRST;             // **Initialize USCI state machine**

    //SPI0_REG(IE) |= UCRXIE;               // Enable USCI_A0 RX interrupt
    //SPI0_REG(IE) |= UCTXIE;               // Enable USCI_A0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              SPI0_isBusy()
// Description:
//! \brief      Check if SPI0 is ready for data exchange
//------------------------------------------------------------------------------
bool SPI0_isBusy(void)
{
    // check if transmission complete
	return (!(SPI0_REG(IFG) & UCTXIFG));
}

//------------------------------------------------------------------------------
// Function:	
//              SPI0_waitMISO()
// Description:
//! \brief      Wait till MISO pin goes down
//------------------------------------------------------------------------------
bool SPI0_waitMISO(void)
{
    uint8_t timeout = 100;
    
    // check MOSI pin state
    while(GPIO_test(SPI0_SOMI_PIN))
    {
        if(--timeout == 0)
            return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//                      SPI0_reset()
// Description:
//! \brief              Reset SPI0 transmission
//------------------------------------------------------------------------------
void SPI0_reset(void)
{
    //uint8_t tmp = UCA0RXBUF;    // clear tx buffer
    SPI0_REG(TXBUF) = SPI0_REG(TXBUF);// clear tx buffer
}

//------------------------------------------------------------------------------
// Function:	
//                      SPI0_txByte()
// Description:
//! \brief              Transmit byte via SPI0
//------------------------------------------------------------------------------
void SPI0_txByte(uint8_t dataByte)
{
	while (!(SPI0_REG(IFG) & UCTXIFG));     // SPI0 TX buffer ready?
    SPI0_REG(TXBUF) = dataByte;             // write data byte
}

//------------------------------------------------------------------------------
// Function:	
//                    SPI0_rxByte()
// Description:
//! \brief            Receive byte via SPI1 (UCA0)
//------------------------------------------------------------------------------
uint8_t SPI0_rxByte(void)
{
	while (!(SPI0_REG(IFG & UCTXIFG)));     // TX buffer ready?
    SPI0_REG(IFG) &= ~(UCRXIFG);            // clear RX flags
	SPI0_REG(TXBUF) = 0x00;                 // write dummy data

	while (SPI0_REG(STAT) & UCBUSY);		// TX buffer ready?    
	while (!(SPI0_REG(IFG) & UCRXIFG));     // RX buffer ready?

    return SPI0_REG(RXBUF);                 // read data byte
}

//------------------------------------------------------------------------------
// Function:	
//                    SPI0_exchByte()
// Description:
//! \brief            Exchange data via SPI0
//!
//! \return           received byte
//! \sa SPI0_txByte(), SPI0_rxByte(), SPI0_receive(), SPI0_transmit()
//------------------------------------------------------------------------------
uint8_t SPI0_exchByte(uint8_t txByte)
{
    BYTE rxByte;
  
    while (!(SPI0_REG(IFG & UCTXIFG)));     // TX buffer ready?
    SPI0_REG(IFG) &= ~(UCRXIFG);            // clear RX flags
	SPI0_REG(TXBUF) = txByte;               // write data

	while (SPI0_REG(STAT) & UCBUSY);		// TX buffer ready?       
    while (!(SPI0_REG(IFG) & UCRXIFG));     // RX buffer ready?
    rxByte = SPI0_REG(RXBUF);               // read data byte

    return rxByte;
}

//------------------------------------------------------------------------------
// Function:	
//              SPI0_startTx()
// Description:
//! \brief      Force interrupt and start transmission 
//------------------------------------------------------------------------------
void SPI0_startTx(void)	
{
    SPI0_REG(IFG) &= ~(UCTXIFG+UCRXIFG);    // clear IFG
    SPI0_REG(IFG) |= UCTXIFG;               // set IFG
}

//------------------------------------------------------------------------------
// Function:	
//              SPI0_exchange()
// Description:
//! \brief      Transmit/Receive packet (same size) via SPI0
//------------------------------------------------------------------------------
void SPI0_exchange(void* rxPacket, const void* txPacket,
                    uint16_t size,  void (*handler)(void))
{
#if ( defined(SPI0_dmaWrite) && defined(SPI0_dmaRead) )
    SPI0_dmaRead(rxPacket, size, handler);
    SPI0_dmaWrite(txPacket, size, NULL);
    SPI0_startTx();
#else
    // Use ISR
    // TO DO...
#endif
}

//------------------------------------------------------------------------------
// Function:	
//                  SPI0_transmit()
// Description:
//! \brief          Transmit byte array via SPI0 (UCA0)
//------------------------------------------------------------------------------
#ifdef SPI0_ASYNCHRONOUS
void SPI0_transmit(const void* data, uint16_t size, void (*handler)(void))
{
#ifdef SPI0_dmaWrite
    SPI0_handler = handler;
    SPI0_dmaWrite(data, size, SPI0_operationComplete);
    SPI0_startTx();
#else
    // Use ISR
    // TO DO...
#endif
}
#else
void SPI0_transmit(const void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(SPI0_REG(IFG) & UCTXIFG));       // TX buffer ready?
        SPI0_REG(TXBUF) = ((const uint8_t*)data)[index]; // send data
    }
}
#endif // SPI0_ASYNCHRONOUS

//------------------------------------------------------------------------------
// Function:	
//                  SPI0_receive()
// Description:
//! \brief          Receive byte array via SPI0
//------------------------------------------------------------------------------
#ifdef SPI0_ASYNCHRONOUS
void SPI0_receive(void* data, uint16_t size, void (*handler)(void))
{
#if ( defined(SPI0_dmaWrite) && defined(SPI0_dmaRead) )
    SPI0_handler = handler;
    SPI0_dmaRead(data, size, SPI0_operationComplete);
    SPI0_dmaWrite(data, size, NULL);
    SPI0_startTx();
#else
    // Use ISR
    // TO DO...
#endif
}
#else
void SPI0_receive(void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(SPI0_REG(IFG) & UCTXIFG));	// TX buffer ready?
        SPI0_REG(IFG) &= ~(UCRXIFG);        // clear RX flags
        SPI0_REG(TXBUF) = 0x00;             // write dummy data

        while (SPI0_REG(STAT) & UCBUSY);    // TX buffer ready?    
        while (!(SPI0_REG(IFG) & UCRXIFG));	// RX buffer ready?
        ((uint8_t*)data)[index] = SPI0_REG(RXBUF); // read data byte
    }
}
#endif // SPI0_ASYNCHRONOUS

#ifdef SPI0_ASYNCHRONOUS
//------------------------------------------------------------------------------
// Function:	
//              SPI0_operationComplete()
// Description:
//! \brief      SPI0 operation complete handler wraper
//------------------------------------------------------------------------------
void SPI0_operationComplete(void)
{
    #ifdef SPI_handlerCaller
        SPI_handlerCaller(SPI0_handler);
    #else
        SPI0_handler();
    #endif
}
#endif // SPI0_ASYNCHRONOUS

//------------------------------------------------------------------------------
// Function:	
//              SPI0_isr()
// Description:
//! \brief      SPI0 ISR
//------------------------------------------------------------------------------
/*#pragma vector=SPI0_ISR_VECTOR
__interrupt void SPI0_isr(void)
{
    BYTE tmp;
    
    switch(__even_in_range(SPI0_REG(IV),4))
    {
        case 0:                             // Vector 0 - no interrupt
            break;                          
        case 2:                             // Vector 2 - RXIFG
            while (!(SPI0_REG(IFG)&UCTXIFG));// TX buffer ready?;
            tmp = SPI0_REG(RXBUF);          // read data
            SPI0_REG(TXBUF) = tmp;          // write data
            break;
        case 4:                             // Vector 4 - TXIFG      
            break;                          
        default:
            break;
    }
}
*/

#endif // SPI0_MASTER/SPI0_SLAVE

// Check selected SPI1 mode
#if ( defined(SPI1_MASTER) && defined(SPI1_SLAVE))
    #error SPI: Select proper SPI1 mode!
#elif ( defined(SPI1_MASTER) || defined(SPI1_SLAVE))

//------------------------------------------------------------------------------
// Function:	
//              SPI1_init()
// Description:
//! \brief      Initialize SPI1
//------------------------------------------------------------------------------
void SPI1_init(void)
{
    // Check selected SPI1 mode
    #if ( defined(SPI1_MASTER) && defined(SPI1_SLAVE))
        #error SPI: Select proper SPI1 mode!
    #endif

    // Initialize SS for slave mode
    #ifdef SPI1_SLAVE
        GPIO_func2(SPI1_STE_PIN);
    #endif
    
    // Initialize SCLK, MISO, MOSI  
    GPIO_func2(SPI1_SCLK_PIN);
    GPIO_func2(SPI1_SIMO_PIN);
    GPIO_func2(SPI1_SOMI_PIN);

	SPI1_REG(CTL1) |= UCSWRST;              // **Put state machine in reset**

    SPI1_REG(CTL0) = UCCKPL+UCSYNC+UCMSB;   // 8-bit SPI, MSB
    #ifdef SPI1_SLAVE
        SPI1_REG(CTL0) |= UCMODE_2;         // SPI slave, SS: active low 
    #endif
    #ifdef SPI1_MASTER
        SPI1_REG(CTL0) |= UCMST;            // SPI master
            
        #if (SPI1_CLK == SMCLK_FREQ)
            SPI1_REG(CTL1) |= UCSSEL__SMCLK;// SPI1_CLK = SMCLK
        #elif (SPI1_CLK == ACLK_FREQ)
            SPI1_REG(CTL1) |= UCSSEL__ACLK; // SPI1_CLK = ACLK
        #else
            #error SPI: Unsupported SPI1 clock frequency
        #endif

        #if ( (SPI1_BAUDRATE) < 1000 || (SPI1_BAUDRATE) > 8000000L)
            #error SPI: Unsupported SPI1 baudrate
        #else
            SPI1_REG(BR0) = (((SPI1_CLK/SPI1_BAUDRATE) - 1)&0xFF);
            SPI1_REG(BR1) = (((SPI1_CLK/SPI1_BAUDRATE) - 1)>>8)&0xFF;
        #endif
    #endif

	SPI1_REG(CTL1) &= ~UCSWRST;             // **Initialize USCI state machine**

    //SPI1_REG(IE) |= UCRXIE;               // Enable USCI_A0 RX interrupt
    //SPI1_REG(IE) |= UCTXIE;               // Enable USCI_A0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              SPI1_waitMISO()
// Description:
//! \brief      Wait till MISO pin goes down
//------------------------------------------------------------------------------
bool SPI1_waitMISO(void)
{
    uint8_t timeout = 100;
    
    // check MOSI pin state
    while(GPIO_test(SPI1_SOMI_PIN))
    {
        if(--timeout == 0)
            return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//                      SPI1_txByte()
// Description:
//! \brief              Transmit byte via SPI1
//!
//! \param *dataByte    byte to transmit
//! \sa SPI1_transmit()
//------------------------------------------------------------------------------
void SPI1_txByte(byte_t dataByte)
{
	while (!(SPI1_REG(IFG) & UCTXIFG));		// USCI_B0 TX buffer ready?
    SPI1_REG(TXBUF) = dataByte;             // write data byte
}

//------------------------------------------------------------------------------
// Function:	
//                    SPI1_rxByte()
// Description:
//! \brief            Receive byte via SPI1
//!
//! \return           received byte
//! \sa SPI1_receive()
//------------------------------------------------------------------------------
byte_t SPI1_rxByte(void)
{
    BYTE dataByte;
  
	while (!(SPI1_REG(IFG) & UCTXIFG));     // TX buffer ready?
    SPI1_REG(IFG) &= ~(UCRXIFG);            // clear RX flags
	SPI1_REG(TXBUF) = 0x00;                 // write dummy data

	while (SPI1_REG(STAT) & UCBUSY);        // TX buffer ready?    
	while (!(SPI1_REG(IFG) & UCRXIFG));     // RX buffer ready?
    dataByte = SPI1_REG(RXBUF);             // read data byte

    return dataByte;
}

//------------------------------------------------------------------------------
// Function:	
//                    SPI1_exchByte()
// Description:
//! \brief            Exchange data via SPI1
//!
//! \return           received byte
//! \sa SPI1_txByte(), SPI1_rxByte(), SPI1_receive(), SPI1_transmit()
//------------------------------------------------------------------------------
uint8_t SPI1_exchByte(uint8_t txByte)
{
    BYTE rxByte;
  
    while (!(SPI1_REG(IFG & UCTXIFG)));     // TX buffer ready?
    SPI1_REG(IFG) &= ~(UCRXIFG);            // clear RX flags
	SPI1_REG(TXBUF) = txByte;               // write data

	while (SPI1_REG(STAT) & UCBUSY);		// TX buffer ready?       
    while (!(SPI1_REG(IFG) & UCRXIFG));     // RX buffer ready?
    rxByte = SPI1_REG(RXBUF);               // read data byte

    return rxByte;
}

//------------------------------------------------------------------------------
// Function:	
//                  SPI1_transmit()
// Description:
//! \brief          Transmit byte array via SPI1 (UCB1)
//!
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI1_txByte()
//------------------------------------------------------------------------------
void SPI1_transmit(const void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(SPI1_REG(IFG) & UCTXIFG));  // TX buffer ready?
        SPI1_REG(TXBUF) = ((const byte_t*)data)[index]; // send data
    }
}

//------------------------------------------------------------------------------
// Function:	
//                  SPI1_receive()
// Description:
//! \brief          Receive byte array via SPI1 (UCB1)
//!
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI1_rxByte()
//------------------------------------------------------------------------------
void SPI1_receive(void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(SPI1_REG(IFG) & UCTXIFG));	// TX buffer ready?
        SPI1_REG(IFG) &= ~(UCRXIFG);        // clear RX flags
        SPI1_REG(TXBUF) = 0x00;             // write dummy data

        while (SPI1_REG(STAT) & UCBUSY);    // TX buffer ready?    
        while (!(SPI1_REG(IFG) & UCRXIFG)); // RX buffer ready?
        ((byte_t*)data)[index] = SPI1_REG(RXBUF);// read data byte
    }
}

//------------------------------------------------------------------------------
// Function:	
//              SPI1_isr()
// Description:
//! \brief      SPI1 ISR
//------------------------------------------------------------------------------
/*#pragma vector=SPI1_ISR_VECTOR
__interrupt void SPI1_isr(void)
{
    BYTE tmp;
    
    switch(__even_in_range(UCB1IV,4))
    {
        case 0:                             // Vector 0 - no interrupt
            break;                          
        case 2:                             // Vector 2 - RXIFG
            while (!(SPI1_REG(IFG)&UCTXIFG));// USCI_B1 TX buffer ready?;
            tmp = UCB1RXBUF;                // read data
            SPI1_REG(TXBUF) = tmp;          // write data
            break;
        case 4:                             // Vector 4 - TXIFG      
            break;                          
        default:
            break;
    }
}
*/

#endif  // SPI1_MASTER/SPI1_SLAVE

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

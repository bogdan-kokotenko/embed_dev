//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/spi.c
//! \brief  MSP430 SPI functions implementation.			
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  11/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "i2c.h"
#include "spi.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning SPI: Unknown MCU core, check HAL configuration!
#else

// Check if I2C mode selected
#if ( defined(I2C_MASTER) || defined(I2C_SLAVE) )
    #error SPI: Interface is not avaliable if I2C mode selected!
#endif

// Check selected SPI mode
#if ( defined(SPI_MASTER) && defined(SPI_SLAVE) )
    #error SPI: Select proper SPI mode!
#elif ( defined(SPI_MASTER) || defined(SPI_SLAVE))

//------------------------------------------------------------------------------
// Function:	
//              SPI_init()
// Description:
//! \brief      Initialize SPI
//------------------------------------------------------------------------------
void SPI_init(void)
{
    // Initialize chip select for slave mode
    #ifdef SPI_SLAVE
        GPIO_func2(SPI_STE_PIN);
    #endif
    
    // Initialize SCLK, MISO, MOSI  
    GPIO_func3(SPI_CLK_PIN);
    GPIO_func3(SPI_SIMO_PIN);
    GPIO_func3(SPI_SOMI_PIN);

	SPI_REG(CTL1) |= UCSWRST;               // **Put state machine in reset**

    SPI_REG(CTL0) = UCCKPL+UCSYNC+UCMSB;    // 8-bit SPI, MSB
    #ifdef SPI_SLAVE
        SPI_REG(CTL0) |= UCMODE_2;          // SPI slave, SS: active low 
    #endif
    #ifdef SPI_MASTER
        SPI_REG(CTL0) |= UCMST;             // SPI master
            
        #if (SPI_CLK == SMCLK_FREQ)
            SPI_REG(CTL1) |= UCSSEL_2;      // SPI_CLK = SMCLK
        #elif (SPI_CLK == ACLK_FREQ)
            SPI_REG(CTL1) |= UCSSEL_1;      // SPI_CLK = ACLK
        #else
            #error SPI: Unsupported SPI clock frequency
        #endif

        #if ( (SPI_BAUDRATE) < 1000 || (SPI_BAUDRATE) > 8000000L)
            #error SPI: Unsupported SPI baudrate
        #else
            SPI_REG(BR0) = (((SPI_CLK/SPI_BAUDRATE) - 1)&0xFF);
            SPI_REG(BR1) = (((SPI_CLK/SPI_BAUDRATE) - 1)>>8)&0xFF;
        #endif
    #endif

	SPI_REG(CTL1) &= ~UCSWRST;              // **Initialize USCI state machine**

    //SPI_REG(IE) |= UCRXIE;                // Enable USCI_A0 RX interrupt
    //SPI_REG(IE) |= UCTXIE;                // Enable USCI_A0 TX interrupt
}

//------------------------------------------------------------------------------
// Function:	
//              SPI_isBusy()
// Description:
//! \brief      Check if SPI is ready for data exchange
//------------------------------------------------------------------------------
bool SPI_isBusy(void)
{
    // check if transmission complete
	return (!(IFG2 & SPI_REG(TXIFG)));
}

//------------------------------------------------------------------------------
// Function:	
//              SPI_waitMISO()
// Description:
//! \brief      Wait till MISO pin goes down
//------------------------------------------------------------------------------
bool SPI_waitMISO(void)
{
    uint8_t timeout = 100;
    
    // check MOSI pin state
    while(GPIO_test(SPI_SOMI_PIN))
    {
        if(--timeout == 0)
            return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//                      SPI_txByte()
// Description:
//! \brief              Transmit byte via SPI
//!
//! \param *dataByte    byte to transmit
//! \sa SPI1_transmit()
//------------------------------------------------------------------------------
void SPI_txByte(byte_t dataByte)
{
    while (!(IFG2 & SPI_REG(TXIFG)));       // TX buffer ready?
    SPI_REG(TXBUF) = dataByte;             // write data byte
    while (!(IFG2 & SPI_REG(TXIFG)));       // TX buffer ready?
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
uint8_t SPI_rxByte(void)
{
    BYTE dataByte;
  
    while (!(IFG2 & SPI_REG(TXIFG)));       // TX buffer ready?
    IFG2&= ~(SPI_REG(RXIFG));               // clear RX flag
	SPI_REG(TXBUF) = 0x00;                  // write dummy data

	while (SPI_REG(STAT) & UCBUSY);         // TX buffer ready?    
    while (!(IFG2 & SPI_REG(RXIFG)));       // RX buffer ready?
    dataByte = SPI_REG(RXBUF);              // read data byte

    return dataByte;
}

//------------------------------------------------------------------------------
// Function:	
//                    SPI_exchByte()
// Description:
//! \brief            Exchange data via SPI1
//!
//! \return           received byte
//! \sa SPI1_receive(), SPI_transmit()
//------------------------------------------------------------------------------
uint8_t SPI_exchByte(uint8_t txByte)
{
    BYTE rxByte;
  
    while (!(IFG2 & SPI_REG(TXIFG)));       // TX buffer ready?
    IFG2&= ~(SPI_REG(RXIFG));               // clear RX flag
	SPI_REG(TXBUF) = txByte;                // write data

	while (SPI_REG(STAT) & UCBUSY);         // TX buffer ready?    
    while (!(IFG2 & SPI_REG(RXIFG)));       // RX buffer ready?
    rxByte = SPI_REG(RXBUF);                // read data byte

    return rxByte;
}

//------------------------------------------------------------------------------
// Function:	
//                  SPI_transmit()
// Description:
//! \brief          Transmit byte array via SPI (UCB0)
//!
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI1_txByte()
//------------------------------------------------------------------------------
void SPI_transmit(const void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(IFG2 & SPI_REG(TXIFG)));   // TX buffer ready?
        SPI_REG(TXBUF) = ((const byte_t*)data)[index]; // send data
    }
	while (SPI_REG(STAT) & UCBUSY);         // TX buffer ready?    
}

//------------------------------------------------------------------------------
// Function:	
//                  SPI_receive()
// Description:
//! \brief          Receive byte array via SPI (UCB0)
//!
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI_rxByte()
//------------------------------------------------------------------------------
void SPI_receive(void* data, uint16_t size)
{
    uint16_t index;
    
    for(index = 0; index < size; index++)
    {
        while (!(IFG2 & SPI_REG(TXIFG)));   // TX buffer ready?
        IFG2&= ~(SPI_REG(RXIFG));           // clear RX flag
        SPI_REG(TXBUF) = 0x00;              // write dummy data

        while (SPI_REG(STAT) & UCBUSY);     // TX buffer ready?    
        while (!(IFG2 & SPI_REG(RXIFG)));   // RX buffer ready?
        ((byte_t*)data)[index] = SPI_REG(RXBUF);// read data byte
    }
}

#endif // SPI_MASTER/SPI_SLAVE

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file
//******************************************************************************

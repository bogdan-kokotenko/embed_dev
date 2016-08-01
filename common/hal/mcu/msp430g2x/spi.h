//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_spi  MSP430G2x SPI APIs
//! \brief MSP430G2x SPI APIs
//! @{
//******************************************************************************
//   File description:
//! \file   hal/mcu/msp430g2x/spi.h
//! \brief  MSP430 SPI API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------
//!  11/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "spi_config.h"    

//----------------------------- SPI APIs --------------------------------------
  
//! SPI chip select initialization
#define SPI_chipInit(port, pin)         GPIO_setOut(port, pin);

//!  Slave chip select for SPI
#define SPI_chipSelect(port, pin)       GPIO_setLow(port, pin)

//!  Slave chip release for SPI
#define SPI_chipRelease(port, pin)      GPIO_setHigh(port, pin)
    
//! Initialize SPI
void SPI_init(void);

//! Check if SPI is ready for data exchange
bool SPI_isBusy(void);

//! Wait till MISO pin goes down
bool SPI_waitMISO(void);

//! \brief  Transmit byte via SPI
//! \sa SPI_transmit()
void SPI_txByte(uint8_t dataByte);

//! Receive byte via SPI
//! \return Received byte
//! \sa SPI_receive()
uint8_t SPI_rxByte(void);

//! \brief  Exchange data via SPI1
//! \return received byte
//! \sa SPI1_receive(), SPI_transmit()
uint8_t SPI_exchByte(uint8_t txByte);

//! \brief  Transmit byte array via SPI
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI_txByte()
void SPI_transmit(const void* data, uint16_t size);

//! \brief  Receive byte array via SPI
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI_rxByte()
void SPI_receive(void* data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif // SPI_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

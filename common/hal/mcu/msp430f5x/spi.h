//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_spi  MSP430F5x SPI APIs
//! \brief MSP430F5x SPI APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/spi.h
//! \brief  MSP430F5x SPI API			
//!		
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -------------------------------
//!  30/01/2014 | Bogdan Kokotenko | Initial draft
//!  21/01/2015 | Bogdan Kokotenko | Added universal SPI APIs
//!  30/11/2015 | Bogdan Kokotenko | Improved SPI APIs
//
//******************************************************************************
#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "spi_config.h"    

//! SPI chip select initialization
#define SPI_chipInit(port, pin)        GPIO_setOut(port, pin)

//!  Slave chip select for SPI
#define SPI_chipSelect(port, pin)      GPIO_setLow(port, pin)

//!  Slave chip release for SPI
#define SPI_chipRelease(port, pin)     GPIO_setHigh(port, pin)    
    
//----------------------------- SPI0 APIs --------------------------------------
//! \brief Initialize SPI0
void SPI0_init(void);

//! \brief Wait till MISO pin goes down
//!
//! \return     false if timeout
bool SPI0_waitMISO(void);

//! \brief Check if SPI0 is ready for data exchange
//! \return     state (true/false)
bool SPI0_isBusy(void);

//! \brief Reset transmission via SPI0
void SPI0_reset(void);

//! \brief Transmit byte via SPI0
//! \sa SPI0_transmit()
void SPI0_txByte(uint8_t dataByte);

//! \brief Receive byte via SPI0
//! \return Received byte
//! \sa SPI0_receive()
uint8_t SPI0_rxByte(void);

//! \brief Exchange data via SPI0
uint8_t SPI0_exchByte(uint8_t txByte);

//! \brief Transmit byte array via SPI0
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI0_txByte()
#ifdef SPI0_ASYNCHRONOUS
void SPI0_transmit(const void* data, uint16_t size, void (*handler)(void));
#else
void SPI0_transmit(const void* data, uint16_t size);
#endif

//! \brief Receive byte array via SPI0
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI0_rxByte()
#ifdef SPI0_ASYNCHRONOUS
void SPI0_receive(void* data, uint16_t size, void (*handler)(void));
#else
void SPI0_receive(void* data, uint16_t size);
#endif

//! Transmit/Receive packet (same size) via SPI0
void SPI0_exchange(void* rxPacket, const void* txPacket,
                  uint16_t size, void (*handler)(void));

//----------------------------- SPI1 APIs --------------------------------------
//! Initialize SPI1
void SPI1_init(void);

//! \brief Wait till MISO pin goes down
//! \return     false if timeout
bool SPI1_waitMISO(void);

//! Check if SPI1 is ready for data exchange
bool SPI1_isBusy(void);

//! \brief Reset transmission via SPI1
void SPI1_reset(void);

//! \brief Transmit byte via SPI1
//! \sa SPI1_transmit()
void SPI1_txByte(uint8_t dataByte);

//! \brief Receive byte via SPI1
//! \return Received byte
//! \sa SPI1_receive()
uint8_t SPI1_rxByte(void);

//! \brief Exchange data via SPI1
uint8_t SPI1_exchByte(uint8_t txByte);

//! \brief Transmit byte array via SPI1
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI1_txByte()
void SPI1_transmit(const void* data, uint16_t size);

//! \brief Receive byte array via SPI1
//! \param *data    pointer to the byte array
//! \param size     size of byte array
//! \sa SPI1_rxByte()
void SPI1_receive(void* data, uint16_t size);

//! \brief Transmit/Receive packet (same size) via SPI1
void SPI1_exchange(void* rxPacket, const void* txPacket,
                  uint16_t size, HANDLE handler);

#ifdef __cplusplus
}
#endif

#endif // SPI_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

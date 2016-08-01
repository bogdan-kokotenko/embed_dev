//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_i2c  MSP430G2x I2C APIs
//! \brief MSP430G2x I2C APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/i2c.h
//! \brief      MSP430G2x I2C API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------------
//!  30/06/2015 | Bogdan Kokotenko | Initial draft
//!  13/02/2016 | Bogdan Kokotenko | Ported to MSP430G2x
//
//******************************************************************************
#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "i2c_config.h"
    
// -------------------- I2C APIs --------------------

//! Initialize I2C
bool I2C_init(void);

//! Check I2C state
bool I2C_isBusy(void);

//! \brief Check I2C error
//! \details Check if the last rx/tx operation finished with NACK or timeout
bool I2C_error(void);

//! \brief Set I2C own address
//! \param address  Own address
void I2C_setOwnAddress(uint8_t address);

//! \brief Select I2C slave address
//! \note Used by master before data exchange
//! \param address  Slave address
void I2C_selectSlaveAddress(uint8_t address);

//! \brief Send data byte via I2C
//! \param dataByte Byte to send
//! \sa I2C_error()
void I2C_txByte(uint8_t dataByte);

//! \brief Receive data byte via I2C
//! \return   Received byte (or 0x00 in case of FAILURE)
//! \sa I2C_error()
uint8_t I2C_rxByte(void);

//! \brief Send data via I2C0
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to send
//! \param handler  Function is called when operation completed
//! \sa I2C_error()
#ifdef I2C_MASTER
void I2C_transmit(uint8_t address,
                   const void* data, uint16_t size, void (*handler)(void));
#else
void I2C_transmit(const void* data, uint16_t size, void (*handler)(void));
#endif

//! \brief Receive byte array via I2C0
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to receive
//! \param handler  Function is called when opration completed
//! \sa I2C_error()
#ifdef I2C_MASTER
void I2C_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void));
#else
void I2C_receive(void* data, uint16_t* size, void (*handler)(void));
#endif

#ifdef I2C_MASTER
//! \brief Transmit request and receive response via I2C
//! \note Used only by master device
//! \param address  Slave device address
//! \param txData   Pointer to the TX data
//! \param txSize   Size of the TX data
//! \param rxData   Pointer to the RX data
//! \param rxSize   Size of the RX data
//! \param handler  Function is called when opration completed
//! \sa I2C_error()
void I2C_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void));
#endif

//! I2C NACK handler
void I2C_nackHandler(void);

#ifdef I2C_SLAVE
//! I2C STOP handler
void I2C_stopHandler(void);
#endif

//! I2C RX handler
void I2C_rxHandler(void);

//! I2C TX handler
void I2C_txHandler(void);

#ifdef __cplusplus
}
#endif

#endif // I2C_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************
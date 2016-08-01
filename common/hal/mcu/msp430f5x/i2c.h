//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_i2c  MSP430F5x I2C APIs
//! \brief MSP430F5x I2C APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/i2c.h
//! \brief  MSP430 I2C API			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------------
//!  11/05/2015 | Artem Porkhun    | Initial draft
//!  29/06/2015 | Bogdan Kokotenko | Added macro for fast USCI unit change.
//!  30/06/2015 | Bogdan Kokotenko | Added ISR based async functions.
//!  30/11/2015 | Bogdan Kokotenko | Added I2C advanced initialization
//
//******************************************************************************
#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "i2c_config.h"
    
// -------------------- I2C0 APIs --------------------

//! Initialize I2C0
//!
//! \return   SUCCESS or false
bool I2C0_init(void);

//! Check I2C0 state
//!
//! \return   true of false      
bool I2C0_isBusy(void);

//! Check I2C0 error
//!
//! Check if the last rx/tx operation finished with NACK or timeout
//! \return   true or false
bool I2C0_error(void);

//! Set I2C0 own address
//!
//! \param address  Own address
void I2C0_setOwnAddress(uint8_t address);

//! Select I2C0 slave address
//!
//! \note Used by master before data exchange
//! \param address  Slave address
void I2C0_selectSlaveAddress(uint8_t address);

//! Send data byte via I2C0
//!
//! \param dataByte Byte to send
//! \sa I2C0_error()
void I2C0_txByte(uint8_t dataByte);

//! Receive data byte via I2C0
//!
//! \return   Received byte (or 0x00 in case of FAILURE)
//! \sa I2C0_error()
uint8_t I2C0_rxByte(void);

//! Send data via I2C0
//!
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to send
//! \param handler  Function is called when opration completed
//! \sa I2C0_error()
#ifdef I2C0_MASTER
void I2C0_transmit(uint8_t address,
                   const void* data, uint16_t size, void (*handler)(void));
#else
void I2C0_transmit(const void* data, uint16_t size, void (*handler)(void));
#endif

//! Receive byte array via I2C0
//!
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to receive
//! \param handler  Function is called when opration completed
//! \sa I2C0_error()
#ifdef I2C0_MASTER
void I2C0_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void));
#else
void I2C0_receive(void* data, uint16_t* size, void (*handler)(void));
#endif

//! Transmit request and receive response via I2C0
//!
//! \note Used only by master device
//! \param address  Slave device address
//! \param txData   Pointer to the TX data
//! \param txSize   Size of the TX data
//! \param rxData   Pointer to the RX data
//! \param rxSize   Size of the RX data
//! \param handler  Function is called when opration completed
//! \sa I2C0_error()
void I2C0_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void));

// -------------------- I2C1 APIs --------------------

//! Initialize I2C1
//!
//! \return   SUCCESS or false
bool I2C1_init(void);

//! Check I2C1 state
//!
//! \return   true of false      
bool I2C1_isBusy(void);

//! Check I2C1 error
//!
//! Check if the last rx/tx operation finished with NACK or timeout
//! \return   true or false
bool I2C1_error(void);

//! Set I2C1 own address
//!
//! \param address  Own address
void I2C1_setOwnAddress(uint8_t address);

//! Select I2C1 slave address
//!
//! \param address  Slave address
void I2C1_selectSlaveAddress(uint8_t address);

//! Send data byte via I2C1
//!
//! \param dataByte Byte to send
//! \return   SUCCESS or false      
//! \sa I2C1_error()
void I2C1_txByte(uint8_t dataByte);

//! Receive data byte via I2C1
//!
//! \return   Received byte (or 0x00 in case of FAILURE)
//! \sa I2C1_error()
uint8_t I2C1_rxByte(void);

//! Send data via I2C1
//!
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to send
//! \param handler  Function is called when opration completed
//! \sa I2C1_error()
#ifdef I2C1_MASTER
void I2C1_transmit(uint8_t address,
                   const void* data, uint16_t size, void (*handler)(void));
#else
void I2C1_transmit(const void* data, uint16_t size, void (*handler)(void));
#endif

//! Receive byte array via I2C1
//!
//! \param address  Slave device address
//! \param data     Pointer to the data
//! \param size     Size of data to receive
//! \param handler  Function is called when opration completed
//! \sa I2C1_error()
#ifdef I2C1_MASTER
void I2C1_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void));
#else
void I2C1_receive(void* data, uint16_t* size, void (*handler)(void));
#endif

//! Transmit request and receive response via I2C1
//!
//! \note Used only by master device
//! \param address  Slave device address
//! \param txData   Pointer to the TX data
//! \param txSize   Size of the TX data
//! \param rxData   Pointer to the RX data
//! \param rxSize   Size of the RX data
//! \param handler  Function is called when opration completed
//! \sa I2C_error(), I2C1_response()
void I2C1_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void));

#ifdef __cplusplus
}
#endif

#endif // I2C_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************
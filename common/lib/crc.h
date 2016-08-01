//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup libs
//! @{
//******************************************************************************
//   File description:
//! \file   lib\crc.h
//! \brief  CRC APIs.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!        2000 | Michael Barr     | Initial draft(http://www.netrino.com/code/)
//!  19/02/2015 | Bogdan Kokotenko | Added CRC16CCITT
//
//******************************************************************************
#ifndef CRC_H
#define CRC_H

#ifdef __cplusplus
extern "C" {
#endif

//! Compute the CRC16 of a given data
uint16_t crc16(const void* data, uint16_t size);

//! Compute the CRC16IBM of a given data
uint16_t crc16ibm(const void* data, uint16_t size);

//! Compute the CRC16CCITT of a given data
uint16_t crc16ccitt(const void* data, uint16_t size);

//! Compute the CRC8 (1-Wire) of a given data
uint8_t crc8(const void* data, uint8_t size );

#ifdef __cplusplus
}
#endif

#endif // CRC_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

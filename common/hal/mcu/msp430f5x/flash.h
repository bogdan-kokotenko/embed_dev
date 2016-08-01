//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_flash  MSP430F5x Flash APIs
//! \brief MSP430F5x internal flash APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/flash.h
//! \brief  MSP430 internal flash APIs			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ---------------------------------------
//!  17/01/2016 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#ifndef FLASH_H
#define FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

//! Flash bank erase
void FLASH_bankErase(void* bankAddress);

//! Flash segment erase
void FLASH_segmentErase(void* segmentAddress);

//! Flash page write
void FLASH_write(void* pagePtr, void *data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif // FLASH_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

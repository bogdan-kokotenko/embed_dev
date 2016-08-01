//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/flash.c
//! \brief  MSP430F5x flash operation implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "flash.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning FLASH: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              FLASH_bankErase()
// Description:
//!             Flash bank erase
//------------------------------------------------------------------------------
void FLASH_bankErase(void* bankAddress)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);

    FCTL3 = FWKEY;                          // Clear Lock bit
    FCTL1 = FWKEY + MERAS;                  // Set Erase bit
    
    *((uint8_t*)bankAddress) = 0;           // Dummy write to erase Flash seg
    while (FCTL3 & BUSY);                   // test busy
    
    FCTL1 = FWKEY;                          // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   // Set LOCK bit

    LeaveCriticalSection(intState);   
}
    
//------------------------------------------------------------------------------
// Function:	
//              FLASH_segmentErase()
// Description:
//!             Flash segment erase
//------------------------------------------------------------------------------
void FLASH_segmentErase(void* segmentAddress)
{
    IntFlag_t intState;
    EnterCriticalSection(intState);

    FCTL3 = FWKEY;                          // Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  // Set Erase bit
    
    *((uint8_t*)segmentAddress) = 0;         // Dummy write to erase Flash seg
    while (FCTL3 & BUSY);                   // test busy
    
    FCTL1 = FWKEY;                          // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   // Set LOCK bit

    LeaveCriticalSection(intState);   
}

//------------------------------------------------------------------------------
// Function:	
//              FLASH_write()
// Description:
//! \brief      Flash page write
//------------------------------------------------------------------------------
void FLASH_write(void* pagePtr, void *data, uint16_t size)
{
    uint8_t* dataPtr = (uint8_t*)data;
    uint8_t* flashPtr = (uint8_t*)pagePtr;

    IntFlag_t intState;
    EnterCriticalSection(intState);

    FCTL3 = FWKEY;                                  // Clear Lock bit
    FCTL1 = FWKEY+WRT;                              // Enable byte write mode

    while (size > 0) 
    {
        while (FCTL3 & BUSY);                       // test busy
        *flashPtr++ = *dataPtr++;                   // Write to Flash
        size--;
    }

    FCTL1 = FWKEY;                                  // Clear write bit
    FCTL3 = FWKEY + LOCK;                           // Set LOCK bit
    
    LeaveCriticalSection(intState);   
}

#endif // _MSP430_HAL_

//******************************************************************************
// End of file
//******************************************************************************

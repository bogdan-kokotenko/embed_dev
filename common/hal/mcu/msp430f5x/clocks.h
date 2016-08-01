//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_clocks  MSP430F5x Clocks APIs
//! \brief MSP430F5x clock sources APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/clocks.h
//! \brief  MSP430F5x clock sources API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//!  26/01/2015 | Bogdan Kokotenko | Added macros for usec delay
//
//******************************************************************************
#ifndef CLOCKS_H
#define CLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "clocks_config.h"    
    
//! Software delay (ms)
#define __delay_ms(MS)  __delay_cycles((MCLK_FREQ*(MS))/1000)

//! Software delay (us)
#define __delay_us(MS)  __delay_cycles((MCLK_FREQ*(MS))/1000000L)
  
//! Initialize system clocks
void CLK_init(void);

//! CPU/System minimal performance levels
#define CPU_SPEED_MIN   DIVM__16,
//! CPU/System midle performance levels
#define CPU_SPEED_MID   DIVM__4,
//! CPU/System maximal performance levels
#define CPU_SPEED_MAX   DIVM__1

//! Set CPU/System performance
#define CPU_setPerformance(CPU_SPEED_Level)                                    \
    /* Set MCLK divider */                                                     \
    UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (CPU_SPEED_Level);                       

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

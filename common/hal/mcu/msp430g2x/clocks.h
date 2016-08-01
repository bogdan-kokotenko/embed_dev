//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430g2x_hal
//! @{
//! \defgroup msp430g2x_clock  MSP430G2x Clocks APIs
//! \brief MSP430G2x clock sources APIs
//! @{
//******************************************************************************
//   File description:
//! \file       hal/mcu/msp430g2x/clocks.h
//! \brief      MSP430G2x clock sources API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//!  26/01/2015 | Bogdan Kokotenko | Added macros for usec delay
//
//******************************************************************************

#ifndef CLOCKS_H_
#define CLOCKS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "clocks_config.h"

//! Internal low-power oscillator frequency (VLO)
#define VLO_FREQ        12000       // Hz

//! Software delay (ms)
#define __delay_ms(MS)  __delay_cycles((MCLK_FREQ*(MS))/1000)

//! Software delay (us)
#define __delay_us(MS)  __delay_cycles((MCLK_FREQ*(MS))/1000000L)
  
//! Initialize system clocks
void CLK_init(void);

//! CPU/System minimal performance levels
#define CPU_SPEED_MIN   DIVM__3,
//! CPU/System midle performance levels
#define CPU_SPEED_MID   DIVM__1,
//! CPU/System maximal performance levels
#define CPU_SPEED_MAX   DIVM__0

//! Set CPU/System performance
#define CPU_setPerformance(CPU_SPEED_Level)                                    \
    /* Set MCLK divider */                                                     \
    BCSCTL2 = (BCSCTL2 & ~(DIVM_3)) | (CPU_SPEED_Level);                       

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_H_
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

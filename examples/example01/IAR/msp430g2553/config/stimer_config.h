//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_msp430g2x_config
//! @{
//******************************************************************************
//  File description:
//! \file       example01/IAR/msp430g2553/config/stimer_config.h
//! \brief      Software timer configuration
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef STIMER_CONFIG_H
#define STIMER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Set the maximal number of timeouts in the software timer schedule
#define STIMER_SCHEDULE_SIZE    5

//! Define the time interval for software timer schedule check
#define STIMER_LATENCY          1  // msec

//! Software timer source clock precise value (for time correction)
#define STIMER_CLK_FREQUENCY    (T0CLK_FREQ)
    
//! Software timer source initialization
#define STIMER_sourceInit() \
    SysTick_init(STIMER_CLK_FREQUENCY*STIMER_LATENCY/1000)

#ifdef __cplusplus
}
#endif

#endif	//STIMER_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

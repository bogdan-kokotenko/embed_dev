//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_mingw_config
//! @{
//******************************************************************************
//  File description:
//! \file  example01/MinGW/config/devtime_config.h
//! \brief Device time configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef DEVTIME_CONFIG_H
#define DEVTIME_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Time tick
#define DEVTIME_TICK_INTERVAL      (STIMER_LATENCY) // ms

//! Time tick source clock precise value (for time correction)
#define DEVTIME_CLK_FREQUENCY      1000     // Hz

#ifdef __cplusplus
}
#endif

#endif	//DEVTIME_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

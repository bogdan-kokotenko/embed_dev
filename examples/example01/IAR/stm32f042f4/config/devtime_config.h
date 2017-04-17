//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup ex01_stm32f0x_config
//! @{
//******************************************************************************
//  File description:
//! \file   example01/IAR/stm32f042f4/config/devtime_config.h
//! \brief Device time configuration			
//
//******************************************************************************
#ifndef DEVTIME_CONFIG_H
#define DEVTIME_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//! Time tick
#define DEVTIME_TICK_INTERVAL      (STIMER_LATENCY)         // ms

//! Time tick source clock precise value (for time correction)
#define DEVTIME_CLK_FREQUENCY      (STIMER_CLK_FREQUENCY)   // Hz

#ifdef __cplusplus
}
#endif

#endif	//DEVTIME_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

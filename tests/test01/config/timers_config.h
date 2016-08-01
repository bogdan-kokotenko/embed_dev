//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup test01_config
//! @{
//******************************************************************************
//	File description:
//! \file   test01\config\timers_config.h
//! \brief  Timers configuration			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  12/01/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Enable WDT in reset mode
// \sa WDT_init(), WDT_feedWatchdog()
//#define WDT_RST     1000 // ms

//------------------------------------------------------------------------------
// Callbacks section

//! Systick handler
#define Systick_OverflowHandler()   STIMER_tick()
    
#ifdef __cplusplus
}
#endif

#endif	//TIMERS_CONFIG_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

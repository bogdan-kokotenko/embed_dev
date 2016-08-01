//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup system
//! @{
//! \defgroup devtime Device Time
//! \brief System time API
//! @{
//******************************************************************************
//   File description:
//! \file       sys/devtime.h
//! \brief      System time API
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------------
//!  14/02/2015 | Bogdan Kokotenko | Initial draft
//!  24/02/2015 | Bogdan Kokotenko | Added standard types and functions
//
//******************************************************************************
#ifndef	DEVTIME_H
#define DEVTIME_H

#ifdef __cplusplus
extern "C" {
#endif

// Standard time APIs
#include <time.h>  

#include "devtime_config.h"
    
//! Get the current calendar time as a value of type time_t
time_t DEVTIME_time(time_t *t);

//! Sets the system's idea of the time and date
int DEVTIME_stime(time_t *t); 

//------------------------------------------------------------------------------
// Device time APIs

//! Initialize device time
void DEVTIME_init(void);

//! Update device time
void DEVTIME_tick(void);

#ifdef __cplusplus
}
#endif

#endif // DEVTIME_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

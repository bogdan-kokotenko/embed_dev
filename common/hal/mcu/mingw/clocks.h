//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//!
//! \addtogroup mingwhal
//! @{
//******************************************************************************
//  File description:
//! \file   mcu\mingw\clocks.h
//! \brief  MinGW clock sources API			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/05/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef CLOCKS_H
#define CLOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

// Include configurations
#include "clocks_config.h"

//! Initialize system clocks
void CLK_init(void);

#ifdef __cplusplus
}
#endif

#endif // CLOCKS_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

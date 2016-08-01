//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//  File description:
//! \file       hal\mcu\mingw\clocks.c
//! \brief      MinGW clock sources handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  24/07/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MINGW_HAL_) )
#warning CLOCKS: Unknown MCU core, check HAL configuration!
#else

//------------------------------------------------------------------------------
// Function:	
//              CLK_init()
// Description:
//! \brief      Initialize system clocks
//------------------------------------------------------------------------------
void CLK_init(void)
{

}

#endif // _MINGW_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

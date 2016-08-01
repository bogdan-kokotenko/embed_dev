//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//	File description:
//! \file       lib\dsp.c
//! \brief      Contains basic DSP algorithm based on integral types
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | --------------------------------
//!  03/04/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "types.h"

#include <stdlib.h>

//------------------------------------------------------------------------------
// Function:        fastRoot16()
// Description:
//! \brief          Calculate fast root for 16-bit value
//! \param x        input integral value
//------------------------------------------------------------------------------
uint8_t fastRoot16(uint16_t x)
{
    uint16_t m = 0x4000;
    uint16_t y = 0;
    uint16_t b;
    
    while (m != 0)
    {
        b = y | m;
        y = y >> 1;
        if (x >= b) 
        {
            x = x - b;
            y = y | m;
        }
        m = m >> 2;
    }
    return y;
}

//------------------------------------------------------------------------------
// Function:        fastRoot32()
// Description:
//! \brief          Calculate fast root for 32-bit value
//! \param x        input integral value
//------------------------------------------------------------------------------
uint16_t fastRoot32(uint32_t x)
{
    uint32_t m = 0x40000000;
    uint32_t y = 0;
    uint32_t b;
    
    while (m != 0)
    {
        b = y | m;
        y = y >> 1;
        if (x >= b) 
        {
            x = x - b;
            y = y | m;
        }
        m = m >> 2;
    }
    return y;
}

//******************************************************************************
// End of file
//******************************************************************************

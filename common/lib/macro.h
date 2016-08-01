//******************************************************************************
// Copyright (C) 2016 Bogdan Kokotenko
//
//! \defgroup libs Algorithm Library
//! \brief Collection of the standard algorithms widely used in embedded systems
//! @{
//******************************************************************************
//   File description:
//! \file   lib\macro.h
//! \brief  Macro functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  21/04/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef MACRO_H
#define MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

//! \brief Extract higher part (16 bit) of double word (32 bit)
//! \hideinitializer
#define highWord(x)	( (unsigned int)((x)>>16) )

//! \brief Extract lower part (16 bit) of double word (32 bit)
//! \hideinitializer
#define lowWord(x)	( (unsigned int) (x) )

//! \brief Extract higher part (8 bit) of word (16 bit)
//! \hideinitializer
#define highByte(x)	( (unsigned char)(((x)&0xFF00)>>8) )

//! \brief Extract lower part (8 bit) of word (16 bit)
//! \hideinitializer
#define lowByte(x)	( (unsigned char) ((x)&0x00FF) )   

//! \brief Check if value is even
//! \hideinitializer
#define isEven(x) ((((x)>>1)<<1) == (x) ? 1:0)

//! \brief Check if value is odd
//! \hideinitializer
#define isOdd(x)  ((((x)>>1)<<1) != (x) ? 1:0)  

//! \brief Convert endianness (little<->big) of 16-bit word
//! \hideinitializer
#define changeEndiannessWORD(x)    ((x<<8) | (x>>8)&0xFF)

//! \brief Convert endianness (little<->big) of 32-bit word
//! \hideinitializer
#define changeEndiannessDWORD(x)   ((x<<24) | (x>>24)&0xFF | \
                                     (x<<8)&0xFF0000 | (x>>8)&0xFF00)
  
#ifdef __cplusplus
}
#endif

#endif // MACRO_H
//! @}
//******************************************************************************
// End of file
//******************************************************************************

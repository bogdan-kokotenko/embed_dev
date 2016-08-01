//******************************************************************************
// Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   types.h
//! \brief  Contains custom types definition			
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | -----------------------------
//!  15/02/2013 | Bogdan Kokotenko | Initial draft
//!   3/05/2013 | Bogdan Kokotenko | Added macro for sizeof check
//
//******************************************************************************
#ifndef	TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//! Custom u8 type
typedef unsigned char	byte_t;

//! Universal address pointer
typedef void*	        ptr_t;

//! Custom NULL definition
#ifndef NULL
#define NULL    ((void*)0)
#endif

//! Check sizeof condition during compilation
#define ct_assert(e) ((void)sizeof(char[1 - 2*!(e)]))

#endif // TYPES_H

//******************************************************************************
// End of file
//******************************************************************************

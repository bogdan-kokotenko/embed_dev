//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//! \addtogroup libs
//! @{
//******************************************************************************
//   File description:
//! \file   lib\dsp.h
//! \brief  Contains basic DSP algorithm based on integral types
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  03/04/2016 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#ifndef DSP_H
#define DSP_H

//------------------------------------------------------------------------------
// Macro:			CircularBuffer()
// Description: 	
//! \brief          Type template of CircularBuffer
//! \param BuffSize buffer size
//! \hideinitializer
//------------------------------------------------------------------------------
#define CircularBufferS16(BuffSize) struct{                                 \
	uint16_t    index;                  /* index */                         \
	int16_t     buff[BuffSize];         /* buffer */                        \
}

//------------------------------------------------------------------------------
// Macro:			CircularBuffer()
// Description: 	
//! \brief          Type template of CircularBuffer
//! \param BuffSize buffer size
//! \hideinitializer
//------------------------------------------------------------------------------
#define CircularBufferS8(BuffSize) struct{                                  \
	uint16_t    index;                  /* index */                         \
	int8_t      buff[BuffSize];         /* buffer */                        \
}

//------------------------------------------------------------------------------
// Macro:			CircularBufferItemSize()
// Description: 	
//! \brief          Get buffer item size
//! \param Buffer   CircularBuffer instance
//! \hideinitializer
//------------------------------------------------------------------------------
#define CircularBufferItemSize(Buffer) (sizeof(Buffer.buff[0]))

//------------------------------------------------------------------------------
// Macro:			CircularBufferSize()
// Description: 	
//! \brief          Get buffer size
//! \param Buffer   CircularBuffer instance
//! \hideinitializer
//------------------------------------------------------------------------------
#define CircularBufferSize(Buffer)                                          \
    ((sizeof(Buffer) - sizeof(uint16_t))/CircularBufferItemSize(Buffer))

//------------------------------------------------------------------------------
// Macro:			CircularBuffer_append()
// Description:
//! \brief          Append new value to circular buffer
//! \param Buffer   CircularBuffer instance
//! \param Signal   signal new sample
//! \hideinitializer
//------------------------------------------------------------------------------
#define CircularBuffer_append(Buffer, Signal)                               \
{                                                                           \
    /*---- set index -------------------------------------------*/          \
    if(++Buffer.index == CircularBufferSize(Buffer))                        \
		Buffer.index = 0;												    \
																			\
	Buffer.buff[Buffer.index] = Signal;								        \
}

//------------------------------------------------------------------------------
// Macro:			MMA()
// Description:
//! \brief          Modified moving average
//! \param MMA_buffer       MMA accumulator buffer
//! \param Signal           signal new sample
//! \param Scale            averaging interval as power of 2
//! \hideinitializer
//------------------------------------------------------------------------------
#define MMA(MMA_buffer, Signal, Scale)                                      \
    (((MMA_buffer = MMA_buffer - (MMA_buffer >> Scale) + (Signal))) >> Scale)    

//------------------------------------------------------------------------------
// Macro:			Integrator()
// Description:
//! \brief          Integrator with output scale
//! \param Buffer       CircularBuffer instance
//! \param Accumulator  Accumulator instance
//!
//! Example:    [buffer size = 128] [value = signalInput]
//!             CircularBuffer_append(buffer, value);
//!             Integrator(buffer, accumulator);
//!             value = accumulator>>7;
//! \hideinitializer
//------------------------------------------------------------------------------
#define Integrator(Buffer, Accumulator)				                        \
{																			\
    uint16_t last = Buffer.index + 1;                                       \
    if(last == CircularBufferSize(Buffer))                                  \
       last =  0;                                                           \
																			\
	Accumulator -= Buffer.buff[last];					                    \
	Accumulator += Buffer.buff[Buffer.index];							    \
}

//! Calculate fast root for 16-bit value
uint8_t fastRoot16(uint16_t x);

//! Calculate fast root for 32-bit value
uint16_t fastRoot32(uint32_t x);

//! @}
#endif // DSP_H
//******************************************************************************
// End of file
//******************************************************************************

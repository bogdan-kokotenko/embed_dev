//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_dma  MSP430F5x DMA APIs
//! \brief MSP430F5x DMA APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/dma.h
//! \brief  MSP430 DMA APIs
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------------------
//!  02/02/2015 | Bogdan Kokotenko | Initial draft
//!  07/02/2015 | Bogdan Kokotenko | Added USCIA1 APIs
//!  01/11/2015 | Bogdan Kokotenko | Improved DMA APIs for USCI
//!
//******************************************************************************
#ifndef DMA_H
#define DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#if ( defined (__MSP430F5438__)  \
   || defined (__MSP430F5438A__) \
   || defined (__MSP430F5329__)  \
   || defined (__MSP430F5529__) )
#define DMA_CHANNEL_NUM     3   //__MSP430_HAS_DMAX_3__
#elif ( defined (__MSP430F6638__) )
#define DMA_CHANNEL_NUM     6   //__MSP430_HAS_DMAX_6__
#else
#define DMA_CHANNEL_NUM 0
#endif

//! DMA USCI transmit trigger selection
typedef enum _DMA_TX_TRIGER{
    DMA_USCIA0TX,       //!< triggered by USCIA0 transmit
    DMA_USCIB0TX,       //!< triggered by USCIB0 transmit
    DMA_USCIA1TX,       //!< triggered by USCIA1 transmit
    DMA_USCIB1TX,       //!< triggered by USCIB1 transmit
}DMA_TX_TRIGER;

//! DMA USCI receive trigger selection
typedef enum _DMA_RX_TRIGER{
    DMA_USCIA0RX,       //!< triggered by USCIA0 receive
    DMA_USCIB0RX,       //!< triggered by USCIB0 receive
    DMA_USCIA1RX,       //!< triggered by USCIA1 receive
    DMA_USCIB1RX,       //!< triggered by USCIB1 receive
    DMA_ADC12IFG        //!< triggered by ADC12IFGx
}DMA_RX_TRIGER;

//! Stop DMA0
void DMA0_reset();

//! Check if DMA0 transfared any bytes
bool DMA0_isSizeChanged(uint16_t initial);

//! Fast memory copy by DMA0
bool DMA0_memcpy(void* dst, const void* src, uint16_t size);

//! Transfer data from ADC by DMA0
bool DMA0_ADC_transfer(void* dst, const void* adcReg, uint16_t size,
                       HANDLE handler);

//! Transmit data packet to USCI
bool DMA0_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler);

//! Receive data packet from USCI    
bool DMA0_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler);

//! Stop DMA1
void DMA1_reset();

//! Transmit data packet to USCI
bool DMA1_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler);

//! Receive data packet from USCI    
bool DMA1_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler);

//! Stop DMA2
void DMA2_reset();

//! Transmit data packet to USCI
bool DMA2_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler);

//! Receive data packet from USCI    
bool DMA2_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler);

#ifdef __cplusplus
}
#endif

#endif // DMA_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

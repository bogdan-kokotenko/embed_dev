//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/dma.c
//! \brief  MSP430F5x DMA functions implementation			
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  02/02/2015 | Bogdan Kokotenko | Initial draft
//!  07/02/2015 | Bogdan Kokotenko | Added DMA functions for USCIA1
//!  20/02/2015 | Bogdan Kokotenko | Added DMA functions for USCIA0 (SPI master)
//!  25/04/2015 | Bogdan Kokotenko | Fixed issue with DMA USCIA0
//!  29/06/2015 | Bogdan Kokotenko | Fixed LPM with DMA
//!  28/12/2015 | Bogdan Kokotenko | Fixed DMA settings options
//!
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "clocks.h"
#include "gpio.h"
#include "dma.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning DMA: Unknown MCU core, check HAL configuration!
#else

//! DMA transfer settings
typedef struct _DMA_Settings{
    uint8_t trigger;
    uint8_t *ptr;
}DMA_Settings;

//! DMA0 TX transfer settings
static const DMA_Settings DMA_txSettings[] = {
    [DMA_USCIA0TX] = {
        .trigger = DMA0TSEL__USCIA0TX,
        .ptr =  (uint8_t*)&UCA0TXBUF,
    },
    [DMA_USCIB0TX] = {
        .trigger = DMA0TSEL__USCIB0TX,
        .ptr =  (uint8_t*)&UCB0TXBUF,
    },
    [DMA_USCIA1TX] = {
        .trigger = DMA0TSEL__USCIA1TX,
        .ptr =  (uint8_t*)&UCA1TXBUF,
    },
    [DMA_USCIB1TX] = {
        .trigger = DMA0TSEL__USCIB1TX,
        .ptr =  (uint8_t*)&UCB1TXBUF,
    }
};

//! DMA0 TX transfer settings
static const DMA_Settings DMA_rxSettings[] = {
    [DMA_USCIA0RX] = {
        .trigger = DMA0TSEL__USCIA0RX,
        .ptr =  (uint8_t*)&UCA0RXBUF,
    },
    [DMA_USCIB0RX] = {
        .trigger = DMA0TSEL__USCIB0RX,
        .ptr =  (uint8_t*)&UCB0RXBUF,
    },
    [DMA_USCIA1RX] = {
        .trigger = DMA0TSEL__USCIA1RX,
        .ptr =  (uint8_t*)&UCA1RXBUF,
    },
    [DMA_USCIB1RX] = {
        .trigger = DMA0TSEL__USCIB1RX,
        .ptr =  (uint8_t*)&UCB1RXBUF,
    }
};

// DMA0 TX transfer settings
#define DMA0_TX_TRIGGER(Dst)    DMA_txSettings[Dst].trigger

// DMA0 RX transfer settings
#define DMA0_RX_TRIGGER(Src)    DMA_rxSettings[Src].trigger

// DMA1 TX transfer settings
#define DMA1_TX_TRIGGER(Dst)    (DMA_txSettings[Dst].trigger << 8)

// DMA1 RX transfer settings
#define DMA1_RX_TRIGGER(Src)    (DMA_rxSettings[Src].trigger << 8)

// DMA2 TX transfer settings
#define DMA2_TX_TRIGGER(Dst)    DMA_txSettings[Dst].trigger

// DMA2 RX transfer settings
#define DMA2_RX_TRIGGER(Src)    DMA_rxSettings[Src].trigger

// DMA channel handlers
HANDLE DMA_handler[DMA_CHANNEL_NUM];

//------------------------------------------------------------------------------
// Function:	
//				DMA0_reset()
// Description:
//! \brief      Stop DMA0
//------------------------------------------------------------------------------
void DMA0_reset()
{
    // disable DMA0
    DMA0CTL &= ~DMAEN;
    DMA0CTL &= ~DMAIFG;
    DMA0SZ = 0;
}

//------------------------------------------------------------------------------
// Function:	
//				DMA0_bytesLeft()
// Description:
//! \brief      Check if DMA0 transfared any bytes
//------------------------------------------------------------------------------
bool DMA0_isSizeChanged(uint16_t initial)
{
    return (DMA0SZ != initial);
}

//------------------------------------------------------------------------------
// Function:	
//              DMA0_memcpy()
// Description:
//! \brief      Fast memory copy by DMA0
//------------------------------------------------------------------------------
bool DMA0_memcpy(void* dst, const void* src, uint16_t size)
{
    IntFlag_t   intState;   // save interrupt flag while critical section

    // check DMA0 state
    if(DMA0CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA0 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA0 trigger 
    DMACTL0 &= ~DMA0TSEL_31;                // clear trigger
    DMACTL0 |= DMA0TSEL__DMA_REQ;           // software trigger

    // set DMA0 source address
    __data16_write_addr((uint16_t) &DMA0SA, (uint32_t) src);
    // set DMA0 destination address
    __data16_write_addr((uint16_t) &DMA0DA, (uint32_t) dst);
    // set data block size
    DMA0SZ = size;
    
    // set DMA0 mode
    DMA0CTL = DMADT_1 + DMASBDB +           // block transfare, byte to byte
              DMASRCINCR_3 + DMADSTINCR_3;  // src and dst increment

    // enable DMA0
    DMA0CTL |= DMAEN;
    
    LeaveCriticalSection(intState);         // leave critical section
    
    // trigger transfare
    DMA0CTL |= DMAREQ;
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA0_ADC_transfer()
// Description:
//! \brief      Transfer ADC result to the buffer by DMA0
//------------------------------------------------------------------------------
bool DMA0_ADC_transfer(void* dst, const void* adcReg, uint16_t size,
                       HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA0 state
    if(DMA0CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA0 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA0 trigger 
    DMACTL0 &= ~DMA0TSEL_31;            // clear trigger
    DMACTL0 |= DMA0TSEL__ADC12IFG;      // trigger on ADC conversion complete

    // set DMA0 source address
    __data16_write_addr((uint16_t) &DMA0SA,(uint32_t) adcReg);
    // set DMA0 destination address
    __data16_write_addr((uint16_t) &DMA0DA,(uint32_t) dst);
    // set data block size
    DMA0SZ = size;
    
    // set DMA0 mode
    DMA0CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMADSTINCR_3 + DMAIE;	    // dst increment, interrupt enable

    // set interrupt handler
    DMA_handler[0] = handler;
    
    // enable DMA0
    DMA0CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA0_USCI_write()
// Description:
//! \brief      Transmit data packet to USCI
//------------------------------------------------------------------------------
bool DMA0_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA0 state
    if(DMA0CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA0 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA0 trigger 
    DMACTL0 &= ~DMA0TSEL_31;                // clear trigger
    DMACTL0 |= DMA0_TX_TRIGGER(dst);        // trigger on USCI transmit

    // set DMA0 source address
    __data16_write_addr((uint16_t) &DMA0SA, (uint32_t) src);
    // set DMA0 destination address
    __data16_write_addr((uint16_t) &DMA0DA, (uint32_t) DMA_txSettings[dst].ptr);
    // set data block size
    DMA0SZ = size;
    
    // set DMA0 mode
    DMA0CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMASRCINCR_3 + DMAIE;     // src increment, interrupt enable

    // set interrupt handler
    DMA_handler[0] = handler;
    
    // enable DMA0
    DMA0CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA0_USCI_read()
// Description:
//! \brief      Receive data packet from USCI    
//------------------------------------------------------------------------------
bool DMA0_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA0 state
    if(DMA0CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA0 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA0 trigger 
    DMACTL0 &= ~DMA0TSEL_31;                // clear trigger
    DMACTL0 |= DMA0_RX_TRIGGER(src);        // trigger on USCI receive

    // set DMA0 source address
    __data16_write_addr((uint16_t) &DMA0SA,(uint32_t) DMA_rxSettings[src].ptr);
    // set DMA0 destination address
    __data16_write_addr((uint16_t) &DMA0DA,(uint32_t) dst);
    // set data block size
    DMA0SZ = size;
    
    // set DMA0 mode
    DMA0CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMADSTINCR_3 + DMAIE;	    // dst increment, interrupt enable

    // set interrupt handler
    DMA_handler[0] = handler;
    
    // enable DMA0
    DMA0CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//				DMA1_reset()
// Description:
//! \brief      Stop DMA1
//------------------------------------------------------------------------------
void DMA1_reset()
{
    // disable DMA1
    DMA1CTL &= ~DMAEN;
    DMA1CTL &= ~DMAIFG;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA1_USCI_write()
// Description:
//! \brief      Transmit data packet to USCI
//------------------------------------------------------------------------------
bool DMA1_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA1 state
    if(DMA1CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA1 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA1 trigger 
    DMACTL0 &= ~DMA1TSEL_31;                // clear trigger
    DMACTL0 |= DMA1_TX_TRIGGER(dst);        // trigger on USCI transmit

    // set DMA1 source address
    __data16_write_addr((uint16_t) &DMA1SA, (uint32_t) src);
    // set DMA1 destination address
    __data16_write_addr((uint16_t) &DMA1DA, (uint32_t) DMA_txSettings[dst].ptr);
    // set data block size
    DMA1SZ = size;
    
    // set DMA1 mode
    DMA1CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMASRCINCR_3 + DMAIE;     // src increment, interrupt enable

    // set interrupt handler
    DMA_handler[1] = handler;
    
    // enable DMA1
    DMA1CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA1_USCI_read()
// Description:
//! \brief      Receive data packet from USCI    
//------------------------------------------------------------------------------
bool DMA1_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA1 state
    if(DMA1CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA1 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA1 trigger 
    DMACTL0 &= ~DMA1TSEL_31;                // clear trigger
    DMACTL0 |= DMA1_RX_TRIGGER(src);        // trigger on USCI receive

    // set DMA1 source address
    __data16_write_addr((uint16_t) &DMA1SA,(uint32_t) DMA_rxSettings[src].ptr);
    // set DMA1 destination address
    __data16_write_addr((uint16_t) &DMA1DA,(uint32_t) dst);
    // set data block size
    DMA1SZ = size;
    
    // set DMA1 mode
    DMA1CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMADSTINCR_3 + DMAIE;	    // dst increment, interrupt enable

    // set interrupt handler
    DMA_handler[1] = handler;
    
    // enable DMA1
    DMA1CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//				DMA2_reset()
// Description:
//! \brief      Stop DMA2
//------------------------------------------------------------------------------
void DMA2_reset()
{
    // disable DMA2
    DMA2CTL &= ~DMAEN;
    DMA2CTL &= ~DMAIFG;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA2_USCI_write()
// Description:
//! \brief      Transmit data packet to USCI
//------------------------------------------------------------------------------
bool DMA2_USCI_write(DMA_TX_TRIGER dst, const void* src, uint16_t size,
                     HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA2 state
    if(DMA2CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA2 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA2 trigger 
    DMACTL1 &= ~DMA2TSEL_31;                // clear trigger
    DMACTL1 |= DMA2_TX_TRIGGER(dst);        // trigger on USCI transmit

    // set DMA2 source address
    __data16_write_addr((uint16_t) &DMA2SA, (uint32_t) src);
    // set DMA2 destination address
    __data16_write_addr((uint16_t) &DMA2DA, (uint32_t) DMA_txSettings[dst].ptr);
    // set data block size
    DMA2SZ = size;
    
    // set DMA2 mode
    DMA2CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMASRCINCR_3 + DMAIE;     // src increment, interrupt enable

    // set interrupt handler
    DMA_handler[2] = handler;
    
    // enable DMA2
    DMA2CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              DMA2_USCI_read()
// Description:
//! \brief      Receive data packet from USCI    
//------------------------------------------------------------------------------
bool DMA2_USCI_read(void* dst, DMA_RX_TRIGER src, uint16_t size,
                    HANDLE handler)
{
    IntFlag_t   intState;   // save interrupt flag while critical section
 
    // check DMA2 state
    if(DMA2CTL&DMAEN)
        return false;
 
    // avoid any interrupts while DMA2 settings is changed
    EnterCriticalSection(intState);
    
    // set DMA priorities
    DMACTL4 |= ROUNDROBIN;

    // set DMA2 trigger 
    DMACTL1 &= ~DMA2TSEL_31;                // clear trigger
    DMACTL1 |= DMA2_RX_TRIGGER(src);        // trigger on USCI receive

    // set DMA2 source address
    __data16_write_addr((uint16_t) &DMA2SA,(uint32_t) DMA_rxSettings[src].ptr);
    // set DMA2 destination address
    __data16_write_addr((uint16_t) &DMA2DA,(uint32_t) dst);
    // set data block size
    DMA2SZ = size;
    
    // set DMA2 mode
    DMA2CTL = DMADT_0 + DMASBDB +       // single transfare, byte to byte
              DMADSTINCR_3 + DMAIE;	    // dst increment, interrupt enable

    // set interrupt handler
    DMA_handler[2] = handler;
    
    // enable DMA2
    DMA2CTL |= DMAEN;
    
    LeaveCriticalSection(intState);     // leave critical section
    
    return true;
}

//------------------------------------------------------------------------------
// DMA Interrupt Service Routine
//------------------------------------------------------------------------------
#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void)
{
#if ( defined (__MSP430F5438__) || \
      defined (__MSP430F5438A__)|| \
      defined (__MSP430F6638__) || \
      defined (__MSP430F5329__))
    switch(__even_in_range(DMAIV,(12)))
    {
        case 0: break;
        case 2:                             // DMA0IFG = DMA Channel 0
            // disable DMA0
            DMA0CTL &= ~DMAEN;
            DMA0CTL &= ~DMAIFG;
            // call DMA0 handler
            if(DMA_handler[0])
                DMA_handler[0](); 
            break;                      
        case 4:                             // DMA1IFG = DMA Channel 1
            // disable DMA1
            DMA1CTL &= ~DMAEN;
            DMA1CTL &= ~DMAIFG;
            // call DMA1 handler
            if(DMA_handler[1])
                DMA_handler[1](); 
            break;   
        case 6:                             // DMA2IFG = DMA Channel 2
            // disable DMA2
            DMA2CTL &= ~DMAEN;
            DMA2CTL &= ~DMAIFG;
            // call DMA2 handler
            if(DMA_handler[2])
                DMA_handler[2](); 
            break;
#if ( defined (__MSP430F6638__) )
        case 8:                             // DMA3IFG = DMA Channel 3
            // disable DMA3
            DMA3CTL &= ~DMAEN;
            // call DMA3 handler
            if(DMA_handler[3])
                DMA_handler[3](); 
            break;
        case 10:                            // DMA4IFG = DMA Channel 4
            // disable DMA4
            DMA4CTL &= ~DMAEN;
            // call DMA4 handler
            if(DMA_handler[4])
                DMA_handler[4](); 
            break;
        case 12:                            // DMA5IFG = DMA Channel 5
            // disable DMA5
            DMA5CTL &= ~DMAEN;
            // call DMA5 handler
            if(DMA_handler[5])
                DMA_handler[5](); 
            break;
#endif
        default: break;
    }
#endif
    
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                      // Wake-up MCU
    #endif
}

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file
//******************************************************************************

//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//	File description:
//! \file   hal/mcu/msp430g2x/clocks.c
//! \brief  MSP430G2x clock sources handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//!  25/09/2015 | Bogdan Kokotenko | Fixed issue with unstable 32.768kHz crystal
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning CLOCKS: Unknown MCU core, check HAL configuration!
#else

// Local function prototypes section

//! Initialize digital clock oscillator (DCO)
void DCO_init(void);
//! Initialize XT1 external clock oscillator
void XT1_init(void);

//! Initialize master (CPU and system) clock
void MCLK_init(void);
//! Initialize sub-main (peripherals) clock
void SMCLK_init(void);
//! Initialize auxilary (peripherals) clock
void ACLK_init(void);

//------------------------------------------------------------------------------
// Function:	
//              CLK_init()
// Description:
//! \brief      Initialize system clocks
//------------------------------------------------------------------------------
void CLK_init(void)
{
    // Initialize CLOCKs
    XT1_init();
    DCO_init();
    
    MCLK_init();
    SMCLK_init();
    ACLK_init();
}

//------------------------------------------------------------------------------
// Function:	
//              DCO_init()
// Description:
//! \brief      Initialize digital clock oscillator (DCO)
//------------------------------------------------------------------------------
void DCO_init(void)
{
#if (DCO_FREQ == 1000000)
    BCSCTL1 = CALBC1_1MHZ;              // Set range
    DCOCTL = CALDCO_1MHZ;               // Set DCO step + modulation

#elif (DCO_FREQ == 8000000)
    BCSCTL1 = CALBC1_8MHZ;              // Set range
    DCOCTL = CALDCO_8MHZ;               // Set DCO step + modulation

#elif (DCO_FREQ == 12000000)
    BCSCTL1 = CALBC1_12MHZ;             // Set range
    DCOCTL = CALDCO_12MHZ;              // Set DCO step + modulation

#elif (DCO_FREQ == 16000000)
    BCSCTL1 = CALBC1_16MHZ;             // Set range
    DCOCTL = CALDCO_16MHZ;              // Set DCO step + modulation
#else
    #error CLOCKS: Unsupported DCO frequency   
#endif   
}

//------------------------------------------------------------------------------
// Function:	
//              XT1_init()
// Description:
//! \brief      Initialize XT1 external clock oscillator
//------------------------------------------------------------------------------
void XT1_init(void)
{
#ifdef XT1_FREQ         // Is XT1 avaliable ?
    __bic_SR_register(OSCOFF);          // XT On
#if (XT1_FREQ == 32768)                 // 32768Hz
    BCSCTL1 &= ~(XTS);
    BCSCTL3 |= XCAP_3;                  // Add XIN/XOUT Cap : 12.5 pF 
#else
    #error CLOCKS: Unsupported XT1 frequency
#endif

    // Test OFIFG until XT1 is stable 
    do{        
        // Clear OFIFG
        IFG1 &= ~OFIFG;
   
        // Wait 50 us
        __delay_cycles(50);

    }while(IFG1&OFIFG);
    
    // Enable oscillator fault interrupt
    IE1 |= OFIE;

#endif // XT1_FREQ
}

//------------------------------------------------------------------------------
// Function:	
//              MCLK_init()
// Description:
//! \brief      Initialize master (CPU and system) clock
//------------------------------------------------------------------------------
void MCLK_init(void)
{
#if (MCLK_FREQ == XT1_FREQ)
    // Use the XT1 oscillator to source the MCLK
    BCSCTL2 |= SELM_3;
   
#elif (MCLK_FREQ == XT2_FREQ)
    // Use the XT2 oscillator to source the MCLK
    BCSCTL2 |= SELM_2;

#elif (MCLK_FREQ == DCO_FREQ)
    // Use the DCO oscillator to source the MCLK
    BCSCTL2 |= SELM_0;
#else
    #error CLOCKS: Wrong MCLK selection
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              SMCLK_init()
// Description:
//! \brief      Initialize sub-main (peripherals) clock
//------------------------------------------------------------------------------
void SMCLK_init(void)
{
    // Select SMCLK source
#if (SMCLK_FREQ == XT1_FREQ)
    BCSCTL2 |= SELS;                // SMCLK <- XT1
#elif (SMCLK_FREQ == XT1_FREQ/2)
    BCSCTL2 |= SELS;                // SMCLK <- XT1
    BCSCTL2 |= DIVS_1;              // SMCLK/2
#elif (SMCLK_FREQ == XT1_FREQ/4)
    BCSCTL2 |= SELS;                // SMCLK <- XT1
    BCSCTL2 |= DIVS_2;              // SMCLK/4
#elif (SMCLK_FREQ == XT1_FREQ/8)
    BCSCTL2 |= SELS;                // SMCLK <- XT1
    BCSCTL2 |= DIVS_3;              // SMCLK/8
#elif (SMCLK_FREQ == DCO_FREQ)
    BCSCTL2 &= SELS;                // SMCLK <- DCO
#elif (SMCLK_FREQ == (DCO_FREQ/2))
    BCSCTL2 &= SELS;                // SMCLK <- DCO
    BCSCTL2 |= DIVS_1;              // SMCLK/2
#elif (SMCLK_FREQ == (DCO_FREQ/4))
    BCSCTL2 &= SELS;                // SMCLK <- DCO
    BCSCTL2 |= DIVS_2;              // SMCLK/4
#elif (SMCLK_FREQ == (DCO_FREQ/8))
    BCSCTL2 &= SELS;                // SMCLK <- DCO
    BCSCTL2 |= DIVS_3;              // SMCLK/8
#else
    #error CLOCKS: Wrong SMCLK selection
#endif

    // Set SMCLK to output  
    #ifdef SMCLK_OUT
        SetPinOut(1,4);             // P1.4 <- output
        SelectPinFunc2(1,4);        // P1.4 <- SMCLK
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//              ACLK_init()
// Description:
//! \brief      Initialize auxilary (peripherals) clock
//------------------------------------------------------------------------------
void ACLK_init(void)
{
    // Select ACLK frequency
#if (ACLK_FREQ == XT1_FREQ)
    BCSCTL1 |= DIVA_0;
#elif (ACLK_FREQ == XT1_FREQ/2)
    BCSCTL1 |= DIVA_1;
#elif (ACLK_FREQ == XT1_FREQ/4)
    BCSCTL1 |= DIVA_2;
#elif (ACLK_FREQ == XT1_FREQ/8)
    BCSCTL1 |= DIVA_3;
#elif (ACLK_FREQ == VLO_FREQ)
    BCSCTL3 |= LFXT1S_2;                // ACLK = VLO
#else
    #error CLOCKS: Wrong ACLK selection
#endif
}

//------------------------------------------------------------------------------
// Function:	
//              NMI_isr()
// Description:
//! \brief      Nonmaskable interrupts ISR
//------------------------------------------------------------------------------
#pragma vector = NMI_VECTOR
__interrupt void NMI_isr (void)
{
    uint16_t timeout = 1000;
    do
    {
        IFG1 &= ~OFIFG;                     // Clear OSCFault flag
        __delay_cycles(50);                 // Time for flag to set
        
        if(--timeout == 0)
            MCU_reset();                    // Force MCU reset
    }
    while (IFG1 & OFIFG);                   // OSCFault flag still set?
}

#endif // _MSP430G2X_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

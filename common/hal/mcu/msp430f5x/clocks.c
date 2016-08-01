//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/clocks.c
//! \brief  MSP430F5x clock sources handling functions
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  17/01/2015 | Bogdan Kokotenko | Initial draft
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "HAL_UCS.h"
#include "HAL_PMM.h"
#include "clocks.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning CLOCK: Unknown MCU core, check HAL configuration!
#else

// Local function prototypes section

//! Initialize digital clock oscillator (DCO)
void DCO_init(void);
//! Initialize XT1 external clock oscillator
void XT1_init(void);
//! Initialize XT2 external clock oscillator
void XT2_init(void);

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
    // Core high frequency requires the VCore set to 1.85V
    SetVCore(PMMCOREV_3);
    
    // Initialize CMOS Oscillator
    #ifdef DSC11x1_CLK
    DSC11x1_init();
    DSC11x1_enable();
    #endif // DSC11x1_CLK
    
    // Initialize external CLOCKs
    XT1_init();
    XT2_init();
    
    // Initialize system clock sources
    MCLK_init();
    SMCLK_init();
    ACLK_init();

    // Initialize internal CLOCKs
    DCO_init();
    
    // Initialize clock fault interrupt
    SFRIFG1 &= ~OFIFG;
    SFRIE1 |= OFIE;
}

//------------------------------------------------------------------------------
// Function:	
//              DCO_init()
// Description:
//! \brief      Initialize digital clock oscillator (DCO)
//------------------------------------------------------------------------------
void DCO_init(void)
{
#ifdef DCO_FREQ
  
#if defined (XT1_FREQ)
    // Use the XT1 oscillator to source the FLL
    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__XT1CLK); 
    
    // Start the FLL, at the freq indicated by the config constant DCO_FREQ
    //Init_FLL((DCO_FREQ/1000), (DCO_FREQ/XT1_FREQ));
    Init_FLL(((DCO_FREQ/2)/1000), ((DCO_FREQ/2)/XT1_FREQ));
    
#elif defined (XT2_FREQ)

    // Use the XT2 oscillator to source the FLL
    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__XT2CLK);
    
    // Start the FLL, at the freq indicated by the config constant DCO_FREQ
    Init_FLL(((DCO_FREQ/2)/1000), ((DCO_FREQ/2)/XT2_FREQ));

#else  
    // Use the REFO oscillator to source the FLL
    UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
    
    // Start the FLL, at the freq indicated by the config constant DCO_FREQ
    Init_FLL((DCO_FREQ/1000), (DCO_FREQ/REFO_FREQ));
#endif  //  XT1_FREQ/XT2_FREQ/REFO

#else
    // DCO frequency control
    UCSCTL0 = 0x0900;                   // DCO = 9
    UCSCTL1 = 0x0050 | DISMOD;          // low freq DCO, modulation disabled
    _BIS_SR(SCG0 + SCG1);               // written to disable DCO
#endif  // DCO_FREQ
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
 
#if ( defined (__MSP430F552x)  \
   || defined (__MSP430F550x)  \
   || defined (__MSP430F5438__)\
   || defined (__MSP430F5438A__))
        SelectPinFunc2(7,0);                    // P7.0 -> XT1IN
    #ifndef DSC11x1_CLK // ext. oscillator
        SelectPinFunc2(7,1);                    // P7.1 -> XT1OUT
    #endif // DSC11x1_CLK
#elif ( defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
        SelectPinFunc2(5,4);                    // P5.4 -> XT1IN
    #ifndef DSC11x1_CLK // ext. oscillator
        SelectPinFunc2(5,5);                    // P5.5 -> XT1OUT
    #endif // DSC11x1_CLK
#endif

#if ( defined (__MSP430F6638__) )        
     while(BAKCTL & LOCKIO)                    // Unlock XT1 pins for operation
     BAKCTL &= ~(LOCKIO); 
#endif
   
#ifdef DSC11x1_CLK // ext. oscillator
    XT1_Bypass();
#else   // select crystal
    #if (XT1_FREQ == 32768)                                 // 32768Hz
        LFXT_Start(XT1DRIVE_0);
    #elif ((4000000 <= XT1_FREQ) && (XT1_FREQ < 8000000) )  // 4~8MHz
        LFXT_Start(XT1DRIVE_2);
    #elif ( (8000000 <= XT1_FREQ) && (XT1_FREQ < 16000000) ) // 8~16MHz
        XT1_Start(XT1DRIVE_1);
    #elif ( (16000000 <= XT1_FREQ) && (XT1_FREQ < 24000000) )// 16~24MHz
        XT1_Start(XT1DRIVE_2);
    #elif ( (24000000 <= XT1_FREQ) && (XT1_FREQ <= 32000000) )// 24~32MHz
        XT1_Start(XT1DRIVE_3);
    #else
        #error CLOCKS: Unsupported XT1 frequency
    #endif
#endif // DSC11x1_CLK

#endif // XT1_FREQ
}

//------------------------------------------------------------------------------
// Function:	
//              XT2_init()
// Description:
//! \brief      Initialize XT2 external clock oscillator
//------------------------------------------------------------------------------
void XT2_init(void)
{
#ifdef XT2_FREQ         // Is XT2 avaliable ?
 
#if ( defined (__MSP430F552x)  \
   || defined (__MSP430F550x)  \
   || defined (__MSP430F5438__)\
   || defined (__MSP430F5438A__) \
   || defined (__MSP430F5329__) )
        SelectPinFunc2(5,2);                    // P5.2 -> XT2IN
        SelectPinFunc2(5,3);                    // P5.3 -> XT2OUT
#elif ( defined (__MSP430F563x_F663x) \
   ||   defined (__MSP430F6638__) )
        SelectPinFunc2(7,2);                    // P7.2 -> XT2IN
        SelectPinFunc2(7,3);                    // P7.3 -> XT2OUT
#endif

#if ( (4000000 <= XT2_FREQ) && (XT2_FREQ < 8000000) )    // 4~8Mhz
    XT2_Start(XT2DRIVE_0);
#elif ( (8000000 <= XT2_FREQ) && (XT2_FREQ < 16000000) ) // 8~16Mhz
    XT2_Start(XT2DRIVE_1);
#elif ( (16000000 <= XT2_FREQ) && (XT2_FREQ < 24000000) )// 16~24Mhz
    XT2_Start(XT2DRIVE_2);
#elif ( (24000000 <= XT2_FREQ) && (XT2_FREQ <= 32000000) )// 24~32Mhz
    XT2_Start(XT2DRIVE_3);
#else
    #error CLOCKS: Unsupported XT2 frequency
#endif
   
#endif // XT2_FREQ
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
    UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__XT1CLK);
   
#elif (MCLK_FREQ == XT2_FREQ)
    // Use the XT2 oscillator to source the MCLK
    UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__XT2CLK);

#elif (MCLK_FREQ == DCO_FREQ)
    // Use the DCO oscillator to source the MCLK
    UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__DCOCLK);
    
#elif (MCLK_FREQ == REFO_FREQ)
    // Use the REFO oscillator to source the MCLK
    UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELSM__REFOCLK);
    
#else
    #error CLOCKS: Wrong MCLK selection
#endif
    
    // Set MCLK to output  
#if ( defined (__MSP430F5438A__) )
    //SetPinOut(2,0);                   // P2.0 <- output
    //SelectPinFunc2(2,0);              // P2.0 <- MCLK
#elif ( defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
    //SetPinOut(7,7);                   // P7.7 <- output
    //SelectPinFunc2(7,7);              // P7.7 <- MCLK
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
#if defined (SMCLK_FREQ)
    
    // Select SMCLK source
#if (SMCLK_FREQ == XT1_FREQ)
    // Use the XT1 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT1CLK);
#elif (SMCLK_FREQ == (XT1_FREQ/2))
    // Use the XT1 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__2); // SMCLK/2
#elif (SMCLK_FREQ == (XT1_FREQ/4))
    // Use the XT1 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__4); // SMCLK/4
#elif (SMCLK_FREQ == (XT1_FREQ/8))
    // Use the XT1 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__8); // SMCLK/8
    
#elif (SMCLK_FREQ == XT2_FREQ)
    // Use the XT2 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK);
#elif (SMCLK_FREQ == (XT2_FREQ/2))
    // Use the XT2 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__2); // SMCLK/2
#elif (SMCLK_FREQ == (XT2_FREQ/4))
    // Use the XT2 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__4); // SMCLK/4
#elif (SMCLK_FREQ == (XT2_FREQ/8))
    // Use the XT2 oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__8); // SMCLK/8
    
#elif (SMCLK_FREQ == DCO_FREQ)
    // Use the DCO oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__DCOCLK);
#elif (SMCLK_FREQ == (DCO_FREQ/2))
    // Use the DCO oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__DCOCLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__2); // SMCLK/2
#elif (SMCLK_FREQ == (DCO_FREQ/4))
    // Use the DCO oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__DCOCLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__4); // SMCLK/4
    
#elif (SMCLK_FREQ == REFO_FREQ)
    // Use the REFO oscillator to source the SMCLK
    UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__REFOCLK);
    
#else
    #error CLOCKS: Wrong SMCLK selection
#endif
    
    // SMCLK divider
    //UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__2); // SMCLK/2

    // Set SMCLK to output  
#if ( defined (__MSP430F552x)  \
   || defined (__MSP430F550x)  \
   || defined (__MSP430F5438__)\
   || defined (__MSP430F5438A__))
    //SetPinOut(1,6);                   // P1.6 <- output
    //SelectPinFunc2(1,6);              // P1.6 <- SMCLK
    //SetPinOut(4,7);                   // P4.7 <- output
    //SelectPinFunc2(4,7);              // P4.7 <- SMCLK
    //SetPinOut(11,2);                  // P11.2 <- output
    //SelectPinFunc2(11,2);             // P11.2 <- SMCLK
#elif ( defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
    //SetPinOut(2,2);                     // P2.2 <- output
    //SelectPinFunc2(2,2);                // P2.2 <- SMCLK
#elif ( defined (__MSP430F563x_F663x) \
   ||   defined (__MSP430F6638__) )
    //SetPinOut(3,4);                     // P3.4 <- output
    //SelectPinFunc2(3,4);                // P3.4 <- SMCLK
#endif

#else
        #error CLOCKS: Wrong SMCLK selection
#endif // SMCLK_FREQ
}

//------------------------------------------------------------------------------
// Function:	
//              ACLK_init()
// Description:
//! \brief      Initialize auxilary (peripherals) clock
//------------------------------------------------------------------------------
void ACLK_init(void)
{
#if defined (ACLK_FREQ)
    
    // Select ACLK source
#if (ACLK_FREQ == XT1_FREQ)
    // Use the XT1 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT1CLK);
#elif (ACLK_FREQ == (XT1_FREQ/2))
    // Use the XT1 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__2);
#elif (ACLK_FREQ == (XT1_FREQ/4))
    // Use the XT1 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__4); 
#elif (ACLK_FREQ == (XT1_FREQ/8))
    // Use the XT1 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT1CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__8); 
    
#elif (ACLK_FREQ == XT2_FREQ)
    // Use the XT2 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT2CLK);
#elif (ACLK_FREQ == (XT2_FREQ/2))
    // Use the XT2 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__2); 
#elif (ACLK_FREQ == (XT2_FREQ/4))
    // Use the XT2 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__4); 
#elif (ACLK_FREQ == (XT2_FREQ/8))
    // Use the XT2 oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT2CLK);
    UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVA__8); 
     
#elif (ACLK_FREQ == REFO_FREQ)
    // Use the REFO oscillator to source the ACLK
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

#else
    #error CLOCKS: Wrong ACLK selection
#endif
    
    // Set ACLK to output  
#if ( defined (__MSP430F552x)  \
   || defined (__MSP430F550x)  \
   || defined (__MSP430F5438__)\
   || defined (__MSP430F5438A__))
    //SetPinOut(1,0);                   // P1.0 <- output
    //SelectPinFunc2(1,0);              // P1.0 <- ACLK
    //SetPinOut(2,6);                   // P2.6 <- output
    //SelectPinFunc2(2,6);              // P2.6 <- ACLK
    //SetPinOut(11,0);                  // P11.0 <- output
    //SelectPinFunc2(11,0);             // P11.0 <- ACLK
    
#elif ( defined (__MSP430F563x_F663x) \
    ||  defined (__MSP430F6638__)    \
    ||  defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
    //SetPinOut(1,0);                   // P1.0 <- output
    //SelectPinFunc2(1,0);              // P1.0 <- ACLK
    
#endif
    
    // ACLK external divider
    //UCSCTL5 = (UCSCTL5 & ~(DIVPA_7)) | (DIVPA__32); // out <- ACLK/32

#else
        #error CLOCKS: Wrong ACLK selection
#endif // ACLK_FREQ
}

//------------------------------------------------------------------------------
// Function:	
//              UNMI_isr()
// Description:
//! \brief      User nonmaskable interrupts ISR
//------------------------------------------------------------------------------
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_isr (void)
{
    static uint16_t OF_timeout = 100;
#if ( defined (__MSP430F5438__) \
   || defined (__MSP430F5438A__) )
    switch (__even_in_range(SYSUNIV, SYSUNIV_SYSBERRIV))
#elif ( defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
    switch (__even_in_range(SYSUNIV, SYSUNIV_SYSBUSIV))
#elif ( defined (__MSP430F6638__) )
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
#endif
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:                 //Clear OSC flaut Flags fault flags
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); 
            SFRIFG1 &= ~OFIFG;
            if(--OF_timeout == 0)
                MCU_reset();
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
#if ( defined (__MSP430F5438__) ||  \
      defined (__MSP430F5438A__) )
        case SYSUNIV_SYSBERRIV:
#elif ( defined (__MSP430F5329__) \
    ||  defined (__MSP430F5529__) )
        case SYSUNIV_SYSBUSIV:
#elif ( defined (__MSP430F6638__) )
        case SYSUNIV_BUSIFG:
#endif
           __no_operation();
    }
}

#endif // _MSP430F5x_HAL_

//******************************************************************************
// End of file.
//******************************************************************************

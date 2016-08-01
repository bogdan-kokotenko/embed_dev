//******************************************************************************
//  Copyright (C) 2016 Bogdan Kokotenko
//
//! \addtogroup msp430f5x_hal
//! @{
//! \defgroup msp430f5x_rtc  MSP430F5x RTC APIs
//! \brief MSP430F5x RTC APIs
//! @{
//******************************************************************************
//	File description:
//! \file   hal/mcu/msp430f5x/rtc.h
//! \brief  MSP430 RTC APIs
//!      			
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ----------------
//!  08/10/2015 | Bogdan Kokotenko | Initial draft
//!
//******************************************************************************
#ifndef RTC_H
#define RTC_H

#ifdef __cplusplus
extern "C" {
#endif

//! RTC clock frequency
#define RTC_CLK_FREQ            SMCLK_FREQ

//! RTC clock divider
#define RTC_CLK_DIV             64

//! RTC mode
#define RTC_COUNTER
//#define RTC_CALENDAR

//! RTC initialization
void RTC_init(void);

//! RTC reset
void RTC_reset(void);

//! RTC get time
uint32_t RTC_getTicks(void);

// Callbacks section

//! RTC handler
//#define RTC_Handler()

#ifdef __cplusplus
}
#endif

#endif // RTC_H
//! @}
//! @}
//******************************************************************************
// End of file
//******************************************************************************

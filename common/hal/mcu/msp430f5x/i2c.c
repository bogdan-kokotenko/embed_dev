//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//   File description:
//! \file   hal/mcu/msp430f5x/i2c.c
//! \brief  MSP430F5x I2C functions implementation.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  11/05/2015 | Artem Porkhun    | Initial draft
//!  19/05/2015 | Bogdan Kokotenko | Code refactoring.
//!  21/05/2015 | Bogdan Kokotenko | Code refactoring. Stable reset option.
//!  29/06/2015 | Bogdan Kokotenko | Added ISR based implementation.
//!  18/08/2015 | Bogdan Kokotenko | I2C NACK checking fixed.
//!  28/08/2015 | Bogdan Kokotenko | Added advanced handler caller option.
//!  28/08/2015 | Bogdan Kokotenko | Fixed autorestart for rx-one-byte mode.
//!  13/01/2016 | Bogdan Kokotenko | Added request size otput for slave.
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "task.h"
#include "i2c.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430F5x_HAL_) )
#warning I2C: Unknown MCU core, check HAL configuration!
#else

//! I2C receive/transmit contexts
struct I2C_Context_t{
    struct _rx_tx{
    uint8_t*    bufferPtr;          //!< data pointer
    uint16_t    size;               //!< data size
    }rx, tx;                        //!< rx/tx items
    uint16_t    index;              //!< data index 
    bool autoRestart;               //!< I2C autorestart for transmit/receive
    bool errorFlag;                 //!< I2C error flag
    void (*handler)(void);          //! I2C receive/transmit handler
#if ( defined(I2C0_SLAVE) || defined(I2C1_SLAVE))
    uint16_t*   rxSizePtr;          //!< pointer to rx size output
#endif
}I2C0_context, I2C1_context;

//! Reset I2C0 bus
static bool I2C0_resetBus(void);

//! Reset I2C1 bus
static bool I2C1_resetBus(void);

// Check selected I2CI0 mode
#if ( defined(I2C0_MASTER) && defined(I2C0_SLAVE))
    #error I2C: Select proper I2C0 mode!
#elif ( defined(I2C0_MASTER) || defined(I2C0_SLAVE))

//------------------------------------------------------------------------------
// Function:	
//              I2C0_init()
// Description:
//! \brief      Initialize I2C
//------------------------------------------------------------------------------
bool I2C0_init(void)
{
    if(I2C0_resetBus() == false)  // Master resets bus if slave hangs SDA
        return false;             // Slave always returns SUCCESS
    
    // Initialize SDA, SCL
    GPIO_func2(I2C0_SDA_PIN);
    GPIO_func2(I2C0_SCL_PIN);

    I2C0_REG(CTL1) |= UCSWRST;                  // Enable SW reset
    I2C0_REG(CTL0) = UCMODE_3 + UCSYNC;         // Synchronous mode
    #ifdef I2C0_MASTER
        I2C0_REG(CTL0) |= UCMST;                // I2C Master
    #endif
  
    #if (I2C0_CLK == SMCLK_FREQ)
        I2C0_REG(CTL1) |= UCSSEL__SMCLK;        // I2C0_CLK = SMCLK
    #elif (I2C0_CLK == ACLK_FREQ)
        I2C0_REG(CTL1) |= UCSSEL__ACLK;         // I2C0_CLK = ACLK
    #else
        #error I2C: Unsupported I2C0 clock frequency
    #endif

    #if ((I2C0_BAUDRATE) < 1000 || (I2C0_BAUDRATE) > 1000000L)
        #error I2C: Unsupported I2C0 baudrate
    #else
        I2C0_REG(BR0) = (((I2C0_CLK/I2C0_BAUDRATE) - 1)&0xFF);
        I2C0_REG(BR1) = (((I2C0_CLK/I2C0_BAUDRATE) - 1)>>8)&0xFF;
    #endif
    
    I2C0_REG(I2COA) =  I2C0_OWN_ADDRESS;
    I2C0_REG(CTL1) &= ~(UCTXSTP+UCTXSTT);
    I2C0_REG(CTL1) &= ~UCSWRST;             // Clear SW reset, resume operation

    #ifdef I2C0_SLAVE
    I2C0_REG(I2COA) |=  UCGCEN;             // Respond to a general call
    I2C0_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG+UCSTPIFG);
    I2C0_REG(IE) |= (UCRXIE+UCTXIE+UCNACKIE+UCSTPIE);// Enable TX&NACK interrupt
    #endif
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              I2C0_resetBus()
// Description:
//! \brief      Reset I2C0 bus
//------------------------------------------------------------------------------
bool I2C0_resetBus(void)
{
    #ifdef I2C0_MASTER

    IntFlag_t intState;
    EnterCriticalSection(intState);
    
    I2C0_REG(CTL1) |= UCTXSTP;          // Send stop condition
    __delay_us((1000000L/I2C0_BAUDRATE) + 1);
    I2C0_REG(CTL1) |= UCSWRST;          // Enable SW reset
    
    // Reset I2C if slave hang the line
    GPIO_func1(I2C0_SDA_PIN);           // SDA -> GPIO
    GPIO_setIn(I2C0_SDA_PIN);           // SDA -> input
    
    GPIO_setHigh(I2C0_SCL_PIN);         // SCL <- high    
    GPIO_func1(I2C0_SCL_PIN);           // SCL <- GPIO
    GPIO_setOut(I2C0_SCL_PIN);          // SCL <- output
    
    // Send nine clock pulses to release bus by slave
    uint8_t counter = 9; // (one byte + ack bit)
    while(counter--) 
    {
        if(GPIO_test(I2C0_SDA_PIN))    // Check SDA line
        {
            LeaveCriticalSection(intState);
            return true;             
        }

        GPIO_setLow(I2C0_SCL_PIN);      // SCL <- low
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);
        GPIO_setHigh(I2C0_SCL_PIN);     // SCL <- high
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);
    }
                
    LeaveCriticalSection(intState);
    return false;                     // unrecoverable bus error
    
    #else
        return SUCCESS;
    #endif

}

//------------------------------------------------------------------------------
// Function:	
//                      I2C0_isBusy()
// Description:
//! \brief              Check I2C0 state
//------------------------------------------------------------------------------
bool I2C0_isBusy(void)
{
    return (I2C0_REG(STAT) & UCBBUSY) ? true : false; // check busy flag
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C0_error()
// Description:
//! \brief              Check I2C0 error
//------------------------------------------------------------------------------
bool I2C0_error(void)
{
    return (I2C0_context.errorFlag || (I2C0_REG(IFG)&UCALIFG));
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C0_setOwnAddress()
// Description:
//! \brief              Set I2C0 own address
//------------------------------------------------------------------------------
void I2C0_setOwnAddress(uint8_t address)
{
    I2C0_REG(I2COA) &=  0xFF00;
    I2C0_REG(I2COA) |=  address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C0_selectSlaveAddress()
// Description:
//! \brief              Select I2C0 slave address 
//------------------------------------------------------------------------------
void I2C0_selectSlaveAddress(uint8_t address)
{
    I2C0_REG(I2CSA) = address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C0_txByte()
// Description:
//! \brief              Transmit byte via I2C0
//------------------------------------------------------------------------------
void I2C0_txByte(uint8_t dataByte)
{
    uint16_t counter;
    
    I2C0_context.errorFlag = false;             // Clear error flag
    
    I2C0_REG(IE) &= ~(UCTXIE+UCRXIE+UCNACKIE);  // Disable interrupts
    I2C0_REG(IFG)&= ~(UCTXIFG+UCRXIFG+UCNACKIFG);

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C0_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C0_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
	I2C0_REG(CTL1) |= UCTR;                     // Set Master transmitter
    I2C0_REG(CTL1) |= UCTXSTT;                  // Send start condition

    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          !(I2C0_REG(IFG) & (UCTXIFG+UCNACKIFG)))
        __delay_us((1000000L/I2C0_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C0_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
    if(I2C0_REG(IFG) & UCNACKIFG)
    {
        I2C0_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C0_context.errorFlag = true;          // I2C NACK error
        return;
    }

    I2C0_REG(TXBUF) = dataByte;
    
    counter = 9;                                //(data byte + ack bit)
    while(counter-- &&                          // Wait data byte is shifted out
          !(I2C0_REG(IFG) & (UCTXIFG+UCNACKIFG)))
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);
    if(!counter)
    {
        I2C0_context.errorFlag = true;          // I2C timeout error
        return;
    }

    if(I2C0_REG(IFG) & UCNACKIFG)
    {
        I2C0_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C0_context.errorFlag = true;          // I2C NACK error
        return;
    }
    
    I2C0_REG(CTL1) |= UCTXSTP;                  // Send stop condition
    I2C0_REG(IFG)  &= ~(UCTXIFG+UCNACKIFG);
}

//------------------------------------------------------------------------------
// Function:	
//                    I2C0_rxByte()
// Description:
//! \brief            Receive byte via I2C
//------------------------------------------------------------------------------
uint8_t I2C0_rxByte(void)
{
    uint16_t counter;
    
    I2C0_context.errorFlag = false;             // clear error flag
    
    I2C0_REG(IE) &= ~(UCTXIE+UCRXIE+UCNACKIE);  // Disable interrupts
    I2C0_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG);

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C0_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C0_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C0_REG(CTL1) |= UCTXSTT;              // Send start condition
    
    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          I2C0_REG(CTL1) & UCTXSTT)
        __delay_us((1000000L/I2C0_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C0_REG(CTL1) |= UCTXSTP;              // Send stop immediately 
                                            // after byte received
    counter = 9;
    while(counter-- &&                      // Wait data byte is shifted in
          !(I2C0_REG(IFG) & (UCRXIFG)))
        __delay_us((1000000L/I2C0_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }

    return I2C0_REG(RXBUF);                  // return data byte
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C_transmit()
// Description:
//! \brief          Transmit byte array via I2C
//------------------------------------------------------------------------------
#ifdef I2C0_MASTER
void I2C0_transmit(uint8_t address, 
                   const void* data, uint16_t size, void (*handler)(void))
#else
void I2C0_transmit(const void* data, uint16_t size, void (*handler)(void))
#endif
{
    #ifdef I2C0_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C0_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C0_REG(I2CSA) = address;
    #endif
    
    I2C0_context.errorFlag = false;         // clear error flag
    
    I2C0_context.tx.bufferPtr = (uint8_t*)data;
    I2C0_context.tx.size = size;
    I2C0_context.index = 0;
    I2C0_context.handler = handler;
    
    I2C0_context.autoRestart = false;
    
    #ifdef I2C0_MASTER
    I2C0_REG(IE) |= (UCTXIE + UCNACKIE);    // Enable TX&NACK interrupt
    I2C0_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C0_REG(CTL1) |= UCTXSTT;              // Send start condition
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C0_receive()
// Description:
//! \brief          Receive byte array via I2C
//------------------------------------------------------------------------------
#ifdef I2C0_MASTER
void I2C0_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void))
#else
void I2C0_receive(void* data, uint16_t* size, void (*handler)(void))
#endif
{
    #ifdef I2C0_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C0_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C0_REG(I2CSA) = address;
    #endif
    
    I2C0_context.errorFlag = false;         // Clear error flag
    
    #ifdef I2C0_SLAVE
    I2C0_context.rxSizePtr = size;
    I2C0_context.rx.size = *size;
    #else
    I2C0_context.rx.size = size;
    #endif
    I2C0_context.rx.bufferPtr = (uint8_t*)data;
    I2C0_context.index = 0;
    I2C0_context.handler = handler;
    
    I2C0_context.autoRestart = false;

    #ifdef I2C0_MASTER
    I2C0_REG(IE) |= UCRXIE;                 // Enable RX interrupt
    I2C0_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C0_REG(CTL1) |= UCTXSTT;              // Send start condition  
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C0_request()
// Description:
//! \brief          Transmit byte array and receive byte array via I2C
//------------------------------------------------------------------------------
void I2C0_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void))
{
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C0_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C0_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C0_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C0_REG(I2CSA) = address;
    
    I2C0_context.errorFlag = false;         // Clear error flag
    
    I2C0_context.tx.bufferPtr = (uint8_t*)txData;
    I2C0_context.tx.size = txSize;
   
    I2C0_context.rx.bufferPtr = (uint8_t*)rxData;
    I2C0_context.rx.size = rxSize;
    
    I2C0_context.index = 0;
    I2C0_context.handler = handler;
    
    I2C0_context.autoRestart = true;
    
    uint8_t tmp = I2C0_REG(RXBUF);
    
    I2C0_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG);
    I2C0_REG(IE) |= (UCRXIE + UCTXIE + UCNACKIE); // Enable RX&TX&NACK interrupt
    
    I2C0_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C0_REG(CTL1) |= UCTXSTT;              // Send start condition  
}

//------------------------------------------------------------------------------
// Function:	
//              I2C0_isr()
// Description:
//! \brief      I2C0 ISR
//------------------------------------------------------------------------------
#pragma vector=I2C0_ISR_VECTOR
__interrupt void I2C0_isr(void)
{
    switch(__even_in_range(I2C0_REG(IV),12))
    {
        case  0: break;                     // Vector  0: No interrupts
        case  2: break;                     // Vector  2: ALIFG
        //------------------------------------------------------------------
        case  4:                            // Vector  4: NACKIFG
            I2C0_context.errorFlag = true;  // I2C error
            #ifdef I2C0_MASTER
            I2C0_REG(CTL1) |= UCTXSTP;      // Send stop condition
            I2C0_REG(IFG) &= ~UCTXIFG;      // Clear TX IFG flag
            if(I2C0_context.handler)
            {
                #ifdef I2C_handlerCaller
                    I2C_handlerCaller(I2C0_context.handler);
                #else
                    I2C0_context.handler();
                #endif
                I2C0_context.handler = NULL;
            }
            #endif
            break;
        //------------------------------------------------------------------
        case  6: break;                     // Vector  6: STTIFG
        //------------------------------------------------------------------
        case  8:                            // Vector  8: STPIFG
            #ifdef I2C0_SLAVE
            if(I2C0_context.handler)
            {
                // Output received size
                if(I2C0_context.rxSizePtr)
                {
                    *I2C0_context.rxSizePtr = I2C0_context.index;
                    I2C0_context.rxSizePtr = NULL;
                }
                #ifdef I2C_handlerCaller
                    I2C_handlerCaller(I2C0_context.handler);
                #else
                    I2C0_context.handler();
                #endif
                I2C0_context.handler = NULL;
            }
            // Disable I2C0 interrupts
            //I2C0_REG(IE) &= ~(UCRXIE+UCTXIE+UCNACKIE+UCSTPIFG);
            #endif
            break;                     
        //------------------------------------------------------------------
        case 10:                            // Vector 10: RXIFG
            // Check RX index
            if(I2C0_context.rx.bufferPtr == NULL)
            {
                #ifdef I2C0_SLAVE
                I2C0_REG(CTL1) |= UCTXNACK; // Send NACK 
                #else
                uint8_t tmp = I2C0_REG(RXBUF);
                #endif
                break;
            }
            
            // Read data byte
            I2C0_context.rx.bufferPtr[I2C0_context.index++] = I2C0_REG(RXBUF);
            if(I2C0_context.index == I2C0_context.rx.size)
                I2C0_context.rx.bufferPtr = NULL;

            #ifdef I2C0_MASTER
            if(I2C0_context.index == (I2C0_context.rx.size - 1) ||
               I2C0_context.rx.size == 1)   // Check RX index
            {                               // Send stop condition immediately
                I2C0_REG(CTL1) |= UCTXSTP;  // after last byte received
            }
            
            if(I2C0_context.index == I2C0_context.rx.size)
            {
                if(I2C0_context.handler)
                {
                    #ifdef I2C_handlerCaller
                        I2C_handlerCaller(I2C0_context.handler);
                    #else
                        I2C0_context.handler();
                    #endif
                    I2C0_context.handler = NULL;
               }
               //I2C0_REG(IE) &= ~(UCRXIE+UCSTPIE);  // Disable RX interrupt
            }
            #endif
           break;
        //------------------------------------------------------------------
        case 12:                            // Vector 12: TXIFG
            // Check TX index
            if(I2C0_context.index < I2C0_context.tx.size) // Send data byte
            {
                I2C0_REG(TXBUF) = 
                    I2C0_context.tx.bufferPtr[I2C0_context.index++];
                if(I2C0_context.index == I2C0_context.tx.size)
                    I2C0_context.tx.bufferPtr = NULL;
            }
            else
            #ifdef I2C0_SLAVE
            {
                I2C0_REG(TXBUF) = 0xFF;
            }
            #else
            {                                   // After last byte received
                if(I2C0_context.autoRestart)    // immediately..
                {
                    I2C0_context.index = 0;
                    I2C0_REG(CTL1) &= ~UCTR;    // set Master receiver and..
                    I2C0_REG(CTL1) |= UCTXSTT;  // send restart condition  
                }
                else
                {                               // After last byte sent
                    I2C0_REG(CTL1) |= UCTXSTP;  // send stop condition
                    I2C0_REG(IFG) &= ~UCTXIFG;  // clear TX IFG flag
                    
                    if(I2C0_context.handler)
                    {
                        #ifdef I2C_handlerCaller
                            I2C_handlerCaller(I2C0_context.handler);
                        #else
                            I2C0_context.handler();
                        #endif
                        I2C0_context.handler = NULL;
                    }                           // Disable TX&NACK interrupts
                    I2C0_REG(IE) &= ~(UCTXIE+UCNACKIE+UCSTPIE); 
                }
            }
            #endif
            break;
        default: break; 
    }
    
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                      // Wake-up MCU
    #endif
}

#endif // I2C0_MASTER/I2C0_SLAVE

// Check selected I2C1 mode
#if ( defined(I2C1_MASTER) && defined(I2C1_SLAVE))
    #error I2C: Select proper I2C1 mode!
#elif ( defined(I2C1_MASTER) || defined(I2C1_SLAVE))

//------------------------------------------------------------------------------
// Function:	
//              I2C1_init()
// Description:
//! \brief      Initialize I2C1
//------------------------------------------------------------------------------
bool I2C1_init(void)
{
    if(I2C1_resetBus() == false)  // Master resets bus if slave hangs SDA
        return false;             // Slave always returns SUCCESS
    
    // Initialize SDA, SCL
    GPIO_func2(I2C1_SDA_PIN);
    GPIO_func2(I2C1_SCL_PIN);

    I2C1_REG(CTL1) |= UCSWRST;                  // Enable SW reset
    I2C1_REG(CTL0) = UCMODE_3 + UCSYNC;         // Synchronous mode
    #ifdef I2C1_MASTER
        I2C1_REG(CTL0) |= UCMST;                // I2C Master
    #endif
  
    #if (I2C1_CLK == SMCLK_FREQ)
        I2C1_REG(CTL1) |= UCSSEL__SMCLK;        // I2C1_CLK = SMCLK
    #elif (I2C1_CLK == ACLK_FREQ)
        I2C1_REG(CTL1) |= UCSSEL__ACLK;         // I2C1_CLK = ACLK
    #else
        #error I2C: Unsupported I2C1 clock frequency
    #endif

    #if ((I2C1_BAUDRATE) < 1000 || (I2C1_BAUDRATE) > 1000000L)
        #error I2C: Unsupported I2C1 baudrate
    #else
        I2C1_REG(BR0) = (((I2C1_CLK/I2C1_BAUDRATE) - 1)&0xFF);
        I2C1_REG(BR1) = (((I2C1_CLK/I2C1_BAUDRATE) - 1)>>8)&0xFF;
    #endif
    
    I2C1_REG(I2COA) =  I2C1_OWN_ADDRESS;
    I2C1_REG(CTL1) &= ~(UCTXSTP+UCTXSTT);
    I2C1_REG(CTL1) &= ~UCSWRST;             // Clear SW reset, resume operation

    #ifdef I2C1_SLAVE
    I2C1_REG(I2COA) |=  UCGCEN;             // Respond to a general call
    I2C1_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG+UCSTPIFG);
    I2C1_REG(IE) |= (UCRXIE+UCTXIE+UCNACKIE+UCSTPIE);// Enable TX&NACK interrupt
    #endif
    
    return true;
}

//------------------------------------------------------------------------------
// Function:	
//              I2C1_resetBus()
// Description:
//! \brief      Reset I2C1 bus
//------------------------------------------------------------------------------
bool I2C1_resetBus(void)
{
    #ifdef I2C1_MASTER

    IntFlag_t intState;
    EnterCriticalSection(intState);
    
    I2C1_REG(CTL1) |= UCTXSTP;          // Send stop condition
    __delay_us((1000000L/I2C1_BAUDRATE) + 1);
    I2C1_REG(CTL1) |= UCSWRST;          // Enable SW reset
    
    // Reset I2C if slave hang the line
    GPIO_func1(I2C1_SDA_PIN);           // SDA -> GPIO
    GPIO_setIn(I2C1_SDA_PIN);           // SDA -> input
    
    GPIO_setHigh(I2C1_SCL_PIN);         // SCL <- high    
    GPIO_func1(I2C1_SCL_PIN);           // SCL <- GPIO
    GPIO_setOut(I2C1_SCL_PIN);          // SCL <- output
    
    // Send nine clock pulses to release bus by slave
    uint8_t counter = 9; // (one byte + ack bit)
    while(counter--) 
    {
        if(GPIO_test(I2C1_SDA_PIN))    // Check SDA line
        {
            LeaveCriticalSection(intState);
            return true;             
        }

        GPIO_setLow(I2C1_SCL_PIN);      // SCL <- low
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);
        GPIO_setHigh(I2C1_SCL_PIN);     // SCL <- high
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);
    }
                
    LeaveCriticalSection(intState);
    return false;                     // unrecoverable bus error
    
    #else
        return true;
    #endif

}

//------------------------------------------------------------------------------
// Function:	
//                      I2C1_isBusy()
// Description:
//! \brief              Check I2C1 state
//------------------------------------------------------------------------------
bool I2C1_isBusy(void)
{
    return (I2C1_REG(STAT) & UCBBUSY) ? true : false; // check busy flag
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C1_error()
// Description:
//! \brief              Check I2C1 error
//------------------------------------------------------------------------------
bool I2C1_error(void)
{
    return (I2C1_context.errorFlag || (I2C1_REG(IFG)&UCALIFG));
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C1_setOwnAddress()
// Description:
//! \brief              Set I2C1 own address
//------------------------------------------------------------------------------
void I2C1_setOwnAddress(uint8_t address)
{
    I2C1_REG(I2COA) &=  0xFF00;
    I2C1_REG(I2COA) |=  address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C1_selectSlaveAddress()
// Description:
//! \brief              Select I2C1 slave address 
//------------------------------------------------------------------------------
void I2C1_selectSlaveAddress(uint8_t address)
{
    I2C1_REG(I2CSA) = address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C1_txByte()
// Description:
//! \brief              Transmit byte via I2C1
//------------------------------------------------------------------------------
void I2C1_txByte(uint8_t dataByte)
{
    uint16_t counter;
    
    I2C1_context.errorFlag = false;             // Clear error flag
    
    I2C1_REG(IE) &= ~(UCTXIE+UCRXIE+UCNACKIE);  // Disable interrupts
    I2C1_REG(IFG)&= ~(UCTXIFG+UCRXIFG+UCNACKIFG);

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C1_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C1_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
	I2C1_REG(CTL1) |= UCTR;                     // Set Master transmitter
    I2C1_REG(CTL1) |= UCTXSTT;                  // Send start condition

    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          !(I2C1_REG(IFG) & (UCTXIFG+UCNACKIFG)))
        __delay_us((1000000L/I2C1_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C1_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
    if(I2C1_REG(IFG) & UCNACKIFG)
    {
        I2C1_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C1_context.errorFlag = true;          // I2C NACK error
        return;
    }

    I2C1_REG(TXBUF) = dataByte;
    
    counter = 9;                                //(data byte + ack bit)
    while(counter-- &&                          // Wait data byte is shifted out
          !(I2C1_REG(IFG) & (UCTXIFG+UCNACKIFG)))
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);
    if(!counter)
    {
        I2C1_context.errorFlag = true;          // I2C timeout error
        return;
    }

    if(I2C1_REG(IFG) & UCNACKIFG)
    {
        I2C1_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C1_context.errorFlag = true;          // I2C NACK error
        return;
    }
    
    I2C1_REG(CTL1) |= UCTXSTP;                  // Send stop condition
    I2C1_REG(IFG)  &= ~(UCTXIFG+UCNACKIFG);
}

//------------------------------------------------------------------------------
// Function:	
//                    I2C1_rxByte()
// Description:
//! \brief            Receive byte via I2C
//------------------------------------------------------------------------------
uint8_t I2C1_rxByte(void)
{
    uint16_t counter;
    
    I2C1_context.errorFlag = false;             // clear error flag
    
    I2C1_REG(IE) &= ~(UCTXIE+UCRXIE+UCNACKIE);  // Disable interrupts
    I2C1_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG);

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C1_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C1_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C1_REG(CTL1) |= UCTXSTT;              // Send start condition
    
    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          I2C1_REG(CTL1) & UCTXSTT)
        __delay_us((1000000L/I2C1_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C1_REG(CTL1) |= UCTXSTP;              // Send stop immediately 
                                            // after byte received
    counter = 9;
    while(counter-- &&                      // Wait data byte is shifted in
          !(I2C1_REG(IFG) & (UCRXIFG)))
        __delay_us((1000000L/I2C1_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }

    return I2C1_REG(RXBUF);                  // return data byte
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C1_transmit()
// Description:
//! \brief          Transmit byte array via I2C1
//------------------------------------------------------------------------------
#ifdef I2C1_MASTER
void I2C1_transmit(uint8_t address, 
                   const void* data, uint16_t size, void (*handler)(void))
#else
void I2C1_transmit(const void* data, uint16_t size, void (*handler)(void))
#endif
{
    #ifdef I2C1_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C1_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C1_REG(I2CSA) = address;
    #endif
    
    I2C1_context.errorFlag = false;         // clear error flag
    
    I2C1_context.tx.bufferPtr = (uint8_t*)data;
    I2C1_context.tx.size = size;
    I2C1_context.index = 0;
    I2C1_context.handler = handler;
    
    I2C1_context.autoRestart = false;
    
    #ifdef I2C1_MASTER
    I2C1_REG(IE) |= (UCTXIE + UCNACKIE);    // Enable TX&NACK interrupt
    I2C1_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C1_REG(CTL1) |= UCTXSTT;              // Send start condition
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C1_receive()
// Description:
//! \brief          Receive byte array via I2C
//------------------------------------------------------------------------------
#ifdef I2C1_MASTER
void I2C1_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void))
#else
void I2C1_receive(void* data, uint16_t* size, void (*handler)(void))
#endif
{
    #ifdef I2C1_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C1_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C1_REG(I2CSA) = address;
    #endif
    
    I2C1_context.errorFlag = false;         // Clear error flag
    
    #ifdef I2C1_SLAVE
    I2C1_context.rxSizePtr = size;
    I2C1_context.rx.size = *size;
    #else
    I2C1_context.rx.size = size;
    #endif
    I2C1_context.rx.bufferPtr = (uint8_t*)data;
    I2C1_context.index = 0;
    I2C1_context.handler = handler;
    
    I2C1_context.autoRestart = false;

    #ifdef I2C1_MASTER
    I2C1_REG(IE) |= UCRXIE;                 // Enable RX interrupt
    I2C1_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C1_REG(CTL1) |= UCTXSTT;              // Send start condition  
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C1_request()
// Description:
//! \brief          Transmit byte array and receive byte array via I2C
//------------------------------------------------------------------------------
void I2C1_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void))
{
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C1_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C1_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C1_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C1_REG(I2CSA) = address;
    
    I2C1_context.errorFlag = false;         // Clear error flag
    
    I2C1_context.tx.bufferPtr = (uint8_t*)txData;
    I2C1_context.tx.size = txSize;
   
    I2C1_context.rx.bufferPtr = (uint8_t*)rxData;
    I2C1_context.rx.size = rxSize;
    
    I2C1_context.index = 0;
    I2C1_context.handler = handler;
    
    I2C1_context.autoRestart = true;
    
    uint8_t tmp = I2C1_REG(RXBUF);
    
    I2C1_REG(IFG)  &= ~(UCTXIFG+UCRXIFG+UCNACKIFG);
    I2C1_REG(IE) |= (UCRXIE + UCTXIE + UCNACKIE); // Enable RX&TX&NACK interrupt
    
    I2C1_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C1_REG(CTL1) |= UCTXSTT;              // Send start condition  
}

//------------------------------------------------------------------------------
// Function:	
//              I2C1_isr()
// Description:
//! \brief      I2C1 ISR
//------------------------------------------------------------------------------
#pragma vector=I2C1_ISR_VECTOR
__interrupt void I2C1_isr(void)
{
    switch(__even_in_range(I2C1_REG(IV),12))
    {
        case  0: break;                     // Vector  0: No interrupts
        case  2: break;                     // Vector  2: ALIFG
        //------------------------------------------------------------------
        case  4:                            // Vector  4: NACKIFG
            I2C1_context.errorFlag = true;  // I2C error
            #ifdef I2C1_MASTER
            I2C1_REG(CTL1) |= UCTXSTP;      // Send stop condition
            I2C1_REG(IFG) &= ~UCTXIFG;      // Clear TX IFG flag
            if(I2C1_context.handler)
            {
                #ifdef I2C_handlerCaller
                    I2C_handlerCaller(I2C1_context.handler);
                #else
                    I2C1_context.handler();
                #endif
                I2C1_context.handler = NULL;
            }
            #endif
            break;
        //------------------------------------------------------------------
        case  6: break;                     // Vector  6: STTIFG
        //------------------------------------------------------------------
        case  8:                            // Vector  8: STPIFG
            #ifdef I2C1_SLAVE
            if(I2C1_context.handler)
            {
                // Output received size
                if(I2C1_context.rxSizePtr)
                {
                    *I2C1_context.rxSizePtr = I2C1_context.index;
                    I2C1_context.rxSizePtr = NULL;
                }
                #ifdef I2C_handlerCaller
                    I2C_handlerCaller(I2C1_context.handler);
                #else
                    I2C1_context.handler();
                #endif
                I2C1_context.handler = NULL;
            }
            // Disable I2C1 interrupts
            //I2C1_REG(IE) &= ~(UCRXIE+UCTXIE+UCNACKIE+UCSTPIFG);
            #endif
            break;                     
        //------------------------------------------------------------------
        case 10:                            // Vector 10: RXIFG
            // Check RX index
            if(I2C1_context.index >= I2C1_context.rx.size)
            {
                #ifdef I2C1_SLAVE
                I2C1_REG(CTL1) |= UCTXNACK; // Send NACK 
                #else
                uint8_t tmp = I2C1_REG(RXBUF);
                #endif
                break;
            }
            
            // Read data byte
            I2C1_context.rx.bufferPtr[I2C1_context.index++] = I2C1_REG(RXBUF);
            if(I2C1_context.index == I2C1_context.rx.size)
                I2C1_context.rx.bufferPtr = NULL;

            #ifdef I2C1_MASTER
            if(I2C1_context.index == (I2C1_context.rx.size - 1) ||
               I2C1_context.rx.size == 1)   // Check RX index
            {                               // Send stop condition immediately
                I2C1_REG(CTL1) |= UCTXSTP;  // after last byte received
            }
            
            if(I2C1_context.index == I2C1_context.rx.size)
            {
                if(I2C1_context.handler)
                {
                    #ifdef I2C_handlerCaller
                        I2C_handlerCaller(I2C1_context.handler);
                    #else
                        I2C1_context.handler();
                    #endif
                    I2C1_context.handler = NULL;
               }
               //I2C1_REG(IE) &= ~(UCRXIE+UCSTPIE);  // Disable RX interrupt
            }
            #endif
           break;
        //------------------------------------------------------------------
        case 12:                            // Vector 12: TXIFG
            // Check TX index
            if(I2C1_context.index < I2C1_context.tx.size) // Send data byte
            {
                I2C1_REG(TXBUF) = 
                    I2C1_context.tx.bufferPtr[I2C1_context.index++];
                if(I2C1_context.index == I2C1_context.tx.size)
                    I2C1_context.tx.bufferPtr = NULL;
            }
            else
            #ifdef I2C1_SLAVE
            {
                I2C1_REG(TXBUF) = 0xFF;
            }
            #else
            {                                   // After last byte received
                if(I2C1_context.autoRestart)    // immediately..
                {
                    I2C1_context.index = 0;
                    I2C1_REG(CTL1) &= ~UCTR;    // set Master receiver and..
                    I2C1_REG(CTL1) |= UCTXSTT;  // send restart condition  
                }
                else
                {                               // After last byte sent
                    I2C1_REG(CTL1) |= UCTXSTP;  // send stop condition
                    I2C1_REG(IFG) &= ~UCTXIFG;  // clear TX IFG flag
                    
                    if(I2C1_context.handler)
                    {
                        #ifdef I2C_handlerCaller
                            I2C_handlerCaller(I2C1_context.handler);
                        #else
                            I2C1_context.handler();
                        #endif
                        I2C1_context.handler = NULL;
                    }                           // Disable TX&NACK interrupts
                    I2C1_REG(IE) &= ~(UCTXIE+UCNACKIE+UCSTPIE); 
                }
            }
            #endif
            break;
        default: break; 
    }
    
    #ifdef USE_LOW_POWER_MODE
        LPM_disable();                      // Wake-up MCU
    #endif
}

#endif // I2C1_MASTER/I2C1_SLAVE

#endif // _MSP430_HAL_
//******************************************************************************
// End of file
//******************************************************************************

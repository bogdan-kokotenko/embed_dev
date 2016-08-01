//******************************************************************************
//  Copyright (C) 2015 Bogdan Kokotenko
//
//  File description:
//! \file   hal/mcu/msp430g2x/i2c.c
//! \brief  MSP430 I2C functions implementation.
//!
//!*****************************************************************************
//! __Revisions:__										
//!  Date       | Author           | Comments			
//!  ---------- | ---------------- | ------------------------------------------
//!  30/06/2015 | Bogdan Kokotenko | Initial draft
//!  13/02/2016 | Bogdan Kokotenko | Ported to MSP430G2x
//!  11/05/2016 | Bogdan Kokotenko | Fixed compatibility with SPI
//
//******************************************************************************
#include "project.h"
#include "types.h"
#include "hal.h"
#include "gpio.h"
#include "clocks.h"
#include "task.h"
#include "spi.h"
#include "i2c.h"

// Warn of inappropriate MCU core selection
#if ( !defined (_MSP430G2X_HAL_) )
#warning I2C: Unknown MCU core, check HAL configuration!
#else

// Check if SPI mode selected
#if ( (defined(I2C_MASTER) || defined(I2C_SLAVE)) && \
      (defined(SPI_MASTER) || defined(SPI_SLAVE)) )
    #error I2C: Interface is not avaliable if SPI mode selected!
#endif

// Check selected I2C mode
#if ( (defined(I2C_MASTER) && defined(I2C_SLAVE)) )
    #error I2C: Select proper I2C mode!
#elif ( defined(I2C_MASTER) || defined(I2C_SLAVE))

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
#if ( defined(I2C_SLAVE) || defined(I2C_SLAVE))
    uint16_t*   rxSizePtr;          //!< pointer to rx size output
#endif
}I2C_context;

//! Reset I2C bus
static bool I2C_resetBus(void);

//------------------------------------------------------------------------------
// Function:	
//              I2C_init()
// Description:
//! \brief      Initialize I2C
//------------------------------------------------------------------------------
bool I2C_init(void)
{
    if(I2C_resetBus() == FAILURE)  // Master resets bus if slave hangs SDA
        return FAILURE;             // Slave always returns SUCCESS
    
    // Initialize SDA, SCL
    GPIO_func3(I2C_SDA_PIN);
    GPIO_func3(I2C_SCL_PIN);

    I2C_REG(CTL1) |= UCSWRST;                   // Enable SW reset
    I2C_REG(CTL0) = UCMODE_3 + UCSYNC;          // Synchronous mode
    #ifdef I2C_MASTER
        I2C_REG(CTL0) |= UCMST;                 // I2C Master
    #endif
  
    #if (I2C_CLK == SMCLK_FREQ)
        I2C_REG(CTL1) |= UCSSEL_2;              // I2C_CLK = SMCLK
    #elif (I2C_CLK == ACLK_FREQ)
        I2C_REG(CTL1) |= UCSSEL_1;              // I2C_CLK = ACLK
    #else
        #error I2C: Unsupported I2C clock frequency
    #endif

    #if ((I2C_BAUDRATE) < 1000 || (I2C_BAUDRATE) > 1000000L)
        #error I2C: Unsupported I2C baudrate
    #else
        I2C_REG(BR0) = (((I2C_CLK/I2C_BAUDRATE) - 1)&0xFF);
        I2C_REG(BR1) = (((I2C_CLK/I2C_BAUDRATE) - 1)>>8)&0xFF;
    #endif
    
    I2C_REG(I2COA) =  I2C_OWN_ADDRESS;
    I2C_REG(CTL1) &= ~(UCTXSTP+UCTXSTT);
    I2C_REG(CTL1) &= ~UCSWRST;              // Clear SW reset, resume operation

    #ifdef I2C_SLAVE
    I2C_REG(I2COA) |=  UCGCEN;              // Respond to a general call
    I2C_REG(STAT) &= ~(UCNACKIFG);
    IFG2&= ~(I2C_REG(TXIFG)+I2C_REG(RXIFG)); 
    I2C_REG(I2CIE) |= (UCNACKIE);           // Enable NACK interrupts
    IE2 |= (I2C_REG(TXIE)+I2C_REG(RXIE));   // Enable TX&RX interrupt
    //I2C_REG(I2CIE) |= (UCSTPIE);          // Enable TX&NACK interrupt
    #endif
    
    return SUCCESS;
}

//------------------------------------------------------------------------------
// Function:	
//              I2C_resetBus()
// Description:
//! \brief      Reset I2C bus
//------------------------------------------------------------------------------
bool I2C_resetBus(void)
{
    #ifdef I2C_MASTER

    IntFlag_t intState;
    EnterCriticalSection(intState);
    
    I2C_REG(CTL1) |= UCTXSTP;          // Send stop condition
    __delay_us((1000000L/I2C_BAUDRATE) + 1);
    I2C_REG(CTL1) |= UCSWRST;          // Enable SW reset
    
    // Reset I2C if slave hang the line
    GPIO_func0(I2C_SDA_PIN);           // SDA -> GPIO
    GPIO_setIn(I2C_SDA_PIN);           // SDA -> input
    
    GPIO_setHigh(I2C_SCL_PIN);         // SCL <- high    
    GPIO_func0(I2C_SCL_PIN);           // SCL <- GPIO
    GPIO_setOut(I2C_SCL_PIN);          // SCL <- output
    
    // Send nine clock pulses to release bus by slave
    uint8_t counter = 9; // (one byte + ack bit)
    while(counter--) 
    {
        if(GPIO_test(I2C_SDA_PIN))    // Check SDA line
        {
            LeaveCriticalSection(intState);
            return SUCCESS;             
        }

        GPIO_setLow(I2C_SCL_PIN);      // SCL <- low
        __delay_us((1000000L/I2C_BAUDRATE) + 1);
        GPIO_setHigh(I2C_SCL_PIN);     // SCL <- high
        __delay_us((1000000L/I2C_BAUDRATE) + 1);
    }
                
    LeaveCriticalSection(intState);
    return FAILURE;                     // unrecoverable bus error
    
    #else
        return SUCCESS;
    #endif

}

//------------------------------------------------------------------------------
// Function:	
//                      I2C_isBusy()
// Description:
//! \brief              Check I2C state
//------------------------------------------------------------------------------
bool I2C_isBusy(void)
{
    return (I2C_REG(STAT) & UCBBUSY) ? true : false; // check busy flag
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C_error()
// Description:
//! \brief              Check I2C error
//------------------------------------------------------------------------------
bool I2C_error(void)
{
    return (I2C_context.errorFlag || (I2C_REG(STAT)&UCALIFG));
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C_setOwnAddress()
// Description:
//! \brief              Set I2C own address
//------------------------------------------------------------------------------
void I2C_setOwnAddress(uint8_t address)
{
    I2C_REG(I2COA) &=  0xFF00;
    I2C_REG(I2COA) |=  address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C_selectSlaveAddress()
// Description:
//! \brief              Select I2C slave address 
//------------------------------------------------------------------------------
void I2C_selectSlaveAddress(uint8_t address)
{
    I2C_REG(I2CSA) = address;
}

//------------------------------------------------------------------------------
// Function:	
//                      I2C_txByte()
// Description:
//! \brief              Transmit byte via I2C
//------------------------------------------------------------------------------
void I2C_txByte(uint8_t dataByte)
{
    uint16_t counter;
    
    I2C_context.errorFlag = false;              // Clear error flag
    
    I2C_REG(I2CIE) &= ~(UCNACKIE);              // Disable NACK interrupts
    IE2 &= ~(I2C_REG(TXIE)+I2C_REG(RXIE));      // Disable RX&TX interrupts
    I2C_REG(STAT) &= ~(UCNACKIFG);
    IFG2&= ~(I2C_REG(TXIFG)+I2C_REG(RXIFG));

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
	I2C_REG(CTL1) |= UCTR;                     // Set Master transmitter
    I2C_REG(CTL1) |= UCTXSTT;                  // Send start condition

    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          !(IFG2 & I2C_REG(TXIFG)) &&
          !(I2C_REG(STAT) & UCNACKIFG))
        __delay_us((1000000L/I2C_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C_context.errorFlag = true;          // I2C timeout error
        return;
    }
    
    if(I2C_REG(STAT) & UCNACKIFG)
    {
        I2C_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C_context.errorFlag = true;          // I2C NACK error
        return;
    }

    I2C_REG(TXBUF) = dataByte;
    
    counter = 9;                                //(data byte + ack bit)
    while(counter-- &&                          // Wait data byte is shifted out
          !(IFG2 & I2C_REG(TXIFG)) &&
          !(I2C_REG(STAT) & UCNACKIFG))
        __delay_us((1000000L/I2C_BAUDRATE) + 1);
    if(!counter)
    {
        I2C_context.errorFlag = true;          // I2C timeout error
        return;
    }

    if(I2C_REG(STAT) & UCNACKIFG)
    {
        I2C_REG(CTL1) |= UCTXSTP;              // Send stop condition
        I2C_context.errorFlag = true;          // I2C NACK error
        return;
    }
    
    I2C_REG(CTL1) |= UCTXSTP;                  // Send stop condition
    I2C_REG(STAT) &= ~(UCNACKIFG);
    IFG2&= ~UCB0TXIFG;

}

//------------------------------------------------------------------------------
// Function:	
//                    I2C_rxByte()
// Description:
//! \brief            Receive byte via I2C
//------------------------------------------------------------------------------
uint8_t I2C_rxByte(void)
{
    uint16_t counter;
    
    I2C_context.errorFlag = false;          // clear error flag
    
    I2C_REG(I2CIE) &= ~(UCNACKIE);          // Disable NACK interrupts
    IE2 &= ~(I2C_REG(TXIE)+I2C_REG(RXIE));  // Disable RX&TX interrupts
    I2C_REG(STAT) &= ~(UCNACKIFG);
    IFG2&= ~(I2C_REG(TXIFG)+I2C_REG(RXIFG));

    counter = 10;                           //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C_REG(CTL1) |= UCTXSTT;              // Send start condition
    
    counter = 10;                           //(st bit + address byte + ack bit)                            
    while(counter-- &&                      // Wait start byte is shifted out
          I2C_REG(CTL1) & UCTXSTT)
        __delay_us((1000000L/I2C_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }
    
    I2C_REG(CTL1) |= UCTXSTP;              // Send stop immediately 
                                            // after byte received
    counter = 9;
    while(counter-- &&                      // Wait data byte is shifted in
          !(IFG2 & UCB0RXIFG))
        __delay_us((1000000L/I2C_BAUDRATE) + 1); 
    if(!counter)
    {
        I2C_context.errorFlag = true;      // I2C timeout error
        return 0x00;
    }

    return I2C_REG(RXBUF);                  // return data byte
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C_transmit()
// Description:
//! \brief          Transmit byte array via I2C
//------------------------------------------------------------------------------
#ifdef I2C_MASTER
void I2C_transmit(uint8_t address, 
                   const void* data, uint16_t size, void (*handler)(void))
#else
void I2C_transmit(const void* data, uint16_t size, void (*handler)(void))
#endif
{
    #ifdef I2C_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C_REG(I2CSA) = address;
    #endif
    
    I2C_context.errorFlag = false;         // clear error flag
    
    I2C_context.tx.bufferPtr = (uint8_t*)data;
    I2C_context.tx.size = size;
    I2C_context.index = 0;
    I2C_context.handler = handler;
    
    I2C_context.autoRestart = false;
    
    #ifdef I2C_MASTER
    I2C_REG(I2CIE) |= UCNACKIE;             // Enable NACK interrupts
    IE2 |= I2C_REG(TXIE);                   // Enable TX interrupts
    
    I2C_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C_REG(CTL1) |= UCTXSTT;              // Send start condition
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C_receive()
// Description:
//! \brief          Receive byte array via I2C
//------------------------------------------------------------------------------
#ifdef I2C_MASTER
void I2C_receive(uint8_t address,
                  void* data, uint16_t size, void (*handler)(void))
#else
void I2C_receive(void* data, uint16_t* size, void (*handler)(void))
#endif
{
    #ifdef I2C_MASTER
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C_REG(I2CSA) = address;
    #endif
    
    I2C_context.errorFlag = false;         // Clear error flag
    
    #ifdef I2C_SLAVE
    I2C_context.rxSizePtr = size;
    I2C_context.rx.size = *size;
    #else
    I2C_context.rx.size = size;
    #endif
    I2C_context.rx.bufferPtr = (uint8_t*)data;
    I2C_context.index = 0;
    I2C_context.handler = handler;
    
    I2C_context.autoRestart = false;

    #ifdef I2C_MASTER
    IE2 |= I2C_REG(RXIE);                  // Enable RX interrupts
    I2C_REG(CTL1) &= ~UCTR;                // Set Master receiver
    I2C_REG(CTL1) |= UCTXSTT;              // Send start condition  
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//                  I2C_request()
// Description:
//! \brief          Transmit byte array and receive byte array via I2C
//------------------------------------------------------------------------------
void I2C_request(uint8_t address,
                  const void* txData, uint16_t txSize,
                  void* rxData, uint16_t rxSize,
                  void (*handler)(void))
{
    uint16_t counter = 10;                  //(data byte + ack bit + sp bit)
    while (counter-- &&
           I2C_REG(CTL1 & UCTXSTP))        // Ensure stop condition got sent
        __delay_us((1000000L/I2C_BAUDRATE) + 1);      
    if(!counter)
    {
        I2C_context.errorFlag = true;      // Timeout error
        return;
    }
    
    I2C_REG(I2CSA) = address;
    
    I2C_context.errorFlag = false;         // Clear error flag
    
    I2C_context.tx.bufferPtr = (uint8_t*)txData;
    I2C_context.tx.size = txSize;
   
    I2C_context.rx.bufferPtr = (uint8_t*)rxData;
    I2C_context.rx.size = rxSize;
    
    I2C_context.index = 0;
    I2C_context.handler = handler;
    
    I2C_context.autoRestart = true;
    
    uint8_t tmp = I2C_REG(RXBUF);
    
    I2C_REG(STAT) &= ~(UCNACKIFG);
    IFG2&= ~(I2C_REG(TXIFG)+I2C_REG(RXIFG));

    I2C_REG(I2CIE) |= UCNACKIE;          // Enable NACK interrupts
    IE2 |= (I2C_REG(TXIE)+I2C_REG(RXIE));  // Enable RX&TX interrupts
    
    I2C_REG(CTL1) |= UCTR;                 // Set Master transmitter
    I2C_REG(CTL1) |= UCTXSTT;              // Send start condition  
}

//------------------------------------------------------------------------------
// Function:	
//              I2C_nackHandler()
// Description:
//! \brief      I2C NACK handler
//------------------------------------------------------------------------------
void I2C_nackHandler(void)
{
    // Clear TX IFG (erratum USCI25)
    IFG2&= ~(I2C_REG(TXIFG));
    
    // Clear NACK IFG
    I2C_REG(STAT) &= ~(UCNACKIFG);

    I2C_context.errorFlag = true;  // I2C error
    #ifdef I2C_MASTER
    I2C_REG(CTL1) |= UCTXSTP;      // Send stop condition
    IFG2&= ~(I2C_REG(TXIFG));       // Clear TX IFG flag
    
    if(I2C_context.handler)
    {
        #ifdef I2C_handlerCaller
            I2C_handlerCaller(I2C_context.handler);
        #else
            I2C_context.handler();
        #endif
        I2C_context.handler = NULL;
    }
    #endif
}

//------------------------------------------------------------------------------
// Function:	
//              I2C_stopHandler()
// Description:
//! \brief      I2C STOP hndler
//------------------------------------------------------------------------------
#ifdef I2C_SLAVE
void I2C_stopHandler(void)
{
    // Clear STOP IFG
    I2C_REG(STAT) &= ~(UCSTOPIFG);

    if(I2C_context.handler)
    {
        // Output received size
        if(I2C_context.rxSizePtr)
        {
            *I2C_context.rxSizePtr = I2C_context.index;
            I2C_context.rxSizePtr = NULL;
        }
        #ifdef I2C_handlerCaller
            I2C_handlerCaller(I2C_context.handler);
        #else
            I2C_context.handler();
        #endif
        I2C_context.handler = NULL;
    }
}
#endif

//------------------------------------------------------------------------------
// Function:	
//              I2C_rxHandler()
// Description:
//! \brief      I2C RX handler
//------------------------------------------------------------------------------
void I2C_rxHandler(void)
{
    // Check RX index
    if(I2C_context.rx.bufferPtr == NULL)
    {
        #ifdef I2C_SLAVE
        I2C_REG(CTL1) |= UCTXNACK; // Send NACK 
        #else
        uint8_t tmp = I2C_REG(RXBUF);
        #endif
        return;
    }

    // Read data byte
    I2C_context.rx.bufferPtr[I2C_context.index++] = I2C_REG(RXBUF);
    if(I2C_context.index == I2C_context.rx.size)
        I2C_context.rx.bufferPtr = NULL;

    #ifdef I2C_MASTER
    if(I2C_context.index == (I2C_context.rx.size - 1) ||
       I2C_context.rx.size == 1)   // Check RX index
    {                               // Send stop condition immediately
        I2C_REG(CTL1) |= UCTXSTP;  // after last byte received
    }

    if(I2C_context.index == I2C_context.rx.size)
    {
        if(I2C_context.handler)
        {
            #ifdef I2C_handlerCaller
                I2C_handlerCaller(I2C_context.handler);
            #else
                I2C_context.handler();
            #endif
            I2C_context.handler = NULL;
       }
       //I2C_REG(IE) &= ~(UCRXIE+UCSTPIE);  // Disable RX interrupt
    }
    #endif
    
    // Clear RX IFG
    IFG2&= ~(I2C_REG(RXIFG));
}

//------------------------------------------------------------------------------
// Function:	
//              I2C_txHandler()
// Description:
//! \brief      I2C TX handler
//------------------------------------------------------------------------------
void I2C_txHandler(void)
{
    // Clear TX IFG
    IFG2&= ~(I2C_REG(TXIFG));
    
    // Check TX index
    if(I2C_context.index < I2C_context.tx.size) // Send data byte
    {
        I2C_REG(TXBUF) = 
            I2C_context.tx.bufferPtr[I2C_context.index++];
        if(I2C_context.index == I2C_context.tx.size)
            I2C_context.tx.bufferPtr = NULL;
    }
    else
    #ifdef I2C_SLAVE
    {
        I2C_REG(TXBUF) = 0xFF;
    }
    #else
    {                                   // After last byte received
        if(I2C_context.autoRestart)    // immediately..
        {
            I2C_context.index = 0;
            I2C_REG(CTL1) &= ~UCTR;    // set Master receiver and..
            I2C_REG(CTL1) |= UCTXSTT;  // send restart condition  
        }
        else
        {                               // After last byte sent
            I2C_REG(CTL1) |= UCTXSTP;  // send stop condition
            IFG2&= ~(I2C_REG(TXIFG));       // Clear TX IFG flag
            
            if(I2C_context.handler)
            {
                #ifdef I2C_handlerCaller
                    I2C_handlerCaller(I2C_context.handler);
                #else
                    I2C_context.handler();
                #endif
                I2C_context.handler = NULL;
            }                           
            I2C_REG(I2CIE) &= ~(UCNACKIE+UCSTPIE);  // Disable STOP&NACK interrupts
            IE2 &= ~(I2C_REG(TXIE));                // Disable TX interrupts
        }
    }
    #endif
}

#endif // I2C_MASTER/I2C_SLAVE

#endif // _MSP430_HAL_
//******************************************************************************
// End of file
//******************************************************************************

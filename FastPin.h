/**
 * FastPin Library for Arduino
 * 
 * Fast but still easy-to-use methods
 * for pin access using the names you know.
 * 
 * Pin names are looked up once at initialization,
 * afterwards direct port manipulation is used.
 * 
 * License: BSD 3-Clause
 * 
 * Nick Schwarzenberg,
 * 11/2015, v0.1.0
 */


// use names of Arduino environment
#include <Arduino.h>

// prevent redefinitions
#ifndef FastPin_class
#define FastPin_class


class FastPin
{
    private:
        
        // PORTx or PINx register
        volatile unsigned char *portRegister;
        
        // mask of the pin's bit in the register
        unsigned char bitMask;
    
    public:
        
        // parameter-less constructor (allows to call setup() later)
        // (initializes pointer and bitmask to valid but useless values)
        FastPin()
        {
            // set register pointer to an arbitrary but valid register
            // (PINB names an 8-bit input register for ATmega128/328, ATmega32u4 and ATmega2560)
            this->portRegister = &PORTB;
            
            // set bitmask to all zeroes (to prevent write() from changing anything)
            this->bitMask = 0b00000000;
        }
        
        // constructor with instant setup (recommended)
        FastPin( unsigned char pinNumber, unsigned char pinMode )
        {
            // call setup immediately
            this->setup( pinNumber, pinMode );
        };
        
        // set up pin by its number and mode
        bool setup( unsigned char pinNumber, unsigned char pinMode )
        {
            // map pin number to port register block
            unsigned char portBlock = digitalPinToPort( pinNumber );
            
            // abort if port is invalid
            if ( portBlock == NOT_A_PIN ) { return false; }
            
            // get direction register (to define input/output)
            volatile unsigned char *directionRegister = portModeRegister( portBlock );
            
            // get input and output registers
            volatile unsigned char *inputRegister = portInputRegister( portBlock );
            volatile unsigned char *outputRegister = portOutputRegister( portBlock );
            
            // get the pin's bitmask for the port registers
            this->bitMask = digitalPinToBitMask( pinNumber );
            
            // treat as output?
            if ( pinMode == OUTPUT )
            {
                // set up pin as output (by adding the pin's bit)
                *directionRegister |= bitMask;
                
                // keep pointer to output register
                this->portRegister = outputRegister;
            }
            else
            {
                // set up pin as input (by clearing the pin's bit)
                *directionRegister &= ~bitMask;
                
                // enable pull-up?
                if ( pinMode == INPUT_PULLUP )
                {
                    // set bit in output register to high
                    *outputRegister |= bitMask;
                }
                else
                {
                    // set bit in output register to low
                    *outputRegister &= ~bitMask;
                }
                
                // keep pointer to input register
                this->portRegister = inputRegister;
            }
            
            // done, pin has been successfully mapped and set up
            return true;
        }
        
        // read the pin's bit from register
        inline bool read()
        {
            // read port register and mask the wanted bit
            return ( *this->portRegister & this->bitMask );
        }
        
        // write the pin's bit in register
        inline void write( bool logicValue )
        {
            // set output to high?
            if ( logicValue  )
            {
                // add bit to register value
                *this->portRegister |= this->bitMask;
            }
            else
            {
                // clear bit in register value
                *this->portRegister &= ~this->bitMask;
            }
        }
};


// see ifndef above
#endif

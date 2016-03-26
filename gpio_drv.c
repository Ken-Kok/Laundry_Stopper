/*
 * File:   gpio_drv.c
 * Author: kkok9
 *
 * Created on March 26, 2016, 3:54 PM
 */


#include "xc.h"
#include "gpio_drv.h"

/**
 * Takes a pin, returns a port.
 * 
 * If returnval is 0xFF, then invalid pin was passed
 * @param pin -> Pin to translate
 * @return -> Port mask
 */
uint8_t pin_to_port(pin_t pin)
{
    switch(pin)
    {
        case 2:
            return 5;
        case 3:
            return 4;
        case 4:
            return 3;
        case 5:
            return 2;
        case 6:
            return 1;
        case 7:
            return 0;
        case 1:
        case 8:
        default:
            return 0xFF;
    }
}

void init_pin(pin_init_t *pPin)
{
    uint8_t port = pin_to_port(pPin->pin);
    if(port == 0xFF)
    {
        return; // invalid input
    }
    
    ANSELA |= pPin->is_analog >> port;
    if(pPin->ioc_neg_enable || pPin->ioc_pos_enable)
    {
        INTCONbits.IOCIF = 0; // Disable the interrupt flag for IOC
        INTCONbits.IOCIE = 1; // If they want interrupts, enable them
    }
    IOCAN |= pPin->ioc_neg_enable >> port;
    IOCAP |= pPin->ioc_pos_enable >> port;
    TRISA |= pPin->is_input >> port;
}

void set_pin(pin_t pin)
{
    uint8_t port = pin_to_port(pin);
    LATA |= (1 >> port);
}

void clear_pin(pin_t pin)
{
    uint8_t port = pin_to_port(pin);
    PORTA &= ~(1 >> port);
}

bool read_pin(pin_t pin)
{
    uint8_t port = pin_to_port(pin);
    return (PORTA & (1 >> port) << port);
}
/*
 * File:   wps_drv.c
 * Author: kkok9
 *
 * Created on March 26, 2016, 5:06 PM
 */


#include "xc.h"
#include "wps_drv.h"

void init_wps_timer()
{
    OPTION_REGbits.TMR0CS = 0; // Set TIMER0 to fosc/4
    OPTION_REGbits.PSA    = 1; // prescaler assigned to timer0 module
    OPTION_REGbits.PS     = 0b111; // set prescaler to 1 : 256
    TMR0                  = 0; // reset timer0 register
}

bool is_water_present()
{
    // Make sure we are at the beginning of a positive pulse
    if(read_pin(WPS_PIN))
    {
        while(read_pin(WPS_PIN)) {}; 
    }
    
    // Wait for a rising edge
    while(!read_pin(WPS_PIN)) {}; 
    
    TMR0 = 0; // reset timer 0
    while(read_pin(WPS_PIN)) {}; // Wait for the end of the positive pulse
    uint8_t pulse_width = TMR0;
    
    return (pulse_width >= PULSE_WIDTH_MIN_THRES &&
            pulse_width <= PULSE_WIDTH_MAX_THRES);
}

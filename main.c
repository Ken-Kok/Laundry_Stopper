/*
 * File:   main.c
 * Author: kkok9
 *
 * Created on March 26, 2016, 3:49 PM
 */


#include "xc.h"
#include "stdint.h"
#include "stdbool.h"
#include "gpio_drv.h"
#include "wps_drv.h"
#include "global_def.h"

static uint8_t button_flag = 0;
static bool    is_filling  = false;

static uint8_t reset_check()
{
    return SUCCESS;
}

static uint8_t system_init()
{
    pin_init_t pin_init;
    pin_init.ioc_neg_enable = false;
    pin_init.ioc_pos_enable = false;
    pin_init.is_analog = false;
    pin_init.is_input = false;
    pin_init.pin = HOT_SOLENOID_PIN;
    
    init_pin(&pin_init);
    
    pin_init.pin = COLD_SOLENOID_PIN;
    
    init_pin(&pin_init);
    
    pin_init.ioc_neg_enable = true;
    pin_init.is_input = true;
    pin_init.pin = HOT_BUTTON_PIN;
    
    init_pin(&pin_init);
    
    pin_init.pin = COLD_BUTTON_PIN;
    
    init_pin(&pin_init);
    
    pin_init.pin = WARM_BUTTON_PIN;
    
    init_pin(&pin_init);
    
    pin_init.ioc_neg_enable = false;
    pin_init.pin = WPS_PIN;
    
    init_pin(&pin_init);
    
    return SUCCESS;
}

void interrupt general_isr(void)
{
    if(INTCONbits.IOCIF)
    {
        // We are in an interrupt on change ISR
        uint8_t intstat = IOCAF;
        uint8_t port_test = pin_to_port(HOT_BUTTON_PIN);
        
        if(intstat & (1 >> port_test) << port_test)
        {
            button_flag = HOT_BUTTON_PIN;
            return;
        }
        
        port_test = pin_to_port(COLD_BUTTON_PIN);
        if(intstat & (1 >> port_test) << port_test)
        {
            button_flag = COLD_BUTTON_PIN;
            return;
        }
        
        port_test = pin_to_port(WARM_BUTTON_PIN);
        if(intstat & (1 >> port_test) << port_test)
        {
            button_flag = WARM_BUTTON_PIN;
            return;
        }
    }
}

int main(void) 
{ 
    reset_check();
    system_init();
    init_wps_timer();
    
    INTCONbits.GIE = 1;
    
    for(;;)
    {
        if(button_flag == HOT_BUTTON_PIN)
        {
            button_flag = 0;
            is_filling = true;
            set_pin(HOT_SOLENOID_PIN);
        }
        
        if(button_flag == COLD_BUTTON_PIN)
        {
            button_flag = 0;
            is_filling = true;
            set_pin(COLD_SOLENOID_PIN);
        }
        
        if(button_flag == WARM_BUTTON_PIN)
        {
            button_flag = 0;
            is_filling = true;
            set_pin(HOT_SOLENOID_PIN);
            set_pin(COLD_SOLENOID_PIN);
        }
        
        if(is_filling)
        {
            if(is_water_present())
            {
                is_filling = false;
                clear_pin(HOT_SOLENOID_PIN);
                clear_pin(COLD_SOLENOID_PIN);
                
                asm("sleep");
            }
        }
    }
    
    return -1;
}

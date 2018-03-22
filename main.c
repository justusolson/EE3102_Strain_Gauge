/*
 * File:   main.c
 * Author: Justus
 *
 * Created on March 7, 2018, 9:11 PM
 */


#include "xc.h"
#include "ADC.h"
#include "LCD.h"
#include "DELAY.h"

void setup(void){
    CLKDIVbits.RCDIV = 0;
    lcd_init();//on I2C1
    adc_init();//on I2C2
    return;
}

void loop(void){
    char outStrA[8];
    char outStrB[8];
    int adValue;
    
    //adValue = getBuffer(0);
    sprintf(outStrA, "%7.6f", (3.3/1024)*adValue);
    outStrB="Volts";
    lcd_printStrB(outStrA, 0);
    lcd_printStrB(outStrB, 1);
    wait(67);   //roughly 15 times per second which is the rate that ADC samples
}

int main(void) {
    setup();
    while(1){
        loop;
    }
    return 0;
}

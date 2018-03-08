/*
 * File:   ADC.c
 * Author: Justus
 *
 * Created on March 7, 2018, 9:25 PM
 */


#include "xc.h"
#include "ADC.h"

void adc_init(){
    AD1PCFGbits.PCFG4 = 1; //Sets SDA2/SCL2 as digital
    AD1PCFGbits.PCFG5 = 1;
    //RB3 = SCL2, RB2 = SDA2
    TRISBbits.TRISB2 = 0;   //sets SDA2 to output
    TRISBbits.TRISB3 = 0;   //sets SCL2 to output
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 0x9d;      //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
}

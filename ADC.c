/*
 * File:   ADC.c
 * Author: Justus
 *
 * Created on March 7, 2018, 9:25 PM
 */


#include "xc.h"
#include "ADC.h"

#define SLAVE_ADDRESS 0b11010001 //set to read, last bit is R/nW

/*******************************************************************
 * Function: adc_config
 * Parameters: res(resolution): specifies the bit resolution and SPS
 *                  of the ADC.
 *                      12=12 bits and 240SPS
 *                      14=14 bits and 60SPS
 *                      16=16 bits and 15SPS
 *                      defaults to 12
 *              pga: Programmable Gain Amplifier can be set to
 *                  x1, x2, x4, or x8
 *                  defaults to x1
 *              mode: Continuous(0) or One-Shot Mode(1), defaults to
 *                  continuous
 ******************************************************************/
void adc_config(int res, int pga){
    
}

/*******************************************************************
 * Function: adc_init
 * Parameters: None
 * Return: none
 * Description: This function is called from main to initialize all
 *              registers and configuration bits for the ADC to
 *              begin writing voltage measurements to the
 *              Microcontroller
 ******************************************************************/
void adc_init(void){
    AD1PCFGbits.PCFG4 = 1; //Sets SDA2/SCL2 as digital
    AD1PCFGbits.PCFG5 = 1;
    //RB3 = SCL2, RB2 = SDA2
    TRISBbits.TRISB2 = 0;   //sets SDA2 to output
    TRISBbits.TRISB3 = 0;   //sets SCL2 to output
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 0x9d;      //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    
    
    adc_config(16, 2, 0);
}

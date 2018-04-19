/*
 * File:   ADC.c
 * Author: Justus
 *
 * Created on March 7, 2018, 9:25 PM
 */

//Hi Justus

#include <p24FJ64GA002.h>

#include "xc.h"
#include "ADC.h"
#include "math.h"
#include "DELAY.h"
#include <stdlib.h>
#include <stdio.h>

#define READ_SLAVE 0b11010001 //set to read, last bit is R/nW
#define WRITE_SLAVE 0b11010000 //set to write
double LSB_PGA = 0;

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
 *              mode: Continuous(1) or One-Shot Mode(0), defaults to
 *                  continuous
 ******************************************************************/
void adc_config(int res, int pga, int mode){
    unsigned char outputByte = 0b10000000;
    
    if(res==14){//14 bit resolution
        outputByte |= 0b00000100;
        outputByte &= 0b11110111; //sets 0bxxxx01xx
    }
    else if(res==16){//16 bit resolution
        outputByte |= 0b00001000;
        outputByte &= 0b11111011; //sets 0bxxxx10xx
    }
    else{//default to 12 bit
        outputByte &= 0b11110011; //sets 0bxxxx00xx
        res =12;
    }
    
  
    if(pga == 8){   //x8 pga
        outputByte |= 0b00000011;   //sets 0bxxxxxx11
    }
    else if(pga == 4){  //x4 pga
        outputByte |= 0b00000010;
        outputByte &= 0b11111110;   //sets 0bxxxxxx10
    }
    else if(pga == 2){  //x2 pga
        outputByte |= 0b00000001;
        outputByte &= 0b11111101;   //sets 0bxxxxxx01
    }
    else{               //defaults to x1 pga
        outputByte &= 0b11111100;   //sets 0bxxxxxx00
        pga=1;
    }
    
    LSB_PGA = (4.096/ pow(2,res))/pga;
    
    if(mode == 0){ //One-shot mode
        outputByte &= 0b11101111; //sets 0bxxx0xxxx
    }
    else{   //continuous mode (defaults)
        outputByte |= 0b00010000; //sets 0bxxx1xxxx
    }
    //write code to push outputByte to MCP
    
    I2C2CONbits.SEN = 1; //START bit
    IFS3bits.MI2C2IF = 0; //reset
    while(I2C2CONbits.SEN==1);//wait for SEN to clear
    while(IFS3bits.MI2C2IF==0);
    
     
    
    I2C2TRN = WRITE_SLAVE; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    IFS3bits.MI2C2IF = 0; //reset
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    
    I2C2TRN = outputByte;//config byte
    IFS3bits.MI2C2IF = 0; //reset
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    
    I2C2CONbits.PEN = 1;
    IFS3bits.MI2C2IF = 0; //reset
    while(I2C2CONbits.PEN==1);  //wait for stop bit to clear
    while(IFS3bits.MI2C2IF==0);
    IFS3bits.MI2C2IF = 0; //reset
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
    //I2C2CONbits.DISSLW = 0; //enabling slew rate control
    I2C2ADD = 0x0;          //set addressing mode to 7 bits
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 0x4E;      //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    IFS3bits.MI2C2IF = 0; //clear interrupt flag
    I2C2CONbits.ACKDT = 0;  //sends ACK
//    CNPU1bits.CN6PUE = 0;   
//    CNPU1bits.CN7PUE = 0; 
//    CNEN1bits.CN6IE =0;
//    CNEN1bits.CN7IE =0;
    
    /*
     AD1PCFGbits.PCFG4 = 1; //Sets SDA2/SCL2 as digital
    AD1PCFGbits.PCFG5 = 1;
    //RB3 = SCL2, RB2 = SDA2
    TRISBbits.TRISB2 = 0;   //sets SDA2 to output
    TRISBbits.TRISB3 = 0;   //sets SCL2 to output
    I2C2CONbits.DISSLW = 0; //enabling slew rate control
    I2C2ADD = 0x0;          //set addressing mode to 7 bits
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 0x9d;      //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    IFS3bits.MI2C2IF = 0; //clear interrupt flag*/
    
    
    wait(50);
    
    adc_config(16, 1, 1);   //set ADC to 16 bit mode, PGA of 2, Continuous mode
}

/********************************************************************
 * Function: read_adc
 *
 *
 *
 *
 *
 *
 *
 ********************************************************************/

double read_adc(void){
    
    I2C2CONbits.ACKDT = 0;  //sends ACK
    I2C2CONbits.SEN = 1; //START bit
    while(I2C2CONbits.SEN==1);//wait for SEN to clear
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2TRN = READ_SLAVE; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    while(IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2CONbits.RCEN = 1;
    while(!I2C2STATbits.RBF);//while the buffer is not full wait
    
    unsigned char UpperByte, LowerByte;
    UpperByte = I2C2RCV;
    I2C2CONbits.ACKEN=1;
    
    while(I2C2CONbits.ACKEN==1);//wait for ACKEN to clear
    
    I2C2CONbits.RCEN = 1;
    while(!I2C2STATbits.RBF);//while the buffer is not full wait
    
    LowerByte = I2C2RCV;
    I2C2CONbits.ACKDT = 1;  //sends NACK
    I2C2CONbits.ACKEN=1;
    int wholeByte = (UpperByte <<8) + LowerByte;
    double outputVoltage;
    int outputNum;
    
    const int negative = (wholeByte & (1 << 15)) != 0;
    if(negative){
        outputNum = wholeByte | ~((1 << 16) - 1);
    }
    else{
        outputNum = wholeByte;
    }
    
    outputVoltage = outputNum * LSB_PGA;
    
    return outputVoltage;
}
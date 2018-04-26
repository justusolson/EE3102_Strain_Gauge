#include <p24FJ64GA002.h>

#include "xc.h"
#include "ADC.h"
#include "math.h"
#include "DELAY.h"
#include <stdlib.h>
#include <stdio.h>

#define READ_SLAVE 0b11010001 //set to read (last bit is R/nW)
#define WRITE_SLAVE 0b11010000 //set to write
double LSB_PGA = 0; 

void adc_config(int res, int pga, int mode){
    unsigned char outputByte = 0b10000000;
    
    if (res == 14){//14 bit resolution
        outputByte |= 0b00000100;
        outputByte &= 0b11110111; //sets 0bxxxx01xx
    }
    else if (res == 16){//16 bit resolution
        outputByte |= 0b00001000;
        outputByte &= 0b11111011; //sets 0bxxxx10xx
    }
    else {//default to 12 bit
        outputByte &= 0b11110011; //sets 0bxxxx00xx
        res = 12;
    }
    
  
    if (pga == 8){   //x8 pga
        outputByte |= 0b00000011;   //sets 0bxxxxxx11
    }
    else if (pga == 4){  //x4 pga
        outputByte |= 0b00000010;
        outputByte &= 0b11111110;   //sets 0bxxxxxx10
    }
    else if (pga == 2){  //x2 pga
        outputByte |= 0b00000001;
        outputByte &= 0b11111101;   //sets 0bxxxxxx01
    }
    else { //x1 pga (default)
        outputByte &= 0b11111100;   //sets 0bxxxxxx00
        pga = 1;
    }
    
    LSB_PGA = (4.096/pow(2,res))/pga;
    
    if (mode == 0){ //One-shot mode
        outputByte &= 0b11101111; //sets 0bxxx0xxxx
    }
    else {   //continuous mode (default)
        outputByte |= 0b00010000; //sets 0bxxx1xxxx
    }
    
    I2C2CONbits.SEN = 1; //START bit
    IFS3bits.MI2C2IF = 0; //reset
    while (I2C2CONbits.SEN == 1); //wait for SEN to clear
    while (IFS3bits.MI2C2IF == 0);
    
     
    
    I2C2TRN = WRITE_SLAVE; //slave address || write
    I2C2STATbits.ACKSTAT = 1;
    IFS3bits.MI2C2IF = 0; //reset
    while (I2C2STATbits.ACKSTAT == 1);
    while (IFS3bits.MI2C2IF==0); //wait for ACK
    
    I2C2TRN = outputByte; //Configure ADC
    I2C2STATbits.ACKSTAT = 1;
    IFS3bits.MI2C2IF = 0; //reset
    while (I2C2STATbits.ACKSTAT == 1);
    while (IFS3bits.MI2C2IF==0); //wait for it to be 1, ACK
    
    I2C2CONbits.PEN = 1;
    IFS3bits.MI2C2IF = 0; //reset
    while (I2C2CONbits.PEN==1);  //wait for stop bit to clear
    while (IFS3bits.MI2C2IF==0);
    IFS3bits.MI2C2IF = 0; //reset
}

void adc_init(void){
    I2C2CONbits.I2CEN = 0; //disable I2C bus
    I2C2BRG = 39;          //SCL at 100kHz
    I2C2CONbits.I2CEN = 1; //enable I2C bus
    I2C2CONbits.ACKDT = 0; //acknowledge
    
    wait(50); 
    adc_config(16, 1, 1);   //set ADC to 16 bit mode, PGA to 1, Continuous mode
}

double read_adc(void){
    
    I2C2CONbits.SEN = 1; //START bit
    IFS3bits.MI2C2IF = 0; //reset
    while (I2C2CONbits.SEN==1);//wait for SEN to clear
    while( IFS3bits.MI2C2IF==0); //wait for ACK
    
    I2C2TRN = READ_SLAVE; //slave address || read
    I2C2STATbits.ACKSTAT = 1;
    IFS3bits.MI2C2IF = 0; 
    while (I2C2STATbits.ACKSTAT == 1);
    while (IFS3bits.MI2C2IF==0); //wait for ACK
    IFS3bits.MI2C2IF = 0; //reset
    
    I2C2CONbits.RCEN = 1;
    I2C2STATbits.RBF = 0;
    IFS3bits.MI2C2IF = 0;
    while (I2C2CONbits.RCEN == 1);
    while (I2C2STATbits.RBF == 0); //wait while the buffer is not full
    while (IFS3bits.MI2C2IF==0);
    
    unsigned char UpperByte, LowerByte;
    UpperByte = I2C2RCV;
    I2C2CONbits.ACKEN=1;
    IFS3bits.MI2C2IF = 0; //TODO: What is this?
    while (I2C2CONbits.ACKEN==1); //wait for ACKEN to clear
    while (IFS3bits.MI2C2IF==0);
    
    I2C2CONbits.RCEN = 1;
    I2C2STATbits.RBF = 0;
    IFS3bits.MI2C2IF = 0; //TODO: What is this?
    while (I2C2CONbits.RCEN == 1);
    while (I2C2STATbits.RBF == 0);//while the buffer is not full wait
    while (IFS3bits.MI2C2IF==0);
    
    LowerByte = I2C2RCV;
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
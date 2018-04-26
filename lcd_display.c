#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <xc.h>
#define FCY 4000000UL //for __delay_ms() with 4MHz FRC internal clock
#include <libpic30.h>
#include "LCD.h"
#include "DELAY.h"

#define SLAVE_ADDRESS 0b01111100 //set to write, last bit is R/nW
#define CONTROL_ADDRESS 0b000000

unsigned char contrast = 0b00010111; //global variable for constrast



void lcd_cmd(char command)
{
    I2C1CONbits.SEN = 1; //START bit
    while(I2C1CONbits.SEN==1); //wait for SEN to clear
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    IFS1bits.MI2C1IF = 0; //reset
    while (IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = 0b00000000;//control byte
    IFS1bits.MI2C1IF = 0; //reset
    while (IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = command; //data byte
    while (IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1CONbits.PEN = 1; //STOP bit
    while (I2C1CONbits.PEN == 1);//wait for Stop bit to complete
    IFS1bits.MI2C1IF = 0; //reset
}

void lcd_init(void)
{     
    TRISBbits.TRISB9 = 0;   //sets SDA1 to output
    TRISBbits.TRISB8 = 0;   //sets SCL1 to output
    I2C1CONbits.I2CEN = 0;  //disable I2C bus
    I2C1BRG = 39;           //SCL at 100kHz
    I2C1CONbits.I2CEN = 1;  //enable I2C bus
    IFS1bits.MI2C1IF = 0;   //clear interrupt flag
    
    wait(50);            //wait 50ms
    lcd_cmd(0b00111000); //function set, normal instruction mode <--- WHAT IS THE POINT
    lcd_cmd(0b00111001); //function set, extended instruction mode
    lcd_cmd(0b00010100); //internal OSC frequency
    
    unsigned char contrastmanip = contrast &0b00001111; //lower 4 bits
    contrastmanip |= 0b01110000; //results in 0111<C3:C0>
    lcd_cmd(contrastmanip); // Contrast Set
    
    contrastmanip = contrast >> 4; //sets bits C4 and C5 at the end
    contrastmanip &= 0b00000011;
    contrastmanip |= 0b01010100; //results in 010101<C5:C4>
    lcd_cmd(contrastmanip); // Power/ICON/Contrast control
    
    lcd_cmd(0b01101100); //Follower control
    wait(200);
    lcd_cmd(0b00111000); //function set, normal instruction mode
    lcd_cmd(0b00001100); //Display on, cursor off
    lcd_cmd(0b00000001); //clears display
    wait(2);
}

void lcd_setCursor(unsigned char x, unsigned char y)
{//sets cursor to row x column y, x must be 0 or 1, y must be 0 through 7
    unsigned char package = 0b01000000; // left shift by 6
    package &= x<<6;
    y &= 0b00001111;
    package |= y;
    package |= 0b10000000; // after all these operations package is: 1<row 0:1>00 <col 0:7>
    lcd_cmd(package);
}

void lcd_printChar(char myChar)
{
    I2C1CONbits.SEN = 1; //START bit
    while(I2C1CONbits.SEN==1);//wait for SEN to clear
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    IFS1bits.MI2C1IF = 0; //reset
    while(IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = 0b01000000;//control byte, RS =1
    IFS1bits.MI2C1IF = 0; //reset
    while(IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = myChar; //data byte
    IFS1bits.MI2C1IF = 0; //reset
    while(IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1CONbits.PEN = 1; //STOP bit
    while(I2C1CONbits.PEN == 1);//wait for Stop bit to complete
    IFS1bits.MI2C1IF = 0; //reset
}

void lcd_printStr(const char *s)
{
    int i = 0;
    while(i<8)
    {
        lcd_printChar(*s);
        s+=1;
        if(*s == 0)
        {
            break;
        }
        i++;
    }
    lcd_setCursor(1,0);
    while(i<16)
    {
        lcd_printChar(*s);
        s++;
        if(*s == 0)
        {
            break;
        }
        i++;
    }
    
}

void lcd_printStrB(char text[], int row){
    int i = 0;
    lcd_setCursor(row,0);
    lcd_cmdSeqStart(1, text[i]);
    
    for(i = 1; i < 40; i++){
        if(text[i] == 0)
            break;
        lcd_cmdSeqMid(1, text[i]);
    }
    
    lcd_cmdSeqEnd(0, 0b00001100); //Makes sure display is on and sends END sequence
    return;
}

void lcd_cmdSeqStart(int RS, char command){
    I2C1CONbits.SEN = 1; //START bit
    while(I2C1CONbits.SEN==1);//wait for SEN to clear
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = SLAVE_ADDRESS; //8 bits consisting of the salve address and the R/nW bit (0 = write, 1 = read)
    while(IFS1bits.MI2C1IF==0); //wait for it to be 1, ACK
    IFS1bits.MI2C1IF = 0; //reset
    
    I2C1TRN = ((1 << 7) | (RS << 6) | CONTROL_ADDRESS);
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    
    I2C1TRN = command;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    return;
}

void lcd_cmdSeqMid(int RS, char command){
    I2C1TRN = ((1 << 7) | (RS << 6) | CONTROL_ADDRESS);
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    
    I2C1TRN = command;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    return;
    
}

void lcd_cmdSeqEnd(int RS, char command){
    I2C1TRN = ((0 << 7) | (RS << 6) | CONTROL_ADDRESS);
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    
    I2C1TRN = command;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    IFS1bits.MI2C1IF = 0;
    return;
}


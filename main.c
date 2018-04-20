/*
 * File:   main.c
 * Author: Justus
 *
 * Created on March 7, 2018, 9:11 PM
 */


#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>
#include <xc.h>
#define FCY 4000000UL //for __delay_ms() with 4MHz FRC internal clock
#include <libpic30.h>
#include "ADC.h"
#include "LCD.h"
#include "IC.h"


// PIC24FJ64GA002 Configuration Bit Settings
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSECME           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRC           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

int offsetVal;

void setup(void){
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0x9fff;
    offsetVal = 0;
    lcd_init();//on I2C1
    char* test = "LCD up";
    lcd_printStrB(test, 0);
    wait(100);
    adc_init();//on I2C2
//    ic_init();
    return;
}

void loop(void){
    char outStrA[8];
    char outStrB[8]= " Volts";
    double adValue= 0.235;
    
    adValue = read_adc();
    
//    if(checkTare())
//    {
//        offsetVal = adValue;
//    }
    
    if(adValue >= 0){
    sprintf(outStrA, "%7.6f", adValue + offsetVal);
    }
    else{
        sprintf(outStrA, "%7.5f", adValue + offsetVal);
    }
    lcd_printStrB(outStrA, 0);
    lcd_printStrB(outStrB, 1);
    
//    checkTare();s //if this is 1 the button has been pressed
    
    
    wait(67);   //roughly 15 times per second which is the rate that ADC samples
}

int main(void) {
    setup();
    while(1){
        loop();
    }
    return 0;
}

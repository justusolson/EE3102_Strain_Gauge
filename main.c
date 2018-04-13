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
#include <stdio.h>


// PIC24FJ64GA002 Configuration Bit Settings
// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled. 
// Primary Oscillator refers to an external osc connected to the OSC1 and OSC2 pins)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // OSC2/CLKO/RC15 functions as port I/O (RC15)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
// Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

void setup(void){
    CLKDIVbits.RCDIV = 0;
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
    if(adValue >= 0){
    sprintf(outStrA, "%7.6f", adValue);
    }
    else{
        sprintf(outStrA, "%7.5f", adValue);
    }
    lcd_printStrB(outStrA, 0);
    lcd_printStrB(outStrB, 1);
    wait(67);   //roughly 15 times per second which is the rate that ADC samples
}

int main(void) {
    setup();
    while(1){
        loop();
    }
    return 0;
}

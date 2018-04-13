/*
 * File:   IC.c
 * Author: Justus
 *
 * Created on April 12, 2018, 5:03 PM
 */

#include <p24FJ64GA002.h>

#include "xc.h"
#include "ADC.h"
#include "math.h"
#include "DELAY.h"
#include <stdlib.h>
#include <stdio.h>

void ic_init(void)
{

    // add your configuration commands below
    PORTB = 0xffff;
    TRISB = 0x7fff; // bit 15 is an output

    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS
    _IC1R = 2 ;   //IC! <- RB2; See Table 10-2 on P108 of the datasheet
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS
    TMR2 = 0;

    T2CON = 0;
    PR2 = 9999;
    T2CONbits.TON = 1;
    TMR2 = 0;
    
    IC1CON = 0;
    IC1CONbits.ICTMR = 1; // timer 2
    IC1CONbits.ICM = 1;   // capture on every edge

    PR1=65000;      // Timer 1 setup for debugging purposes.
    T1CON=0;
    TMR1 = 104;
    T1CONbits.TON = 1;
    _T1IF = 0;
}

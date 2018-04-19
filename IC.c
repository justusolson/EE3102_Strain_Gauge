/*
 * File:   IC.c
 * Author: Justus
 *
 * Created on April 12, 2018, 5:03 PM
 */


#include "xc.h"
#include "math.h"
#include "DELAY.h"
#include <stdlib.h>
#include <stdio.h>
#include "IC.h"
#include <libpic30.h>

int button;
int checkTare(void)
{
    if(button == PORTBbits.RB6) //check if the button state has changed
        return 0;
    int temp = PORTBbits.RB6;
    int n =0;
    while(n < 4) //this loop checks to see if a button was pressed for a significant amount of time, 4 loops
    { //exits loop when button has been held for 4 loops
        if(PORTBbits.RB6 == temp)  //if the button's state hasn't changed, increment n
            n++;
        else    //if the button's state has changed return no button press
            return 0;
        
        delay_10us();
    }
    button = temp;
    return button;
}

void ic_init(void)
{   
    button = 0;
    // add your configuration commands below
    TRISBbits.TRISB6 = 1;   //set pin to input for TARE
}

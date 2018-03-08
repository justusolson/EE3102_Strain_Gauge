/* 
 * File:   DELAYB.c
 * Author: Justus
 *
 * Created on March 1, 2018, 2:56 PM
 */

#include "xc.h"
#include "DELAY.h"
#include <p24Fxxxx.h>

void wait(int t)
{
    int i = 0;
    while(i<t)
    {
        delay_1ms();
        i++;
    }
}

#include <hidef.h>      /* common defines and macros */
#include <MC9S12C128.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12c128"
#include "termio.h"
#include "stdio.h"

unsigned int dc_dty = 19813;

void main(void) {

TERMIO_Init();
EnableInterrupts;

PWMCTL = 0x10; //concatenate PWM0-1
PWMPOL = 0x03; //channel output start off high for PWM0

PWMPER01 = 40000; //set to 50Hz (20ms period) for DC motor

PWMDTY01 = dc_dty;//16 bit unsigned int
PWME |= 2;//enable

//output on PP1 pin 11

while(1) {}

}


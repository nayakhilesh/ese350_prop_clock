#include <nrk.h>
#include <include.h>
#include <ulib.h>
#include <stdio.h>
#include <hal.h>
#include <nrk_error.h>
#include <nrk_timer.h>
#include <nrk_stack_check.h>
#include <nrk_defs.h>
#include <nrk_cfg.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>//needed? or already in include.h
//#include <stdbool.h>

#define NUM_OF_RADII  30
 
const int rpm = 4500;//assumed, change if needed
//const int numOfLeds = 12;//"
const long clockFrequency = 7372800; 
uint16_t blank = 0x0000;
uint16_t longHand = 0x07FF;
uint16_t shortHand = 0x007F;
uint16_t secondsIndicator = 0x0800;
const long timerThreePrescaler = 64;
 
uint16_t display[NUM_OF_RADII];
 
int overflow0;
int hours, minutes, seconds;
int prevSetHours, prevSetMinutes, prevSetSeconds;
int rps,dps;
int index;

long compareValue;

unsigned char sreg;

void initializeDisplay()
{
    for(int i=0;i<NUM_OF_RADII;i++)
	{
		display[i] = blank;
	}
}

void showDisplay()
{
  //int i;
  for(int i=0;i<NUM_OF_RADII;i++)
  {
	printf("%X \n\r", display[i]);
  }
}

void setDisplay()
{
	cli();
	
	//long a,b;
	//int i;
	
	/*
	//inefficient?
	for(int i=0;i<NUM_OF_RADII;i++)
	{
		display[i] = blank;
	}
    */
	
	//printf("display\n\r");
	//showDisplay();
	
	/*
	a = ((hours*NUM_OF_RADII)/12);
	b = ((minutes*NUM_OF_RADII)/720);
	display[(a + b/* + ((seconds*NUM_OF_RADII)/(360*120)) *//*)] = shortHand;//seconds too small to make a difference
	
	a = ((minutes*NUM_OF_RADII)/60);
	b = ((seconds*NUM_OF_RADII)/3600);	  	  
    display[(a + b)] |= longHand;
	
	a = ((seconds*NUM_OF_RADII)/60);		 
	display[a] |= secondsIndicator;
	*/
	
	
	display[(((prevSetHours*NUM_OF_RADII)/12) + ((prevSetMinutes*NUM_OF_RADII)/720))] = blank;
	display[(((prevSetMinutes*NUM_OF_RADII)/60) + ((prevSetSeconds*NUM_OF_RADII)/3600))] = blank;
	display[((prevSetSeconds*NUM_OF_RADII)/60)] = blank;
	
	
	display[(((hours*NUM_OF_RADII)/12) + ((minutes*NUM_OF_RADII)/720))] = shortHand;
	display[(((minutes*NUM_OF_RADII)/60) + ((seconds*NUM_OF_RADII)/3600))] |= longHand;
	display[((seconds*NUM_OF_RADII)/60)] |= secondsIndicator;
	
	
	prevSetHours = hours;
	prevSetMinutes = minutes;
	prevSetSeconds = seconds;
	
	
	//printf("updated display \n\r");
	//showDisplay();
	
	sei();

}

ISR(TIMER0_OVF_vect)
{
     
     //TIFR0 = 0x01;//clear TOV0 bit(flag) - not required
	 //printf("TIFR0 = %X \t", TIFR0);
	 //sei();
	 //printf("SREG = %X \t", SREG);
	 //printf("TCNT0 = %d \t", TCNT0);//%u ?
     //printf("inside interrupt 0 ISR \n\r");
     overflow0++;
     //printf("%d \n\r", overflow);
     if(overflow0 > 27) //was 449
	 {
       overflow0 = 0;
	   seconds++;
	 }
	 if(seconds > 59)
	 {
	   seconds = 0;
	   minutes++;
	 }
	 if(minutes > 59)
	 {
	   minutes = 0;
	   hours++;
	 }
     if (hours > 11)
	 {
	   hours = 0;
	 } 
	 //printf("TCNT0 = %d \n\r", TCNT0);//%u ?
	 //printf("TIFR0 = %X \t", TIFR0);
	 //printf("SREG = %X \n\r", SREG);
	 //setDisplay();
	 //printf("time is %d : %d : %d \n\r", hours, minutes, seconds);
	 //printf("mark \n\r");
	 //setDisplay();
	 //TCNT0 = 0;
}

ISR(TIMER4_OVF_vect)
{
	//TIFR4 = 0x01;//clear TOV4 bit(flag) - not required
    printf("inside interrupt 4 ISR \n\r");
	printf("TCNT4 = %u \n\r", TCNT4);
	//TCNT4 = 0xFB00;
	printf("TCNT4 = %u \n\r", TCNT4);
}

ISR(TIMER3_COMPA_vect)
{
	//printf("inside interrupt 3 ISR \n\r");
    
	//setDisplay();
	
	
	//actual propeller code:
	PORTF = (display[index] >> 4);
	PORTB = ((display[index] & 0x000E) << 4);
	PORTE = ((display[index] & 0x0001) << 6);//propeller LEDs
	
	
	//simulated propeller
	//PORTE &= ~((display[index] & 0x0001) << 2);//for inverse logic wired board LEDs //LED0
	
	//testing
	/*
	if ((index % 2) == 0)
	{
	PORTE &= 0xC3;
	}
	else
	{
	PORTE |= 0x3C;
	}
	*/
	
	
	index++;
	if(index > (NUM_OF_RADII - 1))
	{
	  index = 0;
	}
	//printf("index = %d \n\r", index);
}

int
main ()
{
 nrk_setup_ports();
 nrk_setup_uart(UART_BAUDRATE_115K2);
 
 overflow0 = 0;
 hours = 0;
 minutes = 0;
 seconds = 0;
 prevSetHours = 0;
 prevSetMinutes = 0;
 prevSetSeconds = 0;
 index = 0;
 //rpm = 4500;
 rps = (rpm/60);
 dps = ((rpm*NUM_OF_RADII)/60);
 
 long c = (NUM_OF_RADII*((long)(rps))*timerThreePrescaler);
 //printf("c = %ld \n\r", c);
 
 //compareValue = (clockFrequency/c);
 compareValue = 383;
 //setDisplay();
 initializeDisplay();
 
 //printf("compareValue = %ld", compareValue); 
 
 printf("\n\r");
 //printf("SREG = %X ", SREG);
 //SREG |= 0x80; //this statement and sei(); have the same effect on SREG
 sei();
 //printf("SREG = %X ", SREG);
 
//Port Setting   
 DDRE |= 0x7C;
 PORTE &= 0xFB;//for LEDs on board //switches on LED0
 DDRF = 0xFF;
 PORTF = 0xFF;
 DDRB |= 0xE0;
 PORTB |= 0xE0;
//ends here
 
 printf("here \t");

//Timer/Counter0 Setting
 TIFR0 = 0x01;//clears flag
 TCNT0 = 0;//sets initial value
 TCCR0A &= 0x00;
 //TCCR0B &= 0xF1;
 TCCR0B = 0x05;//prescaler = 1024 (was 64)
 TIMSK0 = 0x01;//enable interrupt
//ends here

//Timer/Counter4 Setting (before above block?)
 PRR1 &= 0xEF;
 TIFR4 = 0x01;//clears flag
 
 sreg = SREG;
 cli();
 TCNT4 = 0;//sets initial value (16 bit)
 SREG = sreg;
 
 TCCR4A = 0x00;
 TCCR4B = 0x01;//prescaler = 1
 TCCR4C = 0x00;
 //TIMSK4 = 0x01;//enable interrupt//deactivated
//ends here

//Timer/Counter3 Setting (before above block?)
 PRR1 &= 0xF7;
 TIFR3 = 0x02;//clears flag
 
 sreg = SREG;
 cli();
 TCNT3 = 0;//sets initial value (16 bit)
 SREG = sreg;
 
 TCCR3A = 0x00;
 //TCCR3B = 0x09;//prescaler = 1
 //TCCR3B = 0x0D;//prescaler = 1024
 //TCCR3B = 0x0C;//prescaler = 256
 TCCR3B = 0x0B;//prescaler = 64
 TCCR3C = 0x00;
 
 OCR3AH = ((compareValue >> 8) & 0xFF);
 OCR3AL = (compareValue & 0xFF);
 
 TIMSK3 = 0x02;//enable interrupt
//ends here

 //printf("Hello \n\r");
 //printf("rps = %d ", rps);
 //printf("TCNT0 = %d \n\r", TCNT0);//%u ?

 //printf("here too \n\r");
 
 while(1)
 {
   //printf("here three");
   setDisplay();
 }

}

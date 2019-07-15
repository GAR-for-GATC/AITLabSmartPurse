/*
 * rotaryEncoder.c
 *
 * Created: 6/20/2019 3:51:35 PM
 *  Author: Rawr
 
 https://www.embeddedrelated.com/showarticle/158.php
 https://www.embeddedrelated.com/showarticle/530.php
 https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */ 

#include "include.h"

void initEncoder(uint8_t *aLastState){
	//define 2 pins on ATMega328pu as input for 
	//rotary encoder outputs.
	//Use PD5 and PD6, or pins 11 and 12.
	//PD5 is outputB and PD6 is outputA
	//use PD7, or pin 13, as the button pin.
	
	
	//uint16_t aLastState;
	
	DDRD &= ~0b11100000; //set pins 5, 6, 7 to inputs.
	*aLastState = PIND & (1<<PD5);//read pin 6
	return;
}

uint8_t readEncOutputA(){
	//return PIND & (1<<PD5);
	if(PIND & (1<<PD5)){
		return 0b00000001;
	}
	return 0b00000000;
}
uint8_t readEncOutputB(){
	//return PIND & (1<<PD6);
	if(PIND & (1<<PD6)){
		return 0b00000001;
	}
	return 0b00000000;
}
/////////////////////////////////////////////////////////////////////
//https://www.avr-tutorials.com/interrupts/avr-external-interrupt-c-programming
//https://arduino.stackexchange.com/questions/32572/how-to-detect-the-direction-of-a-rotary-encoder
//same thing but using interrupts
//pin PD5 will generate the interrupt, pin PD6 and PD7 wont.

//volatile is used if the variable might change unexpectedly.
//volatile uint8_t firedInt;
//volatile uint8_t upInt;



void encoderInitWithInt(){
	DDRD &= ~((1<<DDB5)|(1<<DDB6)|(1<<DDB7));	//set pins 5, 6, 7 to inputs.
							//doesnt need pullups for 5 and 6, since there are hardware ones.
	PORTD |=(1<<PORTD7);//enable pullup on pin PD7
	//DDRD &= ~(DDB5);
	//enable interrupts on PD5 (pin 11/PCINT21/OC0B/T1)
	PCICR |= (1<<PCIE2); //enable interrupts on PCINT[23 to 16]
	PCMSK2 |= (1<<PCINT21); //enable interrupts on PCINT21
	sei(); //enables global interrupts				
	//by default, a change on PD5 will cause an interrupt, rising and falling edge interrupts
	//appear to only work on int0 and int1
	
}










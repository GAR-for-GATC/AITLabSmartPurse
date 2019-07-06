/*
 * bluetoothModule.c
 *
 * Created: 6/22/2019 9:38:58 PM
 *  Author: Rawr
 */ 
/*
The following code uses the JY-MCU bluetooth module V1.06
Since it uses the TX and RX pins, you cannot use the USART.c
functions.

After testing, use a 3V (or 3.3V) pullup on the TX and RX lines.
The JY-MCU outputs one ASCII code for each input sent to it from the
serial monitor on the phone used for testing.  
commands found here:
http://byron76.blogspot.com/2011/09/one-board-several-firmwares.html
*/
#include "include.h"

//initialize RX and TX pins for bluetooth module.
//code modified from arduino's SoftwareSerial module found here:
//https://github.com/PaulStoffregen/SoftwareSerial
void bluetoothModuleInit(){
	//set TX pin (PD1) to output
	//set RX pin (PD0) to input.
	//DDRD |= (1<<PD1);
	//DDRD &= ~(1<<PD0);
	//PORTD |=(1<<PORTD0); //enable 10k pullup on input.
	Init_USART();
}

//uses polling to get byte.
uint8_t bluetoothRecieve(){
	return receiveByte();
}

//resets name and pin number
//name: AITSmartPurse
//pin: 8385
//take letters and numbers and transmit as
//ascii.
//https://www.rapidtables.com/convert/number/ascii-to-hex.html
void bluetoothModleSet(){
	const char setName[] = "AT+NAMEAITSmartPurse";
	const char setPin[] = "AT+PIN8385";
	printString(setName);
	_delay_ms(10); 
	printString(setPin);
	_delay_ms(10);
}

//enables interrupts for RX pins.
//https://www.avrfreaks.net/forum/tut-soft-using-usart-interrupt-driven-serial-comms?page=all
//http://ebook.pldworld.com/_Semiconductors/Atmel/Databook%20CDROM/Atmel/acrobat/doc1451.pdf
void bluetoothModuleEnableInterrupt(){
	//enable interrupts on PD0 (PCINT16/RXD)
	//PCICR |= (1<<PCIE2); //enable interrupts on PCINT[23 to 16]
	//PCMSK2 |= (1<<PCINT16); //enable interrupts on PCINT16
	UCSR0B |= (1<<RXCIE0);	//enable interrupts for receive port
	sei(); //enables global interrupts
}
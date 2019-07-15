
//i2c is used to interface with the oled board.
/*
	i2c notes, from: http://www.embedds.com/programming-avr-i2c-interface/
	
	i2c = two wire interface/twi
	Both Sda and Scl linees are pulled up to vcc.
		Each communication is preceded by a start signal and a stop signal, generated
	by the master.  Both of these are done by pulling SDA low and SCL high.
		Each packet has 4 parts:
		1) start signal
		2) address packet:  seven address bits lead by data direction bit 
			(read or write) + acknowledge bit;
		3) Data packet: eight data bits + acknowledge bit;
		4) Stop signal.
		
		Each byte sent is acknoleged by pulling the SDA line low
		
		i2c has 4 different modes:
		1) Master Transmitter ? initiates transfer sends data to slave device;
		2) Master Receiver ? initiates transfer reads data from slave device;
		3) Slave Transmitter ? waits for master request and then sends data;
		4) Slave Receiver ? waits for master transmission and accepts data.
		
			The SCL and SDA lines are open collector lines, need to be pulled up.
		The pullups have a range.
*/

#include "include.h"

void twiInit(void){
	//set SCL to 400khz
	TWSR = 0x00;	//This is the status register.  Sets prescaler value to 1 and clears the
					// status register.  The status register does stuff, but the datasheet isnt
					// specific.
	TWBR = 0x0C;	//This is the bit rate register and sets the division factor for the bit rate
					// generator.  formula is: 
					//	SCL frequency = clock frequency (16Mhz) / 16 + ((2*TWBR)*(prescale value))
					// 16mill / 16 + ((2*12)*1) = 16mill/40 = 400,000kHz
					// try 100,000khz, 16mill/x = 100,000. x = 160.  160 = 16 + (2*x)*y, same as y*x = 72.
					//x = 72.  0100 1000, 0x48
	//Enable twi
	TWCR = (1<<TWEN);	// When bit2 (TWEN) is on, TWI is enabled.
}

//generates start signal, waits for acknowledge
void twiStart(void){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Interrupt bit (TWINT) needs to be set to send start, as
											// well as a slew of others.  TWSTA set to one generates
											// a start at the first convenience.
	while((TWCR & (1<<TWINT)) == 0);	//The TWINT bit is set after the TWI has finished its current job,
										// in this case, sending a start condition.
}
//generates stop signal
void twiStop(void){
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //TWSTO to 1 generates a stop signal.
}

//writes to the TWDR register, which is shifted to the SDA line
void twiWrite(uint8_t u8data){
	TWDR = u8data; //holds the next byte to be transmitted.
	TWCR = (1<<TWINT)|(1<<TWEN); //sends byte, datasheet isnt clear how this sends the byte.
	while ((TWCR & (1<<TWINT)) == 0); //wait until ack signal, when TWINT is set to 1.
}

//twi read functions
uint8_t twiReadAck(void){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
//read byte with no ack
uint8_t twiReadNoAck(void){
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
//get status
uint8_t twiGetStatus(void){
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;	//masks the status register.  Values are found here: http://www.atmel.com/webdoc/AVRLibcReferenceManual/ch20s33s01.html
							// Statuses are found in the AVR toolchain, under <util/twi.h>
	return status;
}

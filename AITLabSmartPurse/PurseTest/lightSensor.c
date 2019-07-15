/*
 * lightSensor.c
 *
 * Created: 6/16/2019 4:43:05 PM
 *  Author: Rawr
 */ 
#include "include.h"
/* 
Light sensor code for veml7700 on an adafruit board. 


*/

void veml7700_Write(uint8_t c, uint16_t data){
	uint8_t veml7700Address = 0x20;	//address + write bit, write bit is 0, address is 0b0010000 then add 0x10
	twiStart();
	twiWrite(veml7700Address); 
	//command codes are 16 bits long, but only 1 byte is used.
	//twiWrite(0x00);
	twiWrite(c);
	twiWrite (data);//send LSB
	twiWrite(data>>8); //send MSB
	twiStop();
};

uint16_t veml7700_Read(uint8_t c){
	uint16_t returnValue = 0;
	uint8_t veml7700AddressWr = 0x20;
	uint8_t veml7700AddressRd = 0x21; //address + read bit, 0b0010000 + 0x01
	
	twiStart();
	twiWrite(veml7700AddressWr);
	//twiWrite(0x00);
	twiWrite(c);
	twiStart();
	twiWrite(veml7700AddressRd);
	returnValue |= twiReadAck();
	
	returnValue |= ((uint16_t)twiReadNoAck()) << 8;
	twiStop();
	return returnValue;
}

void veml7700Init(){
	
	//set register 0,
	//bits 15-13 are 0, 
	//bits 12-11 set ALS gain to lowest setting to 1/8 gain
	//will be changed if lower than 100 counts than increase gain.
	//0b10 gives a 1/8 gain.
	//bit 10 is set to 0.
	//bits 9-6 are the ALS integration time setting.
	//since we don't need very high accuracy for this application, the integration time
	//is going to be very fast, 25mS.  This translates to 0b1100
	//bits 5 and 4 set the ALS persistence protect number setting.
	//used for interrupts with the light sensor, set to 0b00
	//bits 3 and 2 are set to 0
	//bit 1 enables interrupts, set to 0 to disable.
	//bit 0 "shuts down" the light sensor when set to 1 to save power.
	//send command 0b000 10 0 1100 00 00 0 0
	//0b0001001100000000 = 0b0001 0011 0000 0000 = 0x1300
	veml7700_Write(0x00, 0x1300);
	
	
	//set high and low values
	//veml7700_Write(0x01, 0x0000);
	//veml7700_Write(0x02, 0xFFFF);
	
	//disable power saving mode
	veml7700_Write(0x03, 0x0000);
	
	_delay_ms(3);
	
	
	return;	
}























/*
Pseudocode for light sensor control:
1) When purse is opened, send power-on command to the Control Register.
This is an I2C write command.
2) There is a 0.4 second delay before data is available, this is more than the
50mS time it takes for a human to notice a change in something and isn't very good.
Test the design to make sure that this delay is acceptable.  Leaving the device in power
on mode would waste too much power from the batteries.  Maybe the timing setting
can be altered.
3) then send a command to read from the Ch0 lower register address.
4) then send a command to read from the Ch0 upper register address.
5) then send commands to read from the Ch1 lower and upper register addresses.

Do this once on uC initialization:
Configure the timing register:
since the gain can be done in the uC, check what values that come from the
sensor to see what kind of values are obtained from the same light source.
*/
/*
void TSL2561_command(uint8_t c){
	uint8_t address = 0x39;
	
	twiStart();
	twiWrite(address);
	twiWrite(c);
	twiStop();
}

uint8_t TSL2561_read8bits(uint8_t r){
	uint8_t address = 0x39;
	uint8_t returnStuff = 0;
	twiStart();
	twiWrite(address);
	twiWrite(r);
	returnStuff = twiReadAck();
	twiStart();
	twiWrite(address);
	returnStuff = twiReadAck();
	twiStop();
	return returnStuff;
}

uint8_t lightSensorInit(){
	//	send register ID, 0x0A, used to get the part number, 0000 means TSL2560
	//and 0001 means 2561
	
	
	uint8_t purple = 5;
	purple= TSL2561_read8bits(0x0A);
	
	return purple;
}

*/
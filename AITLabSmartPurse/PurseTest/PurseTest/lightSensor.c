/*
 * lightSensor.c
 *
 * Created: 6/16/2019 4:43:05 PM
 *  Author: Rawr
 */ 
#include"include.h"
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
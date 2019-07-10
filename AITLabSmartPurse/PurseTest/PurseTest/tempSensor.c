/*
 * tempSensor.c
 *
 * Created: 7/9/2019 6:58:19 PM
 *  Author: Rawr
 */ 

/*
code for the TC74 temperature sensor:
http://ww1.microchip.com/downloads/en/DeviceDoc/21462D.pdf
*/

//since the part number for this instance is A0, the
//address is: 0b1001000

//activates the temperature sensor, wakes from sleep mode.
//use this function to initialize the sensor.
void tc74StandbyMode(){
	uint8_t tc74Address = 0b01001000;	//address + write bit, write bit is 0, address is 0b0100100 0 
	twiStart();
	twiWrite(tc74Address);
	//command codes are 8 bits long
	twiWrite(0x01);	//command to write to the CONFIG register
	twiWrite(0b10000000);//send MSB
	twiStop();
	_delay_ms(250); //delay to wait for the DATA_RDY pin to set so temp data can be read.
}

uint8_t tc74ReadTemp(){
	uint8_t returnValue = 0;
	uint8_t tc74AddressWr = 0b001001000;
	uint8_t tc74AddressRd = 0b001001001; //address + read bit, 0b0100100 + 0x01
	
	twiStart();
	twiWrite(tc74AddressWr);
	twiWrite(c);
	twiStart();
	twiWrite(tc74AddressRd);
	returnValue = twiReadNoAck();
	twiStop();
	return returnValue;
}
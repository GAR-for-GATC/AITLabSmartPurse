/*
This program uses a light sensor to modulate the intensity of an LED
using PWM.  It also turns on when either a hall sensor is triggered,
or capacitive touch.

The chipset used for this example is the ATMEGA328P
The light sensor used is the TSL2561, but this light sensor should
be swapped out, since this light sensor will be discontinued soon.
found here: https://www.digikey.com/product-detail/en/ams/TSL2561T/TSL2561-TTR-ND/3095179
The hall effect sensor is a DRV5032, chosen since it's low power.
https://www.digikey.com/product-detail/en/DRV5032FBDBZR/296-47323-1-ND/7688333?utm_medium=email&utm_source=oce&utm_campaign=2578_OCE19RT&utm_content=productdetail_US&utm_cid=560602&so=59927227&mkt_tok=eyJpIjoiWVRjMVlUSmxPVEZqTjJVMiIsInQiOiJFNGRDWDU1U3ZkcFpFaUZ0Y3BMRGw1N1NBTGVTMUlUalJlUjJMRFJoUklhMWI2dEZFMTkzNFZvdVlMdDlnZ2IzOW5nK2RiUW1xdG8zWWw4WnFtaHFDenNwa2JUWTMzZ2NZTHEzazBnaTE5RkdNYkFtRFB1eTJsZG1UWVJvU0EydiJ9
Edit: not sure if it'll work, only using it because I had some laying around.
The indexable LEDs are COM-11821, or WS2812, or "Neopixels", which 
are commonly used indexable RGB LED lights, easy to implement, but are 
more expensive than other RGB LED lights.
----------------------------------------------------------------------------
The RGB LED uses a non-return to zero communication protocol.
The input supply is 5V.  Each packet is 24 bits long.
For multiple LEDs that are wired together, only one command needs
to be issued, then after that the LEDs send the command to the next
LEDs.
Each 24 bit packet is RGB data with 8 bit sections for Green, Red,
and Blue data respectively.
A "0" is 0.35uS+/-150nS at 5V followed by 0.8uS+/-150nS on the Din line
A "1" is 0.7uS+-150nS at 5V followed by 0.6u+/-150nS on the Din line
----------------------------------------------------------------------------
Edit: New light sensor will be used, disregard following text

The light sensor uses I2C.
Has an interrupt feature so you don't have to continuously poll the sensor.
Shouldn't be needed since the sensor will only be used when it's sensed
that the purse is open.  In other words, the sensor will be polled while
the purse is open.  
	The sensor uses 3V power supply, so multiple power
supplies will need to be used.  During the final implementation, either
find a new light sensor and/or use different LEDs that all use the same 
input voltage.  This implementation will use a logic level changing chip since 
I have one laying around that changes 5V to 3V as well as a linear power 
regulator for 3V.
	The I2C address depends on how the address select pin is pulled.  Have to 
look at the board for which address to use.  
	The MSB on each 8bit command needs to  be a "1".  Lower 4 bits are the 
register bits, which is the register destination.  When a I2C write or read
cmd is sent, then the sensor will send data until a NACK is sent by the master.

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

----------------------------------------------------------------------------

Setup:
Datasheet found here:
http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf

Pinout for ATMEGA328-PU
1)       (PCINT14/RESET) PC6
2)         (PCINT16/RXD) PD0
3)         (PCINT17/TXD) PD1
4)        (PCINT18/INT0) PD2
5)   (PCINT19/OC2B/INT1) PD3
6)      (PCINT20/XCK/T0) PD4
7)                       VCC
8)                       GND
9)  (PCINT6/XTAL1/TOSC1) PB6
10) (PCINT7/XTAL2/TOSC2) PB7
11)    (PCINT21/OC0B/T1) PD5
12)	 (PCINT22/OC0A/AIN0) PD6
13)	      (PCINT23/AIN1) PD7
14)   (PCINT0/CLKO/ICP1) PB0

28) PC5 (ADC5/SCL/PCINT13)
27) PC4 (ADC4/SDA/PCINT12)
26) PC3 (ADC3/PCINT11)
25) PC2 (ADC2/PCINT10)
34) PC1 (ADC1/PCINT9)
23) PC0 (ADC0/PCINT8)
22) GND
21) AREF
20) AVCC
19) PB5 (SCK/PCINT5)
18) PB4 (MISO/PCINT4)
17) PB3 (MOSI/OC2A/PCINT3)
16) PB2 (SS/OC1B/PCINT2)
15) PB1 (OC1A/PCINT1)

use linear lm317 regulator to get 3v from a 9v battery.
use the arduino board as a bootloader for breadboard chip
https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard

Use pins 27 and 28 for i2c, which is renamed "twi" to avoid
copyright stuff.

pullup resistor calculation:
from atmega328pu datasheet, the pullup resistor values should be:
Rmin = (Vcc-0.4V)/(3*10^-3A) = (3V-0.4V)/(3*10^-3A) 
Rmax(frequency < 100kHz) = 1000nS/Cb
Rmax(frequency > 100kHz) = 300ns/Cb
Cb = capacitance of a bus line in pF  
https://www.youtube.com/watch?v=ktud8JJU-Bk

use LM317 for power:
https://www.onsemi.com/pub/Collateral/LM317-D.PDF

vout = 1.25(1+(R2/R1))
use 240ohms for R1, then for Vout=3V,
then use ~545ish ohms for R2

======================================================================
To run the AVR at 3V, change the fuse bits to run at 8Mhz, 


*/



#include "include.h"
volatile uint8_t encoderDirection=0; //shows direction
volatile uint8_t encoderTurned=0; //set when encoder is turned
volatile uint8_t bluTest = 0;
volatile uint8_t bluTest2 = 0;
volatile uint16_t bluTest3 = 0;
uint8_t USART_RX_Buffer[16];	//buffer for USART
uint8_t USART_RX_BufferMask = 15; //16-1
volatile uint8_t UART_RxHead = 0;
volatile uint8_t UART_RxTail = 0;
//volatile int encoderCounter = 0;

volatile ringBuffer usartRingBuff;
//volatile ringBuffer * rbPointer = &usartRingBuff;
volatile uint8_t uartIn = 0; //1 if uart input is sensed


//interrupt declaration needs to be here
ISR(PCINT2_vect);
ISR(USART_RX_vect);

int main (void){
	ringBuffInit_v(&usartRingBuff);
	
	//turn on LED attached to pin PB0
	//used to see if chip is working
	PORTB |= (1<<PB0);
	DDRB ^= (1<<DDB0);	
	
	encoderInitWithInt();
	twiInit();	
	initDisplay();
	bluetoothModuleInit();
	//bluetoothModleSet();  //used to set the name and pin of the bluetooth module
	bluetoothModuleEnableInterrupt();
	testDisplay(); //displays lines on the screen.
	refreshDisplay(); //updates the screen.
		
	char bewp[1] = "6";	
	multilineScrollPrint(bewp, (uint16_t) 1);	
	refreshDisplay();	
	
	char myBuffer[30] = {0};	//used for the floatToString function.
	char *myValue;
	
	//buffer used for printing.
	ringBuffer printBuffer;
	ringBuffInit(&printBuffer);
	uint8_t printBuffIn = 0; //1 if stuff in buffer
		
	_delay_ms(1000);
	//turn on temp sensor:
	tc74StandbyMode();
	uint8_t tempReading = 0;

	while(1){
		tempReading = tc74ReadTemp();
		
		myValue = floatToString((float)tempReading, myBuffer, (uint8_t) 0);
		multilineScrollPrint(myValue, (uint16_t) 8);
		refreshDisplay();
		
		_delay_ms(5000);
	}
	
	//initialize Light Sensor.
	veml7700Init();
	_delay_ms(1000);
	uint16_t lightReading = 0;
	uint8_t getALScmd = 0x04;
	uint8_t getWHITEcmd = 0x05;
	while(1){
		lightReading = veml7700_Read(getALScmd);
		
		myValue = floatToString(((float)lightReading)*1.8432, myBuffer, (uint8_t) 0);
		multilineScrollPrint(myValue, (uint16_t) 8);
		refreshDisplay();
		
		_delay_ms(200);
		
		
	}
		
	
	/*
	change colours over time.
	make a sine wave that changes over a time for different colours:
	https://www.instructables.com/id/How-to-Make-Proper-Rainbow-and-Random-Colors-With-/
	rather than using taylor expansion or CORDIC to generate a sine wave, a lookup table
	is easier and more efficient.
	Use 0-170, since the sin wave in this situation only needs to last 1/3 of the whole 255.
	first map 0-170 onto 0-pi, so that each increment of 1 in 0 to 170 is (pi/170) radians.
	Then take the sine of each one and make a lookup table for each value from 0 to 170
	where each value is equal to y=sine(pi/170 * x)
	where x is from 0 to 170.  Then map the results which will be from 0-1 to 0-255.
	This will get a single sin arc that is 170 units long and has a max height of 255 units.
	This was done by typing "Table[ sin(((pi/170)*x))*255.0 , {x, 170}]"
	into wolfram alpha and generating a table of values:
	
	{4.71212, 9.42263, 14.1299, 18.8324, 23.5284, 28.2164, 32.8948, 37.5619, 42.2162, 46.8561, 
	51.48, 56.0863, 60.6735, 65.2399, 69.7841, 74.3044, 78.7993, 83.2674, 87.707, 92.1166, 
	96.4948, 100.84, 105.151, 109.426, 113.663, 117.862, 122.02, 126.137, 130.211, 134.24, 
	138.224, 142.16, 146.047, 149.885, 153.672, 157.406, 161.086, 164.712, 168.281, 171.792, 
	175.245, 178.638, 181.971, 185.241, 188.447, 191.59, 194.667, 197.677, 200.62, 203.494, 
	206.299, 209.034, 211.697, 214.288, 216.805, 219.249, 221.618, 223.911, 226.127, 228.267, 
	230.328, 232.311, 234.214, 236.038, 237.78, 239.442, 241.022, 242.519, 243.934, 245.266, 
	246.513, 247.677, 248.756, 249.749, 250.658, 251.481, 252.218, 252.869, 253.434, 253.912, 
	254.304, 254.608, 254.826, 254.956, 255, 254.956, 254.826, 254.608, 254.304, 253.912, 253.434, 
	252.869, 252.218, 251.481, 250.658, 249.749, 248.756, 247.677, 246.513, 245.266, 243.934, 
	242.519, 241.022, 239.442, 237.78, 236.038, 234.214, 232.311, 230.328, 228.267, 226.127, 
	223.911, 221.618, 219.249, 216.805, 214.288, 211.697, 209.034, 206.299, 203.494, 200.62, 
	197.677, 194.667, 191.59, 188.447, 185.241, 181.971, 178.638, 175.245, 171.792, 168.281, 
	164.712, 161.086, 157.406, 153.672, 149.885, 146.047, 142.16, 138.224, 134.24, 130.211, 
	126.137, 122.02, 117.862, 113.663, 109.426, 105.151, 100.84, 96.4948, 92.1166, 87.707, 
	83.2674, 78.7993, 74.3044, 69.7841, 65.2399, 60.6735, 56.0863, 51.48, 46.8561, 42.2162, 
	37.5619, 32.8948, 28.2164, 23.5284, 18.8324, 14.1299, 9.42263, 4.71212, 0}

		
	Then feed this into a python script and get rid of everything after the decimal point.	
	This was done in python 2.9 with the script:
	myList = [...]
	outputList = []
	for i in myList:
		outputList.append(int(i))
	print outputList

	note: you don't need both sides of the sine table, just one side.  Read the one side
	backwards to get a decreasing slope.
	*/
	
	
	/*
	uint8_t sinLookupTable[] = {4, 9, 14, 18, 23, 28, 32, 37, 42, 46, 51, 56, 60, 65, 69, 74, 
		78, 83, 87, 92, 96, 100, 105, 109, 113, 117, 122, 126, 130, 134, 138, 142, 146, 149, 
		153, 157, 161, 164, 168, 171, 175, 178, 181, 185, 188, 191, 194, 197, 200, 203, 206, 
		209, 211, 214, 216, 219, 221, 223, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242, 
		243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 253, 254, 254, 254, 254, 255, 
		254, 254, 254, 254, 253, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 
		241, 239, 237, 236, 234, 232, 230, 228, 226, 223, 221, 219, 216, 214, 211, 209, 206, 
		203, 200, 197, 194, 191, 188, 185, 181, 178, 175, 171, 168, 164, 161, 157, 153, 149, 
		146, 142, 138, 134, 130, 126, 122, 117, 113, 109, 105, 100, 96, 92, 87, 83, 78, 74, 
		69, 65, 60, 56, 51, 46, 42, 37, 32, 28, 23, 18, 14, 9, 4, 0};
	
	uint8_t ledct = 0;
	uint8_t myred = 0;
	uint8_t myblue = 0;
	uint8_t mygreen = 0;
	uint8_t tC[] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220};
	while(1){
		//setPixelColor(ledct, myred, mygreen, myblue, &testRGBLED);
		for(uint8_t i=0; i<12; i++){
			if(tC[i] == 255){tC[i] = 0;}
			if(tC[i] < 85){
				myred = sinLookupTable[tC[i]+84];//decreasing slope
				myblue = 0;
				mygreen = sinLookupTable[tC[i]];//increasing slope
				setPixelColor(i, myred, mygreen, myblue, &testRGBLED);
			}else if(tC[i] >=85 && tC[i] <170){
				myred = 0;
				myblue = sinLookupTable[tC[i]-84];//increasing slope
				mygreen = sinLookupTable[tC[i]];//decreasing slope
				setPixelColor(i, myred, mygreen, myblue, &testRGBLED);
			}else if(tC[i]>=170){
				myred = sinLookupTable[tC[i]-170];//increasing slope
				myblue = sinLookupTable[tC[i]-85];//decreasing slope
				mygreen = 0;
				setPixelColor(i, myred, mygreen, myblue, &testRGBLED);
			}
			tC[i]++;			
		}
		_delay_ms(15);
		rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
	}
	*/
		
	USART_Buffer_Flush();
	uint8_t printBufferOut0 = 0;
	uint8_t printBufferOut1 = 0;
	uint8_t printBufferOut2 = 0;
	uint8_t printBufferOut3 = 0;
	uint8_t colourUpdater = 0;
	uint8_t redValue = 128;
	uint8_t greenValue = 0;
	uint8_t blueValue = 127;
	
	rgbLEDs testRGBLED;
	uint16_t testNumber = 12;
	uint8_t testOutput = rgbLEDInit(&testRGBLED, testNumber);
	
	uint8_t myBrightness = 1;
	setBrightness(myBrightness, &testRGBLED);
	for(uint16_t i=0; i<12; i++){
		setPixelColor(i, 0, 0, 0, &testRGBLED);
	}
	//setPixelColor(0, redValue, greenValue, blueValue, &testRGBLED);
	
	
	rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
	
	for(uint16_t i=0; i<12; i++){
		setPixelColor(i, redValue, greenValue, blueValue, &testRGBLED);
	}
	setBrightness(myBrightness, &testRGBLED);
	rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
	while(1){
		
		
		//clear stuff out of the usart buffer and put it in 
		//the print buffer.
		/*
		if(usartRingBuff.count != 0){			
			while(usartRingBuff.count >0){
				ringBuffPut(&printBuffer, ringBuffGet_v(&usartRingBuff));
			}
			//printBuffIn = 1;			
			uartIn = 0;
			//bluTest2 = 0;
			//USART_Buffer_Flush();
		}*/
		
		if(usartRingBuff.count != 0){
			while(usartRingBuff.count > 0){
				
				
				printBufferOut0 = ringBuffGet_v(&usartRingBuff);
				//myValue = floatToString((float)printBufferOut0, myBuffer, (uint8_t) 0);
				//multilineScrollPrint(myValue, (uint16_t) 3);
				//refreshDisplay();
				
				
				if(printBufferOut0 == 1){
					//update red values
					//values are given as input = 2n+m
					//n is the first value given and m is the second value given.
					while(usartRingBuff.count == 0){}//wait for buffer to fill 
					printBufferOut0 = ringBuffGet_v(&usartRingBuff);
					colourUpdater = printBufferOut0 + printBufferOut0;
					while(usartRingBuff.count == 0){}
					printBufferOut0 = ringBuffGet_v(&usartRingBuff);
					colourUpdater = colourUpdater + printBufferOut0;
					while(usartRingBuff.count == 0){}
					printBufferOut0 = ringBuffGet_v(&usartRingBuff); //get the zero out of the buffer.
					redValue = colourUpdater;
					//myValue = floatToString((float)colourUpdater, myBuffer, (uint8_t) 0);
					//multilineScrollPrint(myValue, (uint16_t) 3);
					//refreshDisplay();
					
					for(uint16_t i=0; i<12; i++){
						setPixelColor(i, redValue, greenValue, blueValue, &testRGBLED);
						//setPixelColor(i, 0, 0, 200, &testRGBLED);
					}			
					
					
				}
				/*else if(printBufferOut0 == 2){
					//update blue values
					
				}else if(printBufferOut0 == 3){
					//update green values
				}else if(printBufferOut0 == 4){
					//update brightness values
					
				}else if(printBufferOut0 ==5){
					//turn light on and off
					
				}*/
				rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
				//used for debugging:
				myValue = floatToString((float)colourUpdater, myBuffer, (uint8_t) 0);
				multilineScrollPrint(myValue, (uint16_t) 3);
				refreshDisplay();
			}			
		}
		
		
		
		if(encoderTurned == 1){
			if(encoderDirection == 1){
				//lower brightness
				(myBrightness > 1 ) ? (myBrightness-=5) : (myBrightness = 1);
				if(myBrightness == 0){myBrightness = 1;}
				setBrightness(myBrightness, &testRGBLED);
				//update colours with brightness, if not then colours that have a 
				//value less than 128 will not appear on higher brightness.
				//colours not appearing only happens at a brightness of 1.
				for(uint16_t i=0; i<12; i++){
					setPixelColor(i, redValue, greenValue, blueValue, &testRGBLED);
				}
				rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
				
				//encoderCounter++;
				myValue = floatToString((float)myBrightness, myBuffer, (uint8_t) 0);
				multilineScrollPrint(myValue, (uint16_t) 3);
				//multilineScrollPrint("0", (uint16_t) 1); //0 for counter-clockwise
				refreshDisplay();
			}else{
				//turn brightness up
				(myBrightness < 251) ? (myBrightness+=5) : (myBrightness = 255);
				if(myBrightness == 0){myBrightness = 1;}
				setBrightness(myBrightness, &testRGBLED);
				for(uint16_t i=0; i<12; i++){
					setPixelColor(i, redValue, greenValue, blueValue, &testRGBLED);
				}
				rgbLEDshow(testRGBLED.numBytes,(uint8_t *) (testRGBLED.pixels), testRGBLED.pinMask);
				
				//encoderCounter--;
				myValue = floatToString((float)myBrightness, myBuffer, (uint8_t) 0);
				multilineScrollPrint(myValue, (uint16_t) 3);
				//multilineScrollPrint("1", (uint16_t) 1);	//1 for clockwise
				refreshDisplay();
			}
			encoderTurned = 0;
			
		}		
	}	
	
	return 0;
}

//interrupt needs to be here
ISR(PCINT2_vect){
	if(readEncOutputA()==1){
		encoderDirection = readEncOutputB();
	}else{
		encoderDirection = !(readEncOutputB());
	}/*
	if(encoderDirection == 1){
		encoderCounter++;
	}else{
		encoderCounter--;
	}*/
	encoderTurned = 1;
}

ISR(USART_RX_vect){
	ringBuffPut_v(&usartRingBuff, UDR0);
	uartIn = 1;	
}




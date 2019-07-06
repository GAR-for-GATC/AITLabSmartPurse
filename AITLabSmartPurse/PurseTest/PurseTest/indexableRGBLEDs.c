/*
 * indexableRGBLEDs.c
 *
 * Created: 7/3/2019 6:00:07 PM
 *  Author: Rawr
 
 The neopixels run fine at 3.3V with a 5V signal.  They'll probably be able to run fine 
 off of a 3V signal as well.
 At a 3V power source, the neopixels start sputtering and flashing when they shouldnt.
 Code is a modified version of the Adafruit Neopixel library 
 
 apparently the order that the RGBW data is sent depends on the different 
models or versions or revisions of the rgbLED in question for the WS---- kind
of LEDs (neopixels).

at 3V the neopixels seem to run fine.
 */ 

#include "include.h"

//give the function a pointer to a rgbLEDs struct and
//the number of LEDs in the chain of LEDs
uint8_t rgbLEDInit(rgbLEDs* myLEDs, uint16_t n){
	
	
	DDRD |= (1<<DDD4);	//set PD4 as an output.
	PORTD &= ~(1<<PD4);	//set output to low.
	
	myLEDs->brightness = 0;
	myLEDs->endTime=0;
	
	uint8_t t = NEO_GRB + NEO_KHZ800;
	myLEDs -> wOffset = (t >> 6) & 0b11; // See notes in header file
	myLEDs -> rOffset = (t >> 4) & 0b11; // regarding R/G/B/W offsets
	myLEDs -> gOffset = (t >> 2) & 0b11;
	myLEDs -> bOffset =  t       & 0b11;
	
	//use this to find the number of bytes long the "pixels" buffer needs to be.
	//with the 12 LED ring light, it's 36
	myLEDs -> numBytes = n * ((myLEDs -> wOffset == myLEDs -> rOffset) ? 3 : 4);
	
	for(uint8_t i=0; i<sizeof(myLEDs->pixels); i++){
		myLEDs->pixels[i] == 0;
	}
	return myLEDs->numBytes;
	
	//figured out this from:
	//https://garretlab.web.fc2.com/en/arduino/inside/arduino/Arduino.h/digitalPinToBitMask.html
	//same as: pinMask = digitalPinToBitMask(p); in adafruit cpp code.
	myLEDs->pinMask = (1<<PD4);
	
	//port    = portOutputRegister(digitalPinToPort(p));
	myLEDs->port = (&PORTD);
	
	
}

void rgbLEDshow(uint16_t numBytes, uint8_t* pixels, uint8_t pinMask){
	//apparently this refreshes the LEDs
	
	
	_delay_us(500);
	
	volatile uint8_t n1, n2 = 0;  // First, next bits out

	// Squeezing an 800 KHz stream out of an 8 MHz chip requires code
	// specific to each PORT register.

	// 10 instruction clocks per bit: HHxxxxxLLL
	// OUT instructions:              ^ ^    ^   (T=0,2,7)

	// PORTD OUTPUT ----------------------------------------------------
	
	volatile uint16_t
	i   = numBytes; // Loop counter
	volatile uint8_t
	*ptr = pixels,   // Pointer to next byte
	b   = *ptr++,   // Current byte value
	hi,             // PORT w/output bit set high
	lo;             // PORT w/output bit set low

	// Hand-tuned assembly code issues data to the LED drivers at a specific
	// rate. There's separate code for different CPU speeds (8, 12, 16 MHz)
	// for both the WS2811 (400 KHz) and WS2812 (800 KHz) drivers. The
	// datastream timing for the LED drivers allows a little wiggle room each
	// way (listed in the datasheets), so the conditions for compiling each
	// case are set up for a range of frequencies rather than just the exact
	// 8, 12 or 16 MHz values, permitting use with some close-but-not-spot-on
	// devices (e.g. 16.5 MHz DigiSpark). The ranges were arrived at based
	// on the datasheet figures and have not been extensively tested outside
	// the canonical 8/12/16 MHz speeds; there's no guarantee these will work
	// close to the extremes (or possibly they could be pushed further).
	// Keep in mind only one CPU speed case actually gets compiled; the
	// resulting program isn't as massive as it might look from source here.
	hi = PORTD |  pinMask;
	lo = PORTD & ~pinMask;
	n1 = lo;
	if(b & 0x80) n1 = hi;

	// Dirty trick: RJMPs proceeding to the next instruction are used
	// to delay two clock cycles in one instruction word (rather than
	// using two NOPs). This was necessary in order to squeeze the
	// loop down to exactly 64 words -- the maximum possible for a
	// relative branch.

	asm volatile(
	"headD:"                   "\n\t" // Clk  Pseudocode
	// Bit 7:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
	"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 6"        "\n\t" // 1-2  if(b & 0x40)
	"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 6:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
	"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 5"        "\n\t" // 1-2  if(b & 0x20)
	"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 5:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
	"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 4"        "\n\t" // 1-2  if(b & 0x10)
	"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 4:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
	"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 3"        "\n\t" // 1-2  if(b & 0x08)
	"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 3:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
	"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 2"        "\n\t" // 1-2  if(b & 0x04)
	"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 2:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
	"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 1"        "\n\t" // 1-2  if(b & 0x02)
	"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"rjmp .+0"                "\n\t" // 2    nop nop
	// Bit 1:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n2]   , %[lo]"    "\n\t" // 1    n2   = lo
	"out  %[port] , %[n1]"    "\n\t" // 1    PORT = n1
	"rjmp .+0"                "\n\t" // 2    nop nop
	"sbrc %[byte] , 0"        "\n\t" // 1-2  if(b & 0x01)
	"mov %[n2]   , %[hi]"    "\n\t" // 0-1   n2 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"sbiw %[count], 1"        "\n\t" // 2    i-- (don't act on Z flag yet)
	// Bit 0:
	"out  %[port] , %[hi]"    "\n\t" // 1    PORT = hi
	"mov  %[n1]   , %[lo]"    "\n\t" // 1    n1   = lo
	"out  %[port] , %[n2]"    "\n\t" // 1    PORT = n2
	"ld   %[byte] , %a[ptr]+" "\n\t" // 2    b = *ptr++
	"sbrc %[byte] , 7"        "\n\t" // 1-2  if(b & 0x80)
	"mov %[n1]   , %[hi]"    "\n\t" // 0-1   n1 = hi
	"out  %[port] , %[lo]"    "\n\t" // 1    PORT = lo
	"brne headD"              "\n"   // 2    while(i) (Z flag set above)
	: [byte]  "+r" (b),
	[n1]    "+r" (n1),
	[n2]    "+r" (n2),
	[count] "+w" (i)
	: [port]   "I" (_SFR_IO_ADDR(PORTD)),
	[ptr]    "e" (ptr),
	[hi]     "r" (hi),
	[lo]     "r" (lo));
	
	_delay_us(500);
}

void setBrightness(uint8_t b,rgbLEDs* myLEDs) {
	// Stored brightness value is different than what's passed.
	// This simplifies the actual scaling math later, allowing a fast
	// 8x8-bit multiply and taking the MSB. 'brightness' is a uint8_t,
	// adding 1 here may (intentionally) roll over...so 0 = max brightness
	// (color values are interpreted literally; no scaling), 1 = min
	// brightness (off), 255 = just below max brightness.
	uint8_t newBrightness = b + 1;
	if(newBrightness != myLEDs->brightness) { // Compare against prior value
		// Brightness has changed -- re-scale existing data in RAM,
		// This process is potentially "lossy," especially when increasing
		// brightness. The tight timing in the WS2811/WS2812 code means there
		// aren't enough free cycles to perform this scaling on the fly as data
		// is issued. So we make a pass through the existing color data in RAM
		// and scale it (subsequent graphics commands also work at this
		// brightness level). If there's a significant step up in brightness,
		// the limited number of steps (quantization) in the old data will be
		// quite visible in the re-scaled version. For a non-destructive
		// change, you'll need to re-render the full strip data. C'est la vie.
		uint8_t  c,
		*ptr = myLEDs->pixels,
		oldBrightness = myLEDs->brightness - 1; // De-wrap old brightness value
		uint16_t scale;
		if(oldBrightness == 0) scale = 0; // Avoid /0
		else if(b == 255) scale = 65535 / oldBrightness;
		else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;
		for(uint16_t i=0; i<myLEDs->numBytes; i++) {
			c = *ptr;
			*ptr++ = (c * scale) >> 8;
		}
		myLEDs->brightness = newBrightness;
	}
}

void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, rgbLEDs* myLEDs) {

	if(n < myLEDs->numLEDs) {
		if(myLEDs->brightness) { // See notes in setBrightness()
			r = (r * myLEDs->brightness) >> 8;
			g = (g * myLEDs->brightness) >> 8;
			b = (b * myLEDs->brightness) >> 8;
		}
		uint8_t *p;
		if(myLEDs->wOffset == myLEDs->rOffset) { // Is an RGB-type strip
			p = &(myLEDs->pixels[n * 3]);    // 3 bytes per pixel
			} else {                 // Is a WRGB-type strip
			p = &(myLEDs->pixels[n * 4]);    // 4 bytes per pixel
			p[myLEDs->wOffset] = 0;        // But only R,G,B passed -- set W to 0
		}
		p[myLEDs->rOffset] = r;          // R,G,B always stored
		p[myLEDs->gOffset] = g;
		p[myLEDs->bOffset] = b;
	}
}


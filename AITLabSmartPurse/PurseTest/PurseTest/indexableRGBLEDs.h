/*
 * indexableRGBLEDs.h
 *
 * Created: 7/3/2019 6:59:12 PM
 *  Author: Rawr
 */ 


#ifndef INDEXABLERGBLEDS_H_
#define INDEXABLERGBLEDS_H_
	
	#define NEO_GRB  ((1<<6) | (1<<4) | (0<<2) | (2)) ///< Transmit as G,R,B
	#define NEO_KHZ800 0x0000
	
	typedef struct rgbLEDs{
		uint16_t          numLEDs;    ///< Number of RGB LEDs in strip
		uint16_t          numBytes;   ///< Size of 'pixels' buffer below
		int8_t            pin;        ///< Output pin number (-1 if not yet set)
		uint8_t           brightness; ///< Strip brightness 0-255 (stored as +1)
		
		//the number of bytes in buffer is the number of pixels*4 or *3
		uint8_t pixels[36];     ///< Holds LED color values (3 or 4 bytes each)
		uint8_t           rOffset;    ///< Red index within each 3- or 4-byte pixel
		uint8_t           gOffset;    ///< Index of green byte
		uint8_t           bOffset;    ///< Index of blue byte
		uint8_t           wOffset;    ///< Index of white (==rOffset if no white)
		uint32_t          endTime;    ///< Latch timing reference
		
		volatile uint8_t *port;       ///< Output PORT register
		uint8_t           pinMask;    ///< Output PORT bitmask
	}rgbLEDs;

	uint8_t rgbLEDInit(rgbLEDs* myLEDs, uint16_t n);
	void rgbLEDshow(uint16_t numBytes, uint8_t* pixels, uint8_t pinMask);
	void setBrightness(uint8_t b,rgbLEDs* myLEDs);
	static uint32_t rgbColor(uint8_t r, uint8_t g, uint8_t b) {
		return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
	}
	void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b, rgbLEDs* myLEDs);
	void changeBrightness(rgbLEDs* myLEDs, uint8_t updown);
	
#endif /* INDEXABLERGBLEDS_H_ */
/*
 * rotaryEncoder.h
 *
 * Created: 6/20/2019 3:51:53 PM
 *  Author: Rawr
 */ 


#ifndef rotaryEncoder
#define rotaryEncoder
	//volatile uint8_t firedInt;
	//volatile uint8_t upInt;
	
	void initEncoder(uint8_t *aLastState);
	uint8_t readEncOutputA(void);
	uint8_t readEncOutputB(void);
	void encoderInitWithInt(void);
	

#endif /* ROTARYENCODER_H_ */
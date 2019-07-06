/*
 * i2c.h
 *
 * Created: 12/3/2016 5:43:43 PM
 *  Author: Rawr
 */ 


#ifndef I2C_H_
#define I2C_H_

	void twiInit(void);
	void twiStart(void);
	void twiStop(void);
	void twiWrite(uint8_t u8data);
	uint8_t twiReadAck(void);
	uint8_t twiReadNoAck(void);
	uint8_t twiGetStatus(void);

#endif /* I2C_H_ */
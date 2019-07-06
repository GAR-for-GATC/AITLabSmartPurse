/*
 * lightSensor.h
 *
 * Created: 6/16/2019 4:43:20 PM
 *  Author: Rawr
 */ 


#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_


void TSL2561_command(uint8_t c);
uint8_t TSL2561_read8bits(uint8_t r);
uint8_t lightSensorInit();




#endif /* LIGHTSENSOR_H_ */
/*	This file has all of the includes used in this program in one place.
This makes it easy to add more libraries to include by only having to
edit one part of the program.
*/

#ifndef include
#define include

#include "globals.h"
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <stdio.h>
#include <stdlib.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include "USART.h"
//#include "SPI.h"
#include <util/delay.h>
#include "oled_library.h"
#include "i2c.h"
#include "lightSensor.h"
#include "rotaryEncoder.h"
#include "bluetoothModule.h"
#include "ringBuffer.h"
#include "indexableRGBLEDs.h"



#endif
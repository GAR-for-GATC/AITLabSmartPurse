/*
 * bluetoothModule.h
 *
 * Created: 6/22/2019 9:39:07 PM
 *  Author: Rawr
 */ 


#ifndef BLUETOOTHMODULE_H_
#define BLUETOOTHMODULE_H_
	
	//#define _SS_MAX_RX_BUFF 64
	//static char _receive_buffer[_SS_MAX_RX_BUFF];
	//static volatile uint8_t _receive_buffer_tail;
	//static volatile uint8_t _receive_buffer_head;
	
	void bluetoothModuleInit(void);
	uint8_t bluetoothRecieve(void);
	void bluetoothModleSet(void);
	void bluetoothModuleEnableInterrupt(void);

#endif /* BLUETOOTHMODULE_H_ */
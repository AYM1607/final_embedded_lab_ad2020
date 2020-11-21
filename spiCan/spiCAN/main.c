/*
 * spiCAN.c
 *
 * Created: 11/20/2020 6:28:13 PM
 * Author : A00819720
 */ 


#include "sam.h"
#include "uart.h"
#include "myprintf.h"
#include "spi.h"

#include "mcp_can.h"
#include "mcp_can_dfs.h"

#define CAN0_SPEED CAN_250KBPS

uint16_t canId = 0;
uint8_t dataLength = 0;
uint8_t canData[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	
void delay(void) {
	uint32_t i;
	for(i = 0; i <10000000; i++) {
		;
	}
}

void clearCanData() {
	uint8_t i;
	for (i=0;i<8;i++) {
		canData[i] = 0xFF;
	}
}

// Min speed 0, max speed 3000.
void setEngineSpeed(uint16_t speed) {
	speed /= 0.16;
	clearCanData();
	canData[3] = speed & 0xFF;
	canData[4] = (uint8_t)(speed >> 8);
	sendMsgBuf(0x0CF004A3, 1, 8, canData);
}

void setFuelLevel(uint8_t percentage) {
	percentage /= 0.4;
	clearCanData();
	
	canData[1] = percentage;
	sendMsgBuf(0x18FEFCA3,1, 8, canData);
}

// Possible values 0 or 1
void setOilLampValue(uint8_t value) {
	clearCanData();
	
	canData[7] = value;
	sendMsgBuf(0x18FF14A3, 1, 8, canData);
}

int main(void)
{
	uint8_t ret;
	
    /* Initialize the SAM system */
    SystemInit();
	SYSCTRL->OSC8M.bit.PRESC = 0;
	initUART();
	spiInit();
	
	canBegin(SLAVE_CAN_0, CAN0_SPEED);
	
	// Turn off oil lamp.
	setOilLampValue(0);
	// Set fuel high;
	setFuelLevel(100);
	// Set rmp low.
	setEngineSpeed(0);
	
	// delay;
	uint32_t i;
	for(i = 0; i < 30000000; i++) {
		;
	}
	
	// Set rmp high.
	setEngineSpeed(3000);
	// Set fuel.
	setFuelLevel(0);
	
	for(i = 0; i <10000000; i++) {
		;
	}
	
	// Turn on oil lamp.
	setOilLampValue(1);

	return;
	
	/* Code to read messages
	readMsgBufID(&canId, &dataLength, canData);

	myprintf("Received message. Id: %d, Data length: %d, Data:", canId, dataLength);
	uint8_t i = 0;
	for(i = 0; i < dataLength; i++) {
		myprintf(" %d", canData[i]);
	}
	myprintf("\r\n");
	*/
}

#include "sam.h"
#include "spi.h"
#include "myprintf.h"




void spiInit(void) {
	// Configure the slave pin as an output.
	// Clear the PMUXEN bit.
	PORT->Group[0].PINCFG[PIN_PA18].reg = 0;
	// Declare it as output.
	PORT->Group[0].DIRSET.reg = PORT_PA18;
	
	SYSCTRL->OSC8M.bit.PRESC = 0; // Disable preescaler.
	
	PM->APBCMASK.bit.SERCOM1_ = 1; // Enable the clock for SERCOM1 peripheral.
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM1_CORE;
	while( GCLK->STATUS.bit.SYNCBUSY );
	
	//structures to configure the SERCOM1 peripheral
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0, // MSB first
		.bit.CPHA = 0, // Mode 0
		.bit.CPOL = 0,
		.bit.FORM = 0, // SPI frame
		.bit.DIPO = 3, // MISO on PAD[3]
		.bit.DOPO = 0, // MOSI on PAD[0], SCK on PAD[1], SS_ on PAD[2]
		.bit.MODE = 3  // Master Mode
	};
	SERCOM1->SPI.CTRLA.reg = ctrla.reg;
	
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.RXEN = 1,   // RX enabled
		.bit.MSSEN = 0,  // Manual SC
		.bit.CHSIZE = 0  // 8-bit
	};
	SERCOM1->SPI.CTRLB.reg = ctrlb.reg;
	
	//Formula to configure the desired baudrate
	uint32_t br = ( double )( 8000000 / ( 2 * BAUDRATE ) ) - 1 ;
	SERCOM1->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD( ( uint8_t )br );

	//structure to configure multiple PINs
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA16C_SERCOM1_PAD0 | MUX_PA17C_SERCOM1_PAD1 | MUX_PA19C_SERCOM1_PAD3,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK = ( uint16_t )( ( PORT_PA16 | PORT_PA17 | PORT_PA19 ) >> 16 )
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;

	//SS/CS (Slave Select/Chip Select) PIN 18 configuration
	REG_PORT_DIRSET0 = PORT_PA18;
	REG_PORT_OUTSET0 = PORT_PA18;
		
	//enable the SPI
	SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
	while( SERCOM1->SPI.SYNCBUSY.bit.ENABLE );
}

uint8_t spiSend( uint8_t data ) {
	uint8_t temp;
	while( !SERCOM1->SPI.INTFLAG.bit.DRE ); //wait until buffer is empty
	SERCOM1->SPI.DATA.reg = SERCOM_SPI_DATA_DATA( data ); //transmit data
	while( !SERCOM1->SPI.INTFLAG.bit.RXC ); //wait until a data is received
	temp = SERCOM1->SPI.DATA.reg; //read data
	while( !SERCOM1->SPI.INTFLAG.bit.TXC ); //wait until there is no data to transmit
	return temp;
}

uint8_t spiSS(uint8_t device) {
	switch (device) {
		case SLAVE_CAN_0:
			REG_PORT_OUTCLR0 = PORT_PA18;
			break;
		default:
			break;	
	}
	return 1;
}

uint8_t spiSR(uint8_t device) {
	switch (device) {
		case SLAVE_CAN_0:
			REG_PORT_OUTSET0 = PORT_PA18;
			break;
		default:
			break;
	}
	return 1;
}

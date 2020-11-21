#ifndef SPI_H
#define SPI_H

#define BAUDRATE 9600
#define SLAVE_CAN_0 0

void spiInit(void);
uint8_t spiSend(uint8_t data);
uint8_t spiSS(uint8_t device);
uint8_t spiSR(uint8_t device);

#endif // SPI_H
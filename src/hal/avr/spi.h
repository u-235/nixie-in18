#ifndef HAL_AVR_SPI_H_
#define HA_AVRL_SPI_H_

#include <stdint.h>

//! Размер массива данных, передаваемых по SPI.
#define SPI_ARRAY_SIZE 9

void spi_init();
void  spi_clean();
void spi_send_array(uint8_t data[]);

#endif // HAL_AVR_SPI_H_
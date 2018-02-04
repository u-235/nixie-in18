#ifndef HAL_AVR_SPI_H_
#define HA_AVRL_SPI_H_

#include <stdint.h>

//! Размер массива данных, передаваемых по SPI.
#define SPI_ARRAY_SIZE 9

#ifdef __cplusplus
extern "C" {
#endif

void spi_init();
void spi_clean();
void spi_send_array(uint8_t data[]);

#ifdef __cplusplus
}
#endif

#endif // HAL_AVR_SPI_H_

#ifndef HAL_SPI_H_
#define HAL_SPI_H_

//! Размер массива данных, передаваемых по SPI.
#define SPI_ARRAY_SIZE 9

void spi_init();
void  spi_clean();
void spi_send_array(unsigned char data[]);

#endif // HAL_SPI_H_

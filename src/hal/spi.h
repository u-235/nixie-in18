#ifndef _SPI_H_
#define _SPI_H_

/// Размер массива данных, передаваемых по SPI.
#define SPI_ARRAY_SIZE 9

void spi_init();
void spi_send_array(unsigned char data[]);

#endif // _SPI_H_

/**
 * \file
 * \brief Модуль SPI
 * \details Программный интерфейс для работы с SPI.
 *
 * \date создан 31.12.2014
 * \author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef HAL_AVR_SPI_H_
#define HAL_AVR_SPI_H_

#include <stdint.h>

/* Размер массива данных, передаваемых по SPI. */
#define SPI_ARRAY_SIZE 9

#ifdef __cplusplus
extern "C" {
#endif

void spi_init(void);
void spi_clean(void);
void spi_send_array(uint8_t data[]);

#ifdef __cplusplus
}
#endif

#endif // HAL_AVR_SPI_H_

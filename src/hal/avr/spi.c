/**
 * \file
 * \brief Модуль SPI
 * \details Программный интерфейс для работы с SPI. Предполагается
 * использование SPI только для вывода на индикатор.
 *
 * Отличительной особенностью является алгоритм ожидания передачи байта. Если
 * в даташите сначала отсылают байт, затем ждут флаг SPIF, то здесь байт
 * отсылается только после ожидания флага.
 *
 * \date создан 31.12.2014
 * \author Nick Egorrov
 */

#include <avr/io.h>
#include "../../bit-helper.h"
#include "../../config.h"
#include "spi.h"

#define _spi_wait(d)  while (!(SPSR & 1 << SPIF)) {}

/**
 * Инициализация порта B и модуля SPI.
 */
void spi_init()
{
        _dir_out(CFG_SPI_SS);
        _pin_off(CFG_SPI_SS);
        _dir_out(CFG_SPI_MOSI);
        _dir_out(CFG_SPI_SCK);
        _dir_out(CFG_SPI_LOAD);
        /* Разрешает работу в качестве мастера с двойной скоростью. */
        SPCR = 1 << SPE | 1 << MSTR;
        SPSR = 1 << SPI2X;

        spi_clean();
}

/**
 * Отправка массива данных.
 * \param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void spi_send_array(uint8_t data[])
{
        // Отправляем первый байт без опроса флага SPIF,
        // потому что он сбрасывается при формировании
        // строба фиксации данных в регистрах сдвига.
        SPDR = *data++;
        for (uint8_t i = 0; i < SPI_ARRAY_SIZE - 1; i++) {
                uint8_t byte = *data++;
                _spi_wait();
                SPDR = byte;
        }

        // Фиксация данных в регистрах сдвига.
        _pin_off(CFG_SPI_LOAD);
        _spi_wait();
        _pin_on(CFG_SPI_LOAD);
}

/**
 * Очистка дисплея.
 */
void spi_clean()
{
        SPDR = 0;
        for (uint8_t i = 0; i < SPI_ARRAY_SIZE - 1; i++) {
                _spi_wait();
                SPDR = 0;
        }

        _pin_off(CFG_SPI_LOAD);
        _spi_wait();
        _pin_on(CFG_SPI_LOAD);
}

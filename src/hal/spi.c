#include <avr/io.h>
#include "spi.h"
#include "asm.h"

/**
 * \file spi.c
 * \brief Модуль SPI
 * \details Программный интерфейс для работы с SPI. Предполагается
 * использование SPI только для вывода на индикатор.
 *
 * Отличительной особенностью является алгоритм ожидания передачи
 * байта. Если в даташите сначала отсылают байт, затем ждут флаг
 * SPIF, то здесь байт отсылается только после ожидания флага.
 */

#define _strobe_up(d) PORTB |= 1 << DDB2
#define _strobe_down(d) PORTB &= ~(1 << DDB2)
#define _spi_wait(d)  while (!(SPSR & 1 << SPIF)) {}

/**
 * Инициализация порта B и модуля SPI.
 */
void spi_init()
{
        // Пин 2 используется как строб параллельной загрузки регистров.
        DDRB |= 1 << DDB5 | 1 << DDB3 | 1 << DDB2;
        // Разрешает работу в качестве мастера.
        SPCR = 1 << SPE | 1 << MSTR;
        // Двойная скорость SPI.
        SPSR = 1 << SPI2X;

        // Отправка байта необходима для установки флага SPIF.
        SPDR = 0;
        spi_clean();
}

/**
 * Отправка массива данных.
 * \param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void spi_send_array(uint8_t data[])
{
        _strobe_up();
        for (uint8_t i = 0; i < SPI_ARRAY_SIZE; i++) {
                uint8_t byte = *data++;
                _spi_wait();
                SPDR = byte;
        }
        _strobe_down();
}

/**
 * Очистка дисплея.
 */
void spi_clean()
{
        _strobe_up();
        for (uint8_t i = 0; i < SPI_ARRAY_SIZE; i++) {
                _spi_wait();
                SPDR = 0;
        }
        _strobe_down();
}

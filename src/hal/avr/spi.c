#include <avr/io.h>
#include "spi.h"

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

#define _strobe_up(d) PORTC |= 1 << DDC0
#define _strobe_down(d) PORTC &= ~(1 << DDC0)
#define _spi_wait(d)  while (!(SPSR & 1 << SPIF)) {}

/**
 * Инициализация порта B и модуля SPI.
 */
void spi_init()
{
        // Порт C: вывод 3 для данных и вывод 5 для тактирования.
        DDRB |= 1 << DDB5 | 1 << DDB3;
        // Вывод 0 порта C используется как строб параллельной загрузки регистров.
        DDRC |= 1 << DDC0;
        // Разрешает работу в качестве мастера.
        SPCR = 1 << SPE | 1 << MSTR;
        // Двойная скорость SPI.
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
        _strobe_down();
        _spi_wait();
        _strobe_up();
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

        _strobe_down();
        _spi_wait();
        _strobe_up();
}

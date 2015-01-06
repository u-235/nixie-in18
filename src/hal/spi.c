#include <avr/io.h>
#include "spi.h"
#include "asm.h"

/**
 * Инициализация порта B и модуля SPI.
 */
void spi_init()
{
        // Пин 2 используется как строб паралельной загрузки регистров.
        DDRB |= 1 << DDB5 | 1 << DDB3 | 1 << DDB2;
        // Разрешает работу в качестве мастера.
        SPCR = 1 << SPE | 1 << MSTR;
        // Двойная скорость SPI.
        SPSR = 1 << SPI2X;
}

/**
 * Отправка массива данных.
 * \param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void spi_send_array(unsigned char data[])
{
        for (char i = 0; i < SPI_ARRAY_SIZE; i++) {
                SPDR = *data++;
                while (!(SPSR & 1 << SPIF)) {
                        // nop
                }
        }

        PORTB |= 1 << DDB2;
        _nop;
        _nop;
        _nop;
        _nop;
        PORTB &= ~(1 << DDB2);
}

/**
 * Очистка дисплея.
 */
void  spi_clean(){
        for (char i = 0; i < SPI_ARRAY_SIZE; i++) {
                SPDR = 0;
                while (!(SPSR & 1 << SPIF)) {
                        // nop
                }
        }

        PORTB |= 1 << DDB2;
        _nop;
        _nop;
        _nop;
        _nop;
        PORTB &= ~(1 << DDB2);
}

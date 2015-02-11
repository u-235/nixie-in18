#include <avr/io.h>
#include "../display.h"
#include "spi.h"
#include "pwmi.h"

static unsigned char data[SPI_ARRAY_SIZE];

/**
 * Инициализация индикации.
 */
void dislpay_init()
{
        spi_init();
        pwmi_init();
        display_bright(DISPLAY_BRIGHT_MAX);
        display_rate(DISPLAY_RATE_MAX);
}

/**
 * Очистка буфера дисплея. Последующий вызов функции display_flush() погасит
 * все цифры, разделители и значки.
 */
void display_clean()
{
        unsigned char *p = data;

        for (char i = 0; i < SPI_ARRAY_SIZE; i++) {
                *p++ = 0;
        }
}

/**
 * Отправка накопленных изменений в дисплей.
 */
void display_flush()
{
        pwmi_load(data);
}

/**
 * Подготавливает изменения показа дня недели. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param d Показываемый день недели, может быть любым сочетанием следующих
 *      флагов:
 *      - #DISPLAY_MARK_MONDAY
 *      - #DISPLAY_MARK_TEUSDAY
 *      - #DISPLAY_MARK_WEDNESDAY
 *      - #DISPLAY_MARK_THURSDAY
 *      - #DISPLAY_MARK_FRIDAY
 *      - #DISPLAY_MARK_SATURDAY
 *      - #DISPLAY_MARK_SUNDAY
 *      - #DISPLAY_MARK_ALARM
 */
void display_day(unsigned char d)
{
        if (d & DISPLAY_MARK_SUNDAY) {
                data[7] |= 1 << 2;
        } else {
                data[7] &= ~(1 << 2);
        }

        if (d & DISPLAY_MARK_SATURDAY) {
                data[6] |= 1 << 5;
        } else {
                data[6] &= ~(1 << 5);
        }

        if (d & DISPLAY_MARK_FRIDAY) {
                data[5] |= 1 << 0;
        } else {
                data[5] &= ~(1 << 0);
        }

        if (d & DISPLAY_MARK_THURSDAY) {
                data[4] |= 1 << 3;
        } else {
                data[4] &= ~(1 << 3);
        }

        if (d & DISPLAY_MARK_WEDNESDAY) {
                data[3] |= 1 << 6;
        } else {
                data[3] &= ~(1 << 6);
        }

        if (d & DISPLAY_MARK_TEUSDAY) {
                data[2] |= 1 << 1;
        } else {
                data[2] &= ~(1 << 1);
        }

        if (d & DISPLAY_MARK_MONDAY) {
                data[1] |= 1 << 4;
        } else {
                data[1] &= ~(1 << 4);
        }

        if (d & DISPLAY_MARK_ALARM) {
                data[0] |= 1 << 7;
        } else {
                data[0] &= ~(1 << 7);
        }
}

/**
 * Подготавливает изменения показа часов. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param h Новое значение часов в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_hours(unsigned char h)
{
        // Стирание минут в массиве.
        data[2] &= 1 << 0 | 1 << 1; // Бит вторника и разделителя не трогаем
        data[1] &= 1 << 4; // Бит четверга не трогаем
        data[0] &= 1 << 7; // Бит будильника не трогаем

        // Единицы.
        switch (h & 0x0f) {
        case 0:
                data[1] |= 1 << 0;
                break;
        case 1:
                data[2] |= 1 << 7;
                break;
        case 2:
                data[2] |= 1 << 6;
                break;
        case 3:
                data[2] |= 1 << 4;
                break;
        case 4:
                data[1] |= 1 << 2;
                break;
        case 5:
                data[1] |= 1 << 3;
                break;
        case 6:
                data[2] |= 1 << 2;
                break;
        case 7:
                data[2] |= 1 << 3;
                break;
        case 8:
                data[2] |= 1 << 5;
                break;
        case 9:
                data[1] |= 1 << 1;
                break;
        }

        // Десятки.
        switch (h & 0xf0) {
        case 0x00:
                data[0] |= 1 << 3;
                break;
        case 0x10:
                data[0] |= 1 << 2;
                break;
        case 0x20:
                data[0] |= 1 << 1;
                break;
        case 0x30:
                data[1] |= 1 << 7;
                break;
        case 0x40:
                data[0] |= 1 << 5;
                break;
        case 0x50:
                data[0] |= 1 << 6;
                break;
        case 0x60:
                data[1] |= 1 << 5;
                break;
        case 0x70:
                data[1] |= 1 << 6;
                break;
        case 0x80:
                data[0] |= 1 << 0;
                break;
        case 0x90:
                data[0] |= 1 << 4;
                break;
        }
}

/**
 * Подготавливает изменения показа минут. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param m Новое значение минут в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_minutes(unsigned char m)
{
        // Стирание минут в массиве.
        data[5] &= 1 << 0; // Бит пятницы не трогаем
        data[4] &= 1 << 3; // Бит четверга не трогаем
        data[3] &= 1 << 6 | 1 << 7; // Бит среды и разделителя не трогаем

        // Единицы.
        switch (m & 0x0f) {
        case 0:
                data[5] |= 1 << 7;
                break;
        case 1:
                data[5] |= 1 << 6;
                break;
        case 2:
                data[5] |= 1 << 5;
                break;
        case 3:
                data[5] |= 1 << 3;
                break;
        case 4:
                data[4] |= 1 << 1;
                break;
        case 5:
                data[4] |= 1 << 2;
                break;
        case 6:
                data[5] |= 1 << 1;
                break;
        case 7:
                data[5] |= 1 << 2;
                break;
        case 8:
                data[5] |= 1 << 4;
                break;
        case 9:
                data[4] |= 1 << 0;
                break;
        }

        // Десятки.
        switch (m & 0xf0) {
        case 0x00:
                data[3] |= 1 << 2;
                break;
        case 0x10:
                data[3] |= 1 << 1;
                break;
        case 0x20:
                data[3] |= 1 << 0;
                break;
        case 0x30:
                data[4] |= 1 << 6;
                break;
        case 0x40:
                data[3] |= 1 << 4;
                break;
        case 0x50:
                data[3] |= 1 << 5;
                break;
        case 0x60:
                data[4] |= 1 << 4;
                break;
        case 0x70:
                data[4] |= 1 << 5;
                break;
        case 0x80:
                data[4] |= 1 << 7;
                break;
        case 0x90:
                data[3] |= 1 << 3;
                break;
        }
}

/**
 * Подготавливает изменения показа секунд. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param s Новое значение секунд в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_seconds(unsigned char s)
{
        // Стирание секунд в массиве.
        data[8] = 0;
        data[7] &= 1 << 2; // Бит воскресенья не трогаем
        data[6] &= ~(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4);

        // Единицы.
        switch (s & 0x0f) {
        case 0:
                data[8] |= 1 << 6;
                break;
        case 1:
                data[8] |= 1 << 5;
                break;
        case 2:
                data[8] |= 1 << 4;
                break;
        case 3:
                data[8] |= 1 << 2;
                break;
        case 4:
                data[7] |= 1 << 0;
                break;
        case 5:
                data[7] |= 1 << 1;
                break;
        case 6:
                data[8] |= 1 << 0;
                break;
        case 7:
                data[8] |= 1 << 1;
                break;
        case 8:
                data[8] |= 1 << 3;
                break;
        case 9:
                data[8] |= 1 << 7;
                break;
        }

        // Десятки.
        switch (s & 0xf0) {
        case 0x00:
                data[6] |= 1 << 1;
                break;
        case 0x10:
                data[6] |= 1 << 0;
                break;
        case 0x20:
                data[7] |= 1 << 7;
                break;
        case 0x30:
                data[7] |= 1 << 5;
                break;
        case 0x40:
                data[6] |= 1 << 3;
                break;
        case 0x50:
                data[6] |= 1 << 4;
                break;
        case 0x60:
                data[7] |= 1 << 3;
                break;
        case 0x70:
                data[7] |= 1 << 4;
                break;
        case 0x80:
                data[7] |= 1 << 6;
                break;
        case 0x90:
                data[6] |= 1 << 2;
                break;
        }
}

/**
 * Подготавливает изменения показа разделителей. Изменения отправляются в
 *  дисплей функцией display_flush().
 * \param d Зажигаемые точки, может быть любой комбинацией флагов:
 *      - #DISPLAY_DOT_LEFT_TOP
 *      - #DISPLAY_DOT_LEFT_BOTTOM
 *      - #DISPLAY_DOT_RIGHT_TOP
 *      - #DISPLAY_DOT_RIGHT_BOTTOM
 */
void display_dots(unsigned char d)
{

        if (d & DISPLAY_DOT_RIGHT_BOTTOM) {
                data[6] |= 1 << 6;
        } else {
                data[6] &= ~(1 << 6);
        }

        if (d & DISPLAY_DOT_RIGHT_TOP) {
                data[6] |= 1 << 7;
        } else {
                data[6] &= ~(1 << 7);
        }

        if (d & DISPLAY_DOT_LEFT_BOTTOM) {
                data[3] |= 1 << 7;
        } else {
                data[3] &= ~(1 << 7);
        }

        if (d & DISPLAY_DOT_LEFT_TOP) {
                data[2] |= 1 << 0;
        } else {
                data[2] &= ~(1 << 0);
        }
}

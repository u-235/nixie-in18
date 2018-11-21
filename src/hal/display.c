#include "../bit-helper.h"
#include "../config.h"
#include "display.h"
#include "avr/spi.h"
#include "avr/pwmi.h"

static uint8_t data[SPI_ARRAY_SIZE];

#define _segment_on(seg)        _array_bit_set(seg)
#define _segment_off(seg)       _array_bit_clean(seg)
#define _array_bit_set(r, b)    _bit_set(data[r], b)
#define _array_bit_clean(r, b)  _bit_clear(data[r], b)

/**
 * Инициализация индикации.
 */
void display_init()
{
        display_bright(DISPLAY_BRIGHT_MAX);
        display_rate(DISPLAY_RATE_MAX);
}

/**
 * Очистка буфера дисплея. Последующий вызов функции display_flush() погасит
 * все цифры, разделители и значки.
 */
void display_clean()
{
        uint8_t *p = data;
        uint8_t i;

        for (i = 0; i < SPI_ARRAY_SIZE; i++) {
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
void display_day(uint8_t d)
{
        if (d & DISPLAY_MARK_SUNDAY) {
                _segment_on(CFG_REG_SUNDAY);
        } else {
                _segment_off(CFG_REG_SUNDAY);
        }

        if (d & DISPLAY_MARK_SATURDAY) {
                _segment_on(CFG_REG_SATURDAY);
        } else {
                _segment_off(CFG_REG_SATURDAY);
        }

        if (d & DISPLAY_MARK_FRIDAY) {
                _segment_on(CFG_REG_FRIDAY);
        } else {
                _segment_off(CFG_REG_FRIDAY);
        }

        if (d & DISPLAY_MARK_THURSDAY) {
                _segment_on(CFG_REG_THURSDAY);
        } else {
                _segment_off(CFG_REG_THURSDAY);
        }

        if (d & DISPLAY_MARK_WEDNESDAY) {
                _segment_on(CFG_REG_WEDNESDAY);
        } else {
                _segment_off(CFG_REG_WEDNESDAY);
        }

        if (d & DISPLAY_MARK_TEUSDAY) {
                _segment_on(CFG_REG_TEUSDAY);
        } else {
                _segment_off(CFG_REG_TEUSDAY);
        }

        if (d & DISPLAY_MARK_MONDAY) {
                _segment_on(CFG_REG_MONDAY);
        } else {
                _segment_off(CFG_REG_MONDAY);
        }

        if (d & DISPLAY_MARK_ALARM) {
                _segment_on(CFG_REG_ALARM);
        } else {
                _segment_off(CFG_REG_ALARM);
        }
}

/**
 * Подготавливает изменения показа часов. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param h Новое значение часов в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_hours(bcd2_t h)
{
        /* Предварительное стирание сегментов. */
        _segment_off(CFG_REG_x0xxxx);
        _segment_off(CFG_REG_x1xxxx);
        _segment_off(CFG_REG_x2xxxx);
        _segment_off(CFG_REG_x3xxxx);
        _segment_off(CFG_REG_x4xxxx);
        _segment_off(CFG_REG_x5xxxx);
        _segment_off(CFG_REG_x6xxxx);
        _segment_off(CFG_REG_x7xxxx);
        _segment_off(CFG_REG_x8xxxx);
        _segment_off(CFG_REG_x9xxxx);
        _segment_off(CFG_REG_0xxxxx);
        _segment_off(CFG_REG_1xxxxx);
        _segment_off(CFG_REG_2xxxxx);
        _segment_off(CFG_REG_3xxxxx);
        _segment_off(CFG_REG_4xxxxx);
        _segment_off(CFG_REG_5xxxxx);
        _segment_off(CFG_REG_6xxxxx);
        _segment_off(CFG_REG_7xxxxx);
        _segment_off(CFG_REG_8xxxxx);
        _segment_off(CFG_REG_9xxxxx);

        // Единицы.
        switch (h & 0x0f) {
        case 0:
                _segment_on(CFG_REG_x0xxxx);
                break;
        case 1:
                _segment_on(CFG_REG_x1xxxx);
                break;
        case 2:
                _segment_on(CFG_REG_x2xxxx);
                break;
        case 3:
                _segment_on(CFG_REG_x3xxxx);
                break;
        case 4:
                _segment_on(CFG_REG_x4xxxx);
                break;
        case 5:
                _segment_on(CFG_REG_x5xxxx);
                break;
        case 6:
                _segment_on(CFG_REG_x6xxxx);
                break;
        case 7:
                _segment_on(CFG_REG_x7xxxx);
                break;
        case 8:
                _segment_on(CFG_REG_x8xxxx);
                break;
        case 9:
                _segment_on(CFG_REG_x9xxxx);
                break;
        }

        // Десятки.
        switch (h & 0xf0) {
        case 0x00:
                _segment_on(CFG_REG_0xxxxx);
                break;
        case 0x10:
                _segment_on(CFG_REG_1xxxxx);
                break;
        case 0x20:
                _segment_on(CFG_REG_2xxxxx);
                break;
        case 0x30:
                _segment_on(CFG_REG_3xxxxx);
                break;
        case 0x40:
                _segment_on(CFG_REG_4xxxxx);
                break;
        case 0x50:
                _segment_on(CFG_REG_5xxxxx);
                break;
        case 0x60:
                _segment_on(CFG_REG_6xxxxx);
                break;
        case 0x70:
                _segment_on(CFG_REG_7xxxxx);
                break;
        case 0x80:
                _segment_on(CFG_REG_8xxxxx);
                break;
        case 0x90:
                _segment_on(CFG_REG_9xxxxx);
                break;
        }
}

/**
 * Подготавливает изменения показа минут. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param m Новое значение минут в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_minutes(bcd2_t m)
{
        /* Предварительное стирание сегментов. */
        _segment_off(CFG_REG_xxx0xx);
        _segment_off(CFG_REG_xxx1xx);
        _segment_off(CFG_REG_xxx2xx);
        _segment_off(CFG_REG_xxx3xx);
        _segment_off(CFG_REG_xxx4xx);
        _segment_off(CFG_REG_xxx5xx);
        _segment_off(CFG_REG_xxx6xx);
        _segment_off(CFG_REG_xxx7xx);
        _segment_off(CFG_REG_xxx8xx);
        _segment_off(CFG_REG_xxx9xx);
        _segment_off(CFG_REG_xx0xxx);
        _segment_off(CFG_REG_xx1xxx);
        _segment_off(CFG_REG_xx2xxx);
        _segment_off(CFG_REG_xx3xxx);
        _segment_off(CFG_REG_xx4xxx);
        _segment_off(CFG_REG_xx5xxx);
        _segment_off(CFG_REG_xx6xxx);
        _segment_off(CFG_REG_xx7xxx);
        _segment_off(CFG_REG_xx8xxx);
        _segment_off(CFG_REG_xx9xxx);

        // Единицы.
        switch (m & 0x0f) {
        case 0:
                _segment_on(CFG_REG_xxx0xx);
                break;
        case 1:
                _segment_on(CFG_REG_xxx1xx);
                break;
        case 2:
                _segment_on(CFG_REG_xxx2xx);
                break;
        case 3:
                _segment_on(CFG_REG_xxx3xx);
                break;
        case 4:
                _segment_on(CFG_REG_xxx4xx);
                break;
        case 5:
                _segment_on(CFG_REG_xxx5xx);
                break;
        case 6:
                _segment_on(CFG_REG_xxx6xx);
                break;
        case 7:
                _segment_on(CFG_REG_xxx7xx);
                break;
        case 8:
                _segment_on(CFG_REG_xxx8xx);
                break;
        case 9:
                _segment_on(CFG_REG_xxx9xx);
                break;
        }

        // Десятки.
        switch (m & 0xf0) {
        case 0x00:
                _segment_on(CFG_REG_xx0xxx);
                break;
        case 0x10:
                _segment_on(CFG_REG_xx1xxx);
                break;
        case 0x20:
                _segment_on(CFG_REG_xx2xxx);
                break;
        case 0x30:
                _segment_on(CFG_REG_xx3xxx);
                break;
        case 0x40:
                _segment_on(CFG_REG_xx4xxx);
                break;
        case 0x50:
                _segment_on(CFG_REG_xx5xxx);
                break;
        case 0x60:
                _segment_on(CFG_REG_xx6xxx);
                break;
        case 0x70:
                _segment_on(CFG_REG_xx7xxx);
                break;
        case 0x80:
                _segment_on(CFG_REG_xx8xxx);
                break;
        case 0x90:
                _segment_on(CFG_REG_xx9xxx);
                break;
        }
}

/**
 * Подготавливает изменения показа секунд. Изменения отправляются в дисплей
 *  функцией display_flush().
 * \param s Новое значение секунд в формате BCD. Старший нибл - десятки,
 *      младший - единицы. Для гашения разряда нибл должен быть больше 9.
 */
void display_seconds(bcd2_t s)
{
        /* Предварительное стирание сегментов. */
        _segment_off(CFG_REG_xxxxx0);
        _segment_off(CFG_REG_xxxxx1);
        _segment_off(CFG_REG_xxxxx2);
        _segment_off(CFG_REG_xxxxx3);
        _segment_off(CFG_REG_xxxxx4);
        _segment_off(CFG_REG_xxxxx5);
        _segment_off(CFG_REG_xxxxx6);
        _segment_off(CFG_REG_xxxxx7);
        _segment_off(CFG_REG_xxxxx8);
        _segment_off(CFG_REG_xxxxx9);
        _segment_off(CFG_REG_xxxx0x);
        _segment_off(CFG_REG_xxxx1x);
        _segment_off(CFG_REG_xxxx2x);
        _segment_off(CFG_REG_xxxx3x);
        _segment_off(CFG_REG_xxxx4x);
        _segment_off(CFG_REG_xxxx5x);
        _segment_off(CFG_REG_xxxx6x);
        _segment_off(CFG_REG_xxxx7x);
        _segment_off(CFG_REG_xxxx8x);
        _segment_off(CFG_REG_xxxx9x);

        // Единицы.
        switch (s & 0x0f) {
        case 0:
                _segment_on(CFG_REG_xxxxx0);
                break;
        case 1:
                _segment_on(CFG_REG_xxxxx1);
                break;
        case 2:
                _segment_on(CFG_REG_xxxxx2);
                break;
        case 3:
                _segment_on(CFG_REG_xxxxx3);
                break;
        case 4:
                _segment_on(CFG_REG_xxxxx4);
                break;
        case 5:
                _segment_on(CFG_REG_xxxxx5);
                break;
        case 6:
                _segment_on(CFG_REG_xxxxx6);
                break;
        case 7:
                _segment_on(CFG_REG_xxxxx7);
                break;
        case 8:
                _segment_on(CFG_REG_xxxxx8);
                break;
        case 9:
                _segment_on(CFG_REG_xxxxx9);
                break;
        }

        // Десятки.
        switch (s & 0xf0) {
        case 0x00:
                _segment_on(CFG_REG_xxxx0x);
                break;
        case 0x10:
                _segment_on(CFG_REG_xxxx1x);
                break;
        case 0x20:
                _segment_on(CFG_REG_xxxx2x);
                break;
        case 0x30:
                _segment_on(CFG_REG_xxxx3x);
                break;
        case 0x40:
                _segment_on(CFG_REG_xxxx4x);
                break;
        case 0x50:
                _segment_on(CFG_REG_xxxx5x);
                break;
        case 0x60:
                _segment_on(CFG_REG_xxxx6x);
                break;
        case 0x70:
                _segment_on(CFG_REG_xxxx7x);
                break;
        case 0x80:
                _segment_on(CFG_REG_xxxx8x);
                break;
        case 0x90:
                _segment_on(CFG_REG_xxxx9x);
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
void display_dots(uint8_t d)
{
        if (d & DISPLAY_DOT_RIGHT_BOTTOM) {
                _segment_on(CFG_REG_RIGHT_BOTTOM);
        } else {
                _segment_off(CFG_REG_RIGHT_BOTTOM);
        }

        if (d & DISPLAY_DOT_RIGHT_TOP) {
                _segment_on(CFG_REG_RIGHT_TOP);
        } else {
                _segment_off(CFG_REG_RIGHT_TOP);
        }

        if (d & DISPLAY_DOT_LEFT_BOTTOM) {
                _segment_on(CFG_REG_LEFT_BOTTOM);
        } else {
                _segment_off(CFG_REG_LEFT_BOTTOM);
        }

        if (d & DISPLAY_DOT_LEFT_TOP) {
                _segment_on(CFG_REG_LEFT_TOP);
        } else {
                _segment_off(CFG_REG_LEFT_TOP);
        }
}

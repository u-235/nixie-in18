/**
 * @file
 * @brief
 * @details
 *
 * @date создан 02.01.2015
 * @author Nick Egorrov
 */

#include <string.h>
#include "../bit-helper.h"
#include "../config.h"
#include "display.h"
#include "avr/spi.h"
#include "avr/pwmi.h"

static uint8_t dev_data[SPI_ARRAY_SIZE];
static struct display logical;

#define _segment_on(seg)        _array_bit_set(seg)
#define _array_bit_set(r, b)    _bit_set(dev_data[r], b)


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
        memset( &logical, 0, sizeof(struct display));
}

/**
 * Отправка накопленных изменений в дисплей.
 */
void display_flush()
{
        uint8_t units, tens;

        memset( dev_data, 0, SPI_ARRAY_SIZE);

        units = logical.seconds % 10;
        tens = logical.seconds / 10;
        /* Единицы секунд. */
        if (logical.enabled & DISPLAY_ENABLED_SECONDS_UNITS) {
                switch (units) {
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
        }
        /* Десятки секунд. */
        if (logical.enabled & DISPLAY_ENABLED_SECONDS_TENS) {
                switch (tens) {
                case 0:
                        _segment_on(CFG_REG_xxxx0x);
                        break;
                case 1:
                        _segment_on(CFG_REG_xxxx1x);
                        break;
                case 2:
                        _segment_on(CFG_REG_xxxx2x);
                        break;
                case 3:
                        _segment_on(CFG_REG_xxxx3x);
                        break;
                case 4:
                        _segment_on(CFG_REG_xxxx4x);
                        break;
                case 5:
                        _segment_on(CFG_REG_xxxx5x);
                        break;
                case 6:
                        _segment_on(CFG_REG_xxxx6x);
                        break;
                case 7:
                        _segment_on(CFG_REG_xxxx7x);
                        break;
                case 8:
                        _segment_on(CFG_REG_xxxx8x);
                        break;
                case 9:
                        _segment_on(CFG_REG_xxxx9x);
                        break;
                }
        }

        units = logical.minutes % 10;
        tens = logical.minutes / 10;
        /* Единицы минут. */
        if (logical.enabled & DISPLAY_ENABLED_MINUTES_UNITS) {
                switch (units) {
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
        }
        /* Десятки минут. */
        if (logical.enabled & DISPLAY_ENABLED_MINUTES_TENS) {
                switch (tens) {
                case 0:
                        _segment_on(CFG_REG_xx0xxx);
                        break;
                case 1:
                        _segment_on(CFG_REG_xx1xxx);
                        break;
                case 2:
                        _segment_on(CFG_REG_xx2xxx);
                        break;
                case 3:
                        _segment_on(CFG_REG_xx3xxx);
                        break;
                case 4:
                        _segment_on(CFG_REG_xx4xxx);
                        break;
                case 5:
                        _segment_on(CFG_REG_xx5xxx);
                        break;
                case 6:
                        _segment_on(CFG_REG_xx6xxx);
                        break;
                case 7:
                        _segment_on(CFG_REG_xx7xxx);
                        break;
                case 8:
                        _segment_on(CFG_REG_xx8xxx);
                        break;
                case 9:
                        _segment_on(CFG_REG_xx9xxx);
                        break;
                }
        }

        units = logical.hours % 10;
        tens = logical.hours / 10;
        /* Единицы часов. */
        if (logical.enabled & DISPLAY_ENABLED_HOURS_UNITS) {
                switch (units) {
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
        }
        /* Десятки часов. */
        if (logical.enabled & DISPLAY_ENABLED_HOURS_TENS) {
                switch (tens) {
                case 0:
                        _segment_on(CFG_REG_0xxxxx);
                        break;
                case 1:
                        _segment_on(CFG_REG_1xxxxx);
                        break;
                case 2:
                        _segment_on(CFG_REG_2xxxxx);
                        break;
                case 3:
                        _segment_on(CFG_REG_3xxxxx);
                        break;
                case 4:
                        _segment_on(CFG_REG_4xxxxx);
                        break;
                case 5:
                        _segment_on(CFG_REG_5xxxxx);
                        break;
                case 6:
                        _segment_on(CFG_REG_6xxxxx);
                        break;
                case 7:
                        _segment_on(CFG_REG_7xxxxx);
                        break;
                case 8:
                        _segment_on(CFG_REG_8xxxxx);
                        break;
                case 9:
                        _segment_on(CFG_REG_9xxxxx);
                        break;
                }
        }

        /* Загрузка флажков. */
        if (logical.marks & DISPLAY_MARK_SUNDAY) {
                _segment_on(CFG_REG_SUNDAY);
        }
        if (logical.marks & DISPLAY_MARK_SATURDAY) {
                _segment_on(CFG_REG_SATURDAY);
        }
        if (logical.marks & DISPLAY_MARK_FRIDAY) {
                _segment_on(CFG_REG_FRIDAY);
        }
        if (logical.marks & DISPLAY_MARK_THURSDAY) {
                _segment_on(CFG_REG_THURSDAY);
        }
        if (logical.marks & DISPLAY_MARK_WEDNESDAY) {
                _segment_on(CFG_REG_WEDNESDAY);
        }
        if (logical.marks & DISPLAY_MARK_TEUSDAY) {
                _segment_on(CFG_REG_TEUSDAY);
        }
        if (logical.marks & DISPLAY_MARK_MONDAY) {
                _segment_on(CFG_REG_MONDAY);
        }
        if (logical.marks & DISPLAY_MARK_ALARM) {
                _segment_on(CFG_REG_ALARM);
        }

        /* Загрузка точек - разделителей. */
        if (logical.dots & DISPLAY_DOT_RIGHT_BOTTOM) {
                _segment_on(CFG_REG_RIGHT_BOTTOM);
        }
        if (logical.dots & DISPLAY_DOT_RIGHT_TOP) {
                _segment_on(CFG_REG_RIGHT_TOP);
        }
        if (logical.dots & DISPLAY_DOT_LEFT_BOTTOM) {
                _segment_on(CFG_REG_LEFT_BOTTOM);
        }
        if (logical.dots & DISPLAY_DOT_LEFT_TOP) {
                _segment_on(CFG_REG_LEFT_TOP);
        }

        pwmi_load(dev_data);
}

/**
 * Получение управляющей структуры.
 */
struct display *display_get(){
        return &logical;
}

uint8_t display_make_mark(uint8_t week_day, uint8_t alarm)
{
        uint8_t retval = 0;

        if (alarm != 0) {
                retval |= DISPLAY_MARK_ALARM;
        }

        switch (week_day) {
        case MONDAY:
                retval |= DISPLAY_MARK_MONDAY;
                break;
        case TUESDAY:
                retval |= DISPLAY_MARK_TEUSDAY;
                break;
        case WEDNESDAY:
                retval |= DISPLAY_MARK_WEDNESDAY;
                break;
        case THURSDAY:
                retval |= DISPLAY_MARK_THURSDAY;
                break;
        case FRIDAY:
                retval |= DISPLAY_MARK_FRIDAY;
                break;
        case SATURDAY:
                retval |= DISPLAY_MARK_SATURDAY;
                break;
        case SUNDAY:
                retval |= DISPLAY_MARK_SUNDAY;
                break;
        }
        return retval;
}

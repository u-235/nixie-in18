/**
 * @file
 * @brief
 * @details
 *
 * @date создан 02.01.2015
 * @author Nick Egorrov
 */

#ifndef HAL_DISPLAY_H_
#define HAL_DISPLAY_H_

#include <stdint.h>
#include "../time/time.h"

/** Левый верхний разделитель. */
#define DISPLAY_DOT_LEFT_TOP            0x01
/** Левый нижний разделитель. */
#define DISPLAY_DOT_LEFT_BOTTOM         0x02
/** Правый верхний разделитель. */
#define DISPLAY_DOT_RIGHT_TOP           0x04
/** Правый нижний разделитель. */
#define DISPLAY_DOT_RIGHT_BOTTOM        0x08
/** Разделители погашены. */
#define DISPLAY_DOT_HIDE                0x00
/** Правые разделители. */
#define DISPLAY_DOT_LEFT        (DISPLAY_DOT_LEFT_TOP | DISPLAY_DOT_LEFT_BOTTOM)
/** Левые разделители. */
#define DISPLAY_DOT_RIGHT       (DISPLAY_DOT_RIGHT_TOP | DISPLAY_DOT_RIGHT_BOTTOM)
/** Верхние разделители. */
#define DISPLAY_DOT_TOP         (DISPLAY_DOT_LEFT_TOP | DISPLAY_DOT_RIGHT_TOP)
/** Нижние разделители. */
#define DISPLAY_DOT_BOTTOM      (DISPLAY_DOT_LEFT_BOTTOM | DISPLAY_DOT_RIGHT_BOTTOM)
/** Все разделители. */
#define DISPLAY_DOT_ALL         (DISPLAY_DOT_LEFT | DISPLAY_DOT_RIGHT)

/** Значок понедельника. */
#define DISPLAY_MARK_MONDAY     0x02
/** Значок вторника. */
#define DISPLAY_MARK_TEUSDAY    0x04
/** Значок среды. */
#define DISPLAY_MARK_WEDNESDAY  0x08
/** Значок четверга.  */
#define DISPLAY_MARK_THURSDAY   0x10
/** Значок пятницы. */
#define DISPLAY_MARK_FRIDAY     0x20
/** Значок субботы.  */
#define DISPLAY_MARK_SATURDAY   0x40
/** Значок воскресенья. */
#define DISPLAY_MARK_SUNDAY     0x80
/** Значок будильника. */
#define DISPLAY_MARK_ALARM      0x01

#define DISPLAY_ENABLED_SECONDS_UNITS   (1 << 0)
#define DISPLAY_ENABLED_SECONDS_TENS    (1 << 1)
#define DISPLAY_ENABLED_SECONDS \
                (DISPLAY_ENABLED_SECONDS_UNITS | DISPLAY_ENABLED_SECONDS_TENS)
#define DISPLAY_ENABLED_MINUTES_UNITS   (1 << 2)
#define DISPLAY_ENABLED_MINUTES_TENS    (1 << 3)
#define DISPLAY_ENABLED_MINUTES \
                (DISPLAY_ENABLED_MINUTES_UNITS | DISPLAY_ENABLED_MINUTES_TENS)
#define DISPLAY_ENABLED_HOURS_UNITS     (1 << 4)
#define DISPLAY_ENABLED_HOURS_TENS      (1 << 5)
#define DISPLAY_ENABLED_HOURS \
                (DISPLAY_ENABLED_HOURS_UNITS | DISPLAY_ENABLED_HOURS_TENS)
#define DISPLAY_ENABLED_TIME \
                (DISPLAY_ENABLED_MINUTES | DISPLAY_ENABLED_HOURS)
#define DISPLAY_ENABLED_ALL \
                (DISPLAY_ENABLED_SECONDS | DISPLAY_ENABLED_TIME)

typedef struct {
        union {
                uint8_t hours;
                uint8_t day;
        };
        union {
                uint8_t minutes;
                uint8_t month;
        };
        union {
                uint8_t seconds;
                uint8_t year;
        };
        uint8_t enabled;
        uint8_t marks;
        uint8_t dots;
} display_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void display_init();
extern void display_clean();
extern void display_flush();
extern display_t *display_get();
extern uint8_t display_make_mark(uint8_t week_day, uint8_t alarm);

#define DISPLAY_BRIGHT_MAX 16
#define DISPLAY_BRIGHT_MIN 7

#define DISPLAY_RATE_MAX 7
#define DISPLAY_RATE_MIN 0

extern void display_bright(uint8_t lvl);
extern void display_rate(uint8_t lvl);

#ifdef __cplusplus
}
#endif

#endif // HAL_DISPLAY_H_

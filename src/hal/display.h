#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

//! Левый верхний разделитель.
#define DISPLAY_DOT_LEFT_TOP            0x01
//! Левый нижний разделитель.
#define DISPLAY_DOT_LEFT_BOTTOM         0x02
//! Правый верхний разделитель.
#define DISPLAY_DOT_RIGHT_TOP           0x04
//! Правый нижний разделитель.
#define DISPLAY_DOT_RIGHT_BOTTOM        0x08

//! Значок понедельника.
#define DISPLAY_MARK_MONDAY     0x02
//! Значок вторника.
#define DISPLAY_MARK_TEUSDAY    0x04
//! Значок среды.
#define DISPLAY_MARK_WEDNESDAY  0x08
//! Значок четверга.
#define DISPLAY_MARK_THURSDAY   0x10
//! Значок пятницы.
#define DISPLAY_MARK_FRIDAY     0x20
//! Значок субботы.
#define DISPLAY_MARK_SATURDAY   0x40
//! Значок воскресенья.
#define DISPLAY_MARK_SUNDAY     0x80
//! Значок будильника.
#define DISPLAY_MARK_ALARM      0x01

#ifdef __cplusplus
extern "C" {
#endif

        void dislpay_init();
        void display_clean();
        void display_flush();

        void display_dots(uint8_t d);
        void display_day(uint8_t d);
        void display_hours(uint8_t h);
        void display_minutes(uint8_t m);
        void display_seconds(uint8_t s);

#define DISPLAY_BRIGHT_MAX 16
#define DISPLAY_BRIGHT_MIN 7

#define DISPLAY_RATE_MAX 7
#define DISPLAY_RATE_MIN 0

        void display_bright(uint8_t lvl);
        void display_rate(uint8_t lvl);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H_

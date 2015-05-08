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
#define DISPLAY_MARK_MONDAY     0x01
//! Значок вторника.
#define DISPLAY_MARK_TEUSDAY    0x02
//! Значок среды.
#define DISPLAY_MARK_WEDNESDAY  0x04
//! Значок четверга.
#define DISPLAY_MARK_THURSDAY   0x08
//! Значок пятницы.
#define DISPLAY_MARK_FRIDAY     0x10
//! Значок субботы.
#define DISPLAY_MARK_SATURDAY   0x20
//! Значок воскресенья.
#define DISPLAY_MARK_SUNDAY     0x40
//! Значок будильника.
#define DISPLAY_MARK_ALARM      0x80

void dislpay_init();
void display_clean();
void display_flush();

void display_dots(uint8_t d);
void display_day(uint8_t d);
void display_hours(uint8_t h);
void display_minutes(uint8_t m);
void display_seconds(uint8_t s);

#define DISPLAY_BRIGHT_MAX 15
/* Минимальный уровень яркости для степенной зависимости равен 7,
 *  для линейной зависимости 2.*/
#ifdef BRIGHT_SCHEME_POW
#       define DISPLAY_BRIGHT_MIN 7
#else
#       define DISPLAY_BRIGHT_MIN 2
#endif

#define DISPLAY_RATE_MAX 7
#define DISPLAY_RATE_MIN 0

void display_bright(uint8_t lvl);
void display_rate(uint8_t lvl);

#endif // DISPLAY_H_

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/// Левый верхний разделитель.
#define DISPLAY_DOT_LEFT_TOP            0x01
/// Левый нижний разделитель.
#define DISPLAY_DOT_LEFT_BOTTOM         0x02
/// Правый верхний разделитель.
#define DISPLAY_DOT_RIGHT_TOP           0x04
/// Правый нижний разделитель.
#define DISPLAY_DOT_RIGHT_BOTTOM        0x08

/// Значок понедельника.
#define DISPLAY_MARK_MONDAY     0x01
/// Значок вторника.
#define DISPLAY_MARK_TEUSDAY    0x02
/// Значок среды.
#define DISPLAY_MARK_WEDNESDAY  0x04
/// Значок четверга.
#define DISPLAY_MARK_THURSDAY   0x08
/// Значок пятницы.
#define DISPLAY_MARK_FRIDAY     0x10
/// Значок субботы.
#define DISPLAY_MARK_SATURDAY   0x20
/// Значок воскресенья.
#define DISPLAY_MARK_SUNDAY     0x40
/// Значок будильника.
#define DISPLAY_MARK_ALARM      0x80

void dislpay_init();
void display_clean();
void display_flush();

void display_dots(unsigned char d);
void display_day(unsigned char d);
void display_hours(unsigned char h);
void display_minutes(unsigned char m);
void display_seconds(unsigned char s);

#endif // _DISPLAY_H_

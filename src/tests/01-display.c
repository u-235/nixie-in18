#include <avr/io.h>
#include <util/delay.h>
#include "../hal/display.h"
#include "../hal/avr/asm.h"

#define STEP_DELAY 1000

static void flush();

int main(void)
{
        dislpay_init();
        display_rate(2);
        _sei;

        while (1) {
                display_clean();
                flush();

                unsigned char c = 1;
                // Показ дней недели и будильника начиная с понедельника.
                for (char i = 0; i < 8; i++) {
                        display_day(c);
                        flush();
                        c <<= 1;
                        _delay_ms(STEP_DELAY);
                }
                // Гасим дни.
                display_day(0);

                // Показ разделителей.
                display_dots(DISPLAY_DOT_LEFT_BOTTOM);
                flush();
                _delay_ms(STEP_DELAY);

                display_dots(DISPLAY_DOT_LEFT_TOP);
                flush();
                _delay_ms(STEP_DELAY);

                display_dots(DISPLAY_DOT_RIGHT_TOP);
                flush();
                _delay_ms(STEP_DELAY);

                display_dots(DISPLAY_DOT_RIGHT_BOTTOM);
                flush();
                _delay_ms(STEP_DELAY);
                // Гасим разделители.
                display_dots(0);

                // Показ единиц секунд
                for (char i = 0; i <= 9; i++) {
                        display_seconds(i + 0xa0);
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков секунд
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_seconds(i + 0x0a);
                        flush();
                        _delay_ms(STEP_DELAY);
                }
                display_seconds(0xff);

                // Показ единиц минут
                for (char i = 0; i <= 9; i++) {
                        display_minutes(i + 0xa0);
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков минут
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_minutes(i + 0x0a);
                        flush();
                        _delay_ms(STEP_DELAY);
                }
                display_minutes(0xff);

                // Показ единиц часов
                for (char i = 0; i <= 9; i++) {
                        display_hours(i + 0xa0);
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков часов
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_hours(i + 0x0a);
                        flush();
                        _delay_ms(STEP_DELAY);
                }
                display_hours(0xff);
        }

        return 0;
}

void flush()
{
        static unsigned char count = 0;
        static unsigned char dir = 0;
        static unsigned char lvl = DISPLAY_BRIGHT_MAX;

        count++;
        if (count > 8) {
                count = 0;
                if (dir == 0) {
                        lvl--;
                        if (lvl == DISPLAY_BRIGHT_MIN) {
                                dir = 1;
                        }
                } else {
                        lvl++;
                        if (lvl == DISPLAY_BRIGHT_MAX) {
                                dir = 0;
                        }
                }
        }
        display_bright(lvl); //DISPLAY_BRIGHT_MIN
        display_flush();
}

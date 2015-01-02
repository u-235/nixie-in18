#include <avr/io.h>
#include <util/delay.h>
#include "display.h"

#define STAP_DELAY 600

int main(void)
{
        dislpay_init();

        while (1) {
                display_clean();
                display_flush();

                unsigned char c = 1;
                // Показ дней недели и будильника начиная с понедельника.
                for (char i = 0; i < 8; i++) {
                        display_day(c);
                        display_flush();
                        c <<= 1;
                        _delay_ms(STAP_DELAY);
                }
                // Гасим дни.
                display_day(0);

                // Показ разделителей.
                display_dots(DISPLAY_DOT_LEFT_BOTTOM);
                display_flush();
                _delay_ms(STAP_DELAY);

                display_dots(DISPLAY_DOT_LEFT_TOP);
                display_flush();
                _delay_ms(STAP_DELAY);

                display_dots(DISPLAY_DOT_RIGHT_TOP);
                display_flush();
                _delay_ms(STAP_DELAY);

                display_dots(DISPLAY_DOT_RIGHT_BOTTOM);
                display_flush();
                _delay_ms(STAP_DELAY);
                // Гасим разделители.
                display_dots(0);

                // Показ единиц секунд
                for (char i = 0; i <= 9; i++) {
                        display_seconds(i + 0xa0);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }

                // Показ десятков секунд
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_seconds(i + 0x0a);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }
                display_seconds( 0xff);

                // Показ единиц минут
                for (char i = 0; i <= 9; i++) {
                        display_minutes(i + 0xa0);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }

                // Показ десятков минут
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_minutes(i + 0x0a);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }
                display_minutes(0xff);

                // Показ единиц часов
                for (char i = 0; i <= 9; i++) {
                        display_hours(i + 0xa0);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }

                // Показ десятков часов
                for (char i = 0; i <= 0x90; i += 0x10) {
                        display_hours(i + 0x0a);
                        display_flush();
                        _delay_ms(STAP_DELAY);
                }
                display_hours(0xff);
        }

        return 0;
}

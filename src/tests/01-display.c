#include <avr/io.h>
#include <util/delay.h>
#include "../hal/avr/asm.h"
#include "../hal/display.h"
#include "../hal/mcu.h"

#define STEP_DELAY 1000

static void flush();

int main(void)
{
        display_t *pdisp = display_get();
        mcu_init();
        display_init();
        display_rate(2);
        mcu_interrupt_enable();

        while (1) {
                display_clean();
                flush();

                unsigned char c = 1;
                // Показ дней недели и будильника начиная с понедельника.
                for (unsigned char i = 0; i < 8; i++) {
                        pdisp->marks = c;
                        flush();
                        c <<= 1;
                        _delay_ms(STEP_DELAY);
                }
                // Гасим дни.
                pdisp->marks = 0;

                // Показ разделителей.
                pdisp->dots = DISPLAY_DOT_LEFT_BOTTOM;
                flush();
                _delay_ms(STEP_DELAY);

                pdisp->dots = DISPLAY_DOT_LEFT_TOP;
                flush();
                _delay_ms(STEP_DELAY);

                pdisp->dots = DISPLAY_DOT_RIGHT_TOP;
                flush();
                _delay_ms(STEP_DELAY);

                pdisp->dots = DISPLAY_DOT_RIGHT_BOTTOM;
                flush();
                _delay_ms(STEP_DELAY);
                // Гасим разделители.
                pdisp->dots = (0);

                // Показ единиц секунд
                pdisp->enabled = DISPLAY_ENABLED_SECONDS_UNITS;
                for (unsigned char i = 0; i <= 9; i++) {
                        pdisp->seconds = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков секунд
                pdisp->enabled = DISPLAY_ENABLED_SECONDS_TENS;
                for (unsigned char i = 0; i <= 90; i += 10) {
                        pdisp->seconds = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ единиц минут
                pdisp->enabled = DISPLAY_ENABLED_MINUTES_UNITS;
                for (unsigned char i = 0; i <= 9; i++) {
                        pdisp->minutes = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков минут
                pdisp->enabled = DISPLAY_ENABLED_MINUTES_TENS;
                for (unsigned char i = 0; i <= 90; i += 10) {
                        pdisp->minutes = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ единиц часов
                pdisp->enabled = DISPLAY_ENABLED_HOURS_UNITS;
                for (unsigned char i = 0; i <= 9; i++) {
                        pdisp->hours = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }

                // Показ десятков часов
                pdisp->enabled = DISPLAY_ENABLED_HOURS_TENS;
                for (unsigned char i = 0; i <= 0x90; i += 0x10) {
                        pdisp->hours = i;
                        flush();
                        _delay_ms(STEP_DELAY);
                }
                pdisp->enabled = 0;
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
        display_bright(lvl);  //DISPLAY_BRIGHT_MIN
        display_flush();
}

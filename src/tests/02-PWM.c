#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../config.h"
#include "../hal/adc.h"
#include "../hal/avr/asm.h"
#include "../hal/display.h"
#include "../hal/mcu.h"

static uint8_t get_rate();
static uint8_t get_bright();

int main(void)
{
        display_t *pdisp = display_get();
        uint8_t bright, rate;
        uint8_t count = 0;
        uint8_t day = 0, day_state = 0;
        uint8_t dots = 0;

        mcu_init();
        display_init();
        mcu_interrupt_enable();
        display_clean();
        pdisp->enabled = DISPLAY_ENABLED_ALL;

        while (1) {
                bright = get_bright();
                rate = get_rate();

                display_bright(bright);
                display_rate(rate);

                pdisp->hours = bright;
                pdisp->minutes = rate;

                pdisp->seconds = count;
                count++;
                if (count >= 100) {
                        count = 0;
                }

                switch (day_state) {
                case 0:
                        if (day == 0) {
                                day = DISPLAY_MARK_ALARM;
                        } else if ((day & DISPLAY_MARK_SUNDAY) != 0) {
                                day = 0;
                                day_state++;
                        } else {
                                day <<= 1;
                        }
                        break;
                case 1:
                        if (day == 0) {
                                day = DISPLAY_MARK_SUNDAY;
                        } else if ((day & DISPLAY_MARK_ALARM) != 0) {
                                day = 0;
                                day_state++;
                        } else {
                                day >>= 1;
                        }
                        break;
                case 2:
                        if (day == 0) {
                                day = DISPLAY_MARK_ALARM;
                        } else if ((day & DISPLAY_MARK_SUNDAY) != 0) {
                                day = 0;
                                day_state++;
                        } else {
                                day = day << 1 | DISPLAY_MARK_ALARM;
                        }
                        break;
                default:
                        if (day == 0) {
                                day = DISPLAY_MARK_SUNDAY;
                        } else if ((day & DISPLAY_MARK_ALARM) != 0) {
                                day = 0;
                                day_state = 0;
                        } else {
                                day = day >> 1 | DISPLAY_MARK_SUNDAY;
                        }
                        break;
                }
                pdisp->marks = day;

                if ((dots & DISPLAY_DOT_LEFT_BOTTOM) != 0) {
                        dots = DISPLAY_DOT_LEFT_TOP | DISPLAY_DOT_RIGHT_BOTTOM;
                } else {
                        dots = DISPLAY_DOT_LEFT_BOTTOM | DISPLAY_DOT_RIGHT_TOP;
                }
                pdisp->dots = dots;

                display_flush();
                _delay_ms(1000);
        }

        return (0);
}

/*
 * Получение длительности смены показаний.
 */
uint8_t get_rate()
{
        uint16_t ret;
        /*
         * Запуск измерения и ожидание результата.
         */
        adc_set_channel(CFG_ADC_CHANNEL_RATE);
        _delay_ms(2);
        adc_start(0);
        ret = adc_get_result();
        /*
         * К результату измерения прибавляем половину интервала
         * АЦП, приходящегося на один шаг скорости смены показаний.
         * Это нужно для гарантированного получения значения
         * DISPLAY_RATE_MAX.
         */
        ret += 128 / (DISPLAY_RATE_MAX - DISPLAY_RATE_MIN + 1);
        ret = ret * (DISPLAY_RATE_MAX - DISPLAY_RATE_MIN + 1) / 256;
        ret += DISPLAY_RATE_MIN;
        return ret;
}

/*
 * Калька с функции get_rate().
 */
uint8_t get_bright()
{
        uint16_t ret;

        adc_set_channel(CFG_ADC_CHANNEL_BRIGHT);
        _delay_ms(2);
        adc_start(0);
        ret = adc_get_result();
        ret += 128 / (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN + 1);
        ret = ret * (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN + 1) / 256;
        ret += DISPLAY_BRIGHT_MIN;
        return ret;
}

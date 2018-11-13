#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "../hal/display.h"
#include "../bcd/bcd.h"
#include "../hal/avr/asm.h"

/* Разрешаем АЦП и устанавливаем минимальную частоту преобразования */
#define ADC_MODE        ((1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define ADC_START       (1 << ADSC)
#define ADC_FINISH      (1 << ADIF)

/* Выравнивание результата преобразования влево для отбрасывания
 *  младших двух бит. Результат брать в ADCH. */
#define ADC_CANNEL_BRIGHT       ((1 << ADLAR) | (1 << MUX0))
#define ADC_CANNEL_RATE         ((1 << ADLAR) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))

static void adc_init(void);
static uint8_t get_rate();
static uint8_t get_bright();

int main(void)
{
        uint8_t bright, rate;
        bcd2_t count = 0;
        uint8_t day = 0, day_state = 0;
        uint8_t dots = 0;

        display_init();
        adc_init();
        mcu_interrupt_enable();

        while (1) {
                bright = get_bright();
                rate = get_rate();

                display_bright(bright);
                display_rate(rate);

                display_hours(bcd_from_uint8(bright));
                display_minutes(bcd_from_uint8(rate));

                display_seconds(count);
                count = bcd_add2(count, 1);

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
                display_day(day);

                if ((dots & DISPLAY_DOT_LEFT_BOTTOM) != 0) {
                        dots = DISPLAY_DOT_LEFT_TOP | DISPLAY_DOT_RIGHT_BOTTOM;
                } else {
                        dots = DISPLAY_DOT_LEFT_BOTTOM | DISPLAY_DOT_RIGHT_TOP;
                }
                display_dots(dots);

                display_flush();
                _delay_ms(1000);
        }

        return (0);
}

/*
 * Инициализация АЦП.
 */
static void adc_init(void)
{

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
        ADMUX = ADC_CANNEL_RATE;
        _delay_ms(2);
        ADCSRA = ADC_MODE | ADC_START;
        while (!(ADCSRA & ADC_FINISH)) {
                // nop
        }
        /*
         * К результату измерения прибавляем половину интервала
         * АЦП, приходящегося на один шаг скорости смены показаний.
         * Это нужно для гарантированного получения значения
         * DISPLAY_RATE_MAX.
         */
        ret = ADCH + 128 / (DISPLAY_RATE_MAX - DISPLAY_RATE_MIN + 1);
        ret = ret * (DISPLAY_RATE_MAX - DISPLAY_RATE_MIN) / 255;
        ret += DISPLAY_RATE_MIN;
        /*
         * Сброс флага прерывания АЦП.
         */
        ADCSRA = ADC_MODE | ADC_FINISH;
        return ret;
}

uint8_t get_bright()
{
        /*
         * Калька с функции get_rate().
         */
        uint16_t ret;
        ADMUX = ADC_CANNEL_BRIGHT;
        _delay_ms(2);
        ADCSRA = ADC_MODE | ADC_START;
        while (!(ADCSRA & ADC_FINISH)) {
                // nop
        }
        ret = ADCH + 128 / (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN + 1);
        ret = ret * (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN) / 255;
        ret += DISPLAY_BRIGHT_MIN;
        ADCSRA = ADC_MODE | ADC_FINISH;
        return ret;
}

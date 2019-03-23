/**
 * @file
 * @brief
 * @details
 *
 * @date создан 10.03.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../bit-helper.h"
#include "../adc.h"

/* Разрешаем АЦП и устанавливаем минимальную частоту преобразования */
#define ADC_ON          ((1 << ADEN) | (1 << ADIE))
#define ADC_MODE        ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define ADC_START       (1 << ADSC)

static uint8_t channel;

static void (*handler)(uint8_t ch, uint8_t res);

void adc_init()
{
        ADCSRA = ADC_ON;
}

void adc_set_channel(uint8_t ch)
{
        channel = ch;
        ADMUX = (1 << ADLAR) | ch;
}

uint8_t adc_get_channel()
{
        return channel;
}

uint8_t adc_is_busy()
{
        return _is_bit_set(ADCSRA, ADSC);
}

void adc_start(void (*on_complete)(uint8_t ch, uint8_t res))
{
        while (_is_bit_set(ADCSRA, ADSC)) {
        }
        handler = on_complete;
        ADCSRA = ADC_ON | ADC_MODE | ADC_START;
}

uint8_t adc_get_result()
{
        while (_is_bit_set(ADCSRA, ADSC)) {
        }
        return ADCH;
}

ISR(ADC_vect)
{
        if (handler)
                handler(channel, ADCH);
}


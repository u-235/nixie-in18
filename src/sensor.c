/**
 * @file
 * @brief
 * @details
 *
 * @date создан 11.03.2019
 * @author Nick Egorrov
 */

#include <avr/pgmspace.h>
#include "config.h"
#include "sensor.h"
#include "hal/adc.h"
#include "tms/tms.h"

static void timer_callback(void);
static void adc_callback(uint8_t ch, uint8_t res);
static void to_bright(uint8_t conv);
static void to_rate(uint8_t conv);

static timer_id_t timer_update;
static uint8_t raw;
static uint8_t bright;
static uint8_t rate;

void sensor_init()
{
        timer_update = tms_create_timer(&timer_callback);
        tms_set_timer(timer_update, _ticks_from_ms(CFG_ADC_CONVERTING_PERIOD));
        tms_start_timer(timer_update);
        adc_start(&adc_callback);
}

uint8_t sensor_get_bright()
{
        return bright;
}

uint8_t sensor_get_rate()
{
        return rate;
}

void timer_callback()
{
        if (adc_get_channel() != CFG_ADC_CHANNEL_BRIGHT) {
                to_bright(raw);
        } else {
                to_rate(raw);
        }
        adc_start(&adc_callback);
}

void adc_callback(uint8_t ch, uint8_t res)
{
        uint8_t next_channel;

        raw = res;

        if (ch == CFG_ADC_CHANNEL_BRIGHT) {
                next_channel = CFG_ADC_CHANNEL_RATE;
        } else {
                next_channel = CFG_ADC_CHANNEL_BRIGHT;
        }

        adc_set_channel(next_channel);
}

/* При изменении макросов #DISPLAY_BRIGHT_MIN и #DISPLAY_BRIGHT_MAX нужно
 * менять и таблицу для преобразования показаний датчика освещённости в
 * уровень яркости. */
#define BT_SIZE 10
#if BT_SIZE != (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN + 1)
# error bad size of bright_table
#endif

void to_bright(uint8_t conv)
{
        uint8_t i;
        static const PROGMEM uint8_t bright_table[BT_SIZE] = {
                        12, 37, 62, 87, 112, 137, 162, 187, 212, 237
        };

        for (i = 0; i < BT_SIZE; i++) {
                if (pgm_read_byte(&bright_table[i]) >= conv)
                        break;
        }
        bright = i + DISPLAY_BRIGHT_MIN;
}

#define RATE_STEP (256 / (DISPLAY_RATE_MAX - DISPLAY_RATE_MIN + 1))

void to_rate(uint8_t conv)
{
        rate = (uint8_t) (conv / 2 + RATE_STEP / 2) / RATE_STEP * 2
                        + DISPLAY_RATE_MIN;
}

/**
 * \file rtc.c
 *
 * \date создан: 26 окт. 2015 г.
 * \author Nick Egorrov
 */

#include "../rtc.h"

#define HAL_RTC_CHIPS_IMPL_C_

static void impl_begin();
static void impl_index(uint8_t i);
static uint8_t impl_read(uint8_t last);
static void impl_write(uint8_t d);
static void impl_end();

static rtc_error_t s_error(char flag, rtc_error_t err);

static void (*update_handler)();
static void (*alarm_handler)(unsigned char alarm_id);
static void (*error_handler)(rtc_error_t error);

extern void rtc_init(void (*update)(), void (*alarm)(unsigned char alarm_id),
                void (*error)(rtc_error_t error))
{
        update_handler = update;
        alarm_handler = alarm;
        error_handler = error;
}

extern rtc_error_t rtc_error()
{
        return s_error(0, RTC_NO_ERROR);
}

extern void rtc_clear()
{
        s_error(1, RTC_NO_ERROR);
}

extern void rtc_check()
{
        static uint8_t old = 0, current;

        impl_begin();
        impl_index(0);
        current = impl_read(0);
        impl_end();

        if (old != current) {
                old = current;
                update_handler();
        }
}

extern rtc_error_t rtc_get_time(bcd_time_t * time)
{
        return rtc_error();
}

extern rtc_error_t rtc_set_time(bcd_time_t * time)
{
        return rtc_error();
}

extern rtc_error_t rtc_get_date(bcd_date_t * date)
{
        return rtc_error();
}

extern rtc_error_t rtc_set_date(bcd_date_t * date)
{
        return rtc_error();
}

static rtc_error_t s_error(char flag, rtc_error_t err)
{
        static rtc_error_t error = RTC_NO_ERROR;
        if (flag != 0) {
                error = err;

                if (error != RTC_NO_ERROR && error_handler != 0) {
                        error_handler(error);
                }
        }
        return error;
}

#include "chips_impl.h"

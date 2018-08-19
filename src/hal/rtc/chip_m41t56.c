/**
 * \file chip_m41t56.c
 * \brief Реализация RTC M41T56
 * \details
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

#ifndef __RTC_M41T56__
#define __RTC_M41T56__
#endif

#include "../rtc.h"
#include "../iic.h"

#define IIC_ADDRESS 0xd0

static rtc_error_t rtcError;

static void (*update_handler)();
static void (*alarm_handler)(unsigned char alarm_id);
static void (*error_handler)(rtc_error_t rtcError);

extern void rtc_init_m41t56(void (*update)(),
                void (*alarm)(unsigned char alarm_id),
                void (*error)(rtc_error_t error))
{
        update_handler = update;
        alarm_handler = alarm;
        error_handler = error;
}

static void fire_error(rtc_error_t e)
{
        rtcError = e;
        if (e != RTC_NO_ERROR && error_handler != 0) {
                error_handler(e);
        }
}

extern rtc_error_t rtc_error_m41t56()
{
        return rtcError;
}

extern void rtc_clear_m41t56()
{
        if (iic_error() != IIC_NO_ERROR) {
                iic_clear();
        }
        fire_error(RTC_NO_ERROR);
}

extern void rtc_check_m41t56()
{
}

extern rtc_error_t rtc_get_time_m41t56(bcd_time_t * time)
{
        return rtc_error_m41t56();
}

extern rtc_error_t rtc_set_time_m41t56(bcd_time_t * time)
{
        return rtc_error_m41t56();
}

extern rtc_error_t rtc_get_date_m41t56(bcd_date_t * date)
{
        return rtc_error_m41t56();
}

extern rtc_error_t rtc_set_date_m41t56(bcd_date_t * date)
{
        return rtc_error_m41t56();
}

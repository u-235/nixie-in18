/**
 * @file
 *
 * @date created on: 02 нояб. 2015 г.
 * @author Nick Egorrov
 */

#include "alarm.h"
#include "hal/mcu.h"
#include "hal/display.h"
#include "hal/rtc.h"
#include "tms/tms.h"
#include "show.hpp"
#include "user.h"

#define ERROR_CHECK_PERIOD      1200

static time_t time;
static date_t date;

static void init();
static void loop();
static void check_error();
static void on_rtc_failture();

static timer_id_t timer_check;

int main(void)
{
        init();
        loop();
        return 0;
}

static void init()
{
        mcu_init();
        display_init();
        tms_init();
        timer_check = tms_create_timer(&check_error);
        tms_set_timer(timer_check, _ticks_from_ms(ERROR_CHECK_PERIOD));
        tms_start_timer(timer_check);
        Show::init(&time, &date);
        rtc_init();
        user_init();
        alarm_init();
        check_error();
        mcu_interrupt_unlock();
}

static void loop()
{
        while (1) {
                if (mcu_get_timer_fire() != 0) {
                        tms_tick();
                } else {
                        Show::handle_key(user_get_key());
                        if (rtc_check() != 0) {
                                rtc_get_time(&time);
                                rtc_get_date(&date);
                                alarm_check(&time);
                                Show::synchronize();
                                days_from_millenium(&date);
                        }
                }
        }
}

extern void check_error()
{
        rtc_error_t error = rtc_error();

        if (error == RTC_NO_ERROR) {
                return;
        }

        rtc_clear();

        if (error == RTC_POWER_ERROR) {
                on_rtc_failture();
                return;
        }

        Show::show_error(1);
}

extern void on_rtc_failture(){
        Show::rtc_failture();
}

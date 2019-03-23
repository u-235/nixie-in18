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
#include "sensor.h"
#include "user.h"

static void init();
static void loop();
static void idle();
static void check_error();
static void on_rtc_failture();

int main(void)
{
        init();
        loop();
        return 0;
}

void init()
{
        const struct rtc_tm *p_tm;

        mcu_init();
        tms_init();
        mcu_interrupt_unlock();
        sensor_init();
        display_init();
        rtc_init();
        p_tm = rtc_get_time();
        Show::init(p_tm);
        user_init();
        alarm_init(p_tm);
}

void loop()
{
        while (1) {
                if (mcu_get_timer_fire() != 0) {
                        tms_tick();
                } else {
                        idle();
                }
        }
}

void idle()
{
        check_error();
        Show::handle_key(user_get_key());
        if (rtc_check() != 0) {
                rtc_sync();
                alarm_check();
                Show::synchronize();
        }

}

void check_error()
{
        uint8_t error = rtc_error();

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

void on_rtc_failture()
{
        // TODO reset alarm
        struct rtc_tm ttm;
        ttm.seconds = 7;
        ttm.minutes = 7;
        ttm.hours = 7;
        rtc_set_time(&ttm);
        ttm.day = 12;
        ttm.month = 12;
        ttm.year = 18;
        ttm.actual = 0;
        rtc_date_adjust(&ttm);
        rtc_set_date(&ttm);
}

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

static time_t time;
static date_t date;

static void init();
static void loop();

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
        rtc_init();
        tms_init();
        Show::init(&time, &date);
        user_init();
        alarm_init();
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
                        }
                }
        }
}

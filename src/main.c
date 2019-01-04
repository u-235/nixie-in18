/**
 * \file main.c
 *
 * \date created on: 02 нояб. 2015 г.
 * \author Nick Egorrov
 */

#include "hal/mcu.h"
#include "hal/display.h"
#include "hal/rtc.h"
#include "tms/tms.h"
#include "show.h"
#include "user.h"

static bcd_time_t time;
static bcd_date_t date;

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
        show_init(&time, &date);
        user_init();
        mcu_interrupt_unlock();
}

static void loop()
{
        while (1) {
                if (mcu_get_timer_fire() != 0) {
                        tms_tick();
                } else {
                        show_handle_key(user_get_key());
                        if (rtc_check() != 0) {
                                rtc_get_time(&time);
                                rtc_get_date(&date);
                                show_synchronize();
                        }
                }
        }
}

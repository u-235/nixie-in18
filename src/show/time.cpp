/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 */

#include "childs.hpp"
#include "../alarm.h"
#include "../config.h"

void ShowTime::on_start()
{
        tms_stop_timer(timer_back);
}

void ShowTime::on_update()
{
        display->enabled = DISPLAY_ENABLED_ALL;
        display->hours = time->hours;
        display->minutes = time->minutes;
        display->seconds = time->seconds;
        display->marks = display_make_mark(date->week_day, alarm_is_on());
        display->dots = DISPLAY_DOT_BOTTOM;
}

void ShowTime::on_hide()
{
        if (flags.rtc_power_fail == 0) {
                display->dots = DISPLAY_DOT_HIDE;
        } else {
                display_clean();
        }
}

void ShowTime::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                mode(SHOW_SET_ALARM);
        }

        if (_key == VK_SELECT_DOWN) {
                mode(SHOW_ALARM);
        }

        if (_key == VK_CHANGE_UP) {
                mode(SHOW_DATE);
        }
}


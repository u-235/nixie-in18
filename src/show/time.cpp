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
        display_hours(time->hour);
        display_minutes(time->min);
        display_seconds(time->sec);
        display_day(date->day, alarm_is_on());
        display_dots(DISPLAY_DOT_BOTTOM);
}

void ShowTime::on_hide()
{
        if (flags.rtc_power_fail == 0) {
                display_dots(DISPLAY_DOT_HIDE);
        } else {
                display_clean();
        }
}

void ShowTime::on_key(const key_t _key)
{
        if(_key == VK_MENU_UP){
                mode(SHOW_SET_ALARM);
        }

        if(_key == VK_SELECT_DOWN){
                mode(SHOW_ALARM);
        }

        if(_key == VK_CHANGE_UP){
                mode(SHOW_DATE);
        }
}


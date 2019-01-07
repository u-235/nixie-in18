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

void ShowDate::on_start()
{
        tms_set_timer(timer_back, _ticks_from_ms(CFG_SHOW_DURATION_DATE));
        tms_start_timer(timer_back);
}

void ShowDate::on_stop(){
        tms_stop_timer(timer_back);
}

void ShowDate::on_update()
{
        display_hours(date->date);
        display_minutes(date->month);
        display_seconds(date->year);
        display_day(date->day, alarm_is_on());
        display_dots(DISPLAY_DOT_ALL);
}

void ShowDate::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                        mode(SHOW_TIME);
        }
}


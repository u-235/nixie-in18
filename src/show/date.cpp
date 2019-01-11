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

void ShowDate::on_stop()
{
        tms_stop_timer(timer_back);
}

void ShowDate::on_update()
{
        display->enabled = DISPLAY_ENABLED_ALL;
        display->day = date->day;
        display->month = date->month;
        display->year = date->year;
        display->marks = display_make_mark(date->week_day, alarm_is_on());
        display->dots = DISPLAY_DOT_ALL;
}

void ShowDate::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                mode(SHOW_TIME);
        }
}


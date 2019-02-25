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
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_DATE));
        tms_start_timer(timer_back_);
}

void ShowDate::on_stop()
{
        tms_stop_timer(timer_back_);
}

void ShowDate::on_update()
{
        display_->enabled = DISPLAY_ENABLED_ALL;
        display_->day = time_ptr_->day;
        display_->month = time_ptr_->month;
        display_->year = time_ptr_->year;
        display_->marks = display_make_mark(time_ptr_->week_day, alarm_is_on());
        display_->dots = DISPLAY_DOT_ALL;
}

void ShowDate::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                mode(SHOW_TIME);
        }
}


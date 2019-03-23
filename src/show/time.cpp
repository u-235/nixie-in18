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
        tms_stop_timer(timer_back_);
}

void ShowTime::on_update()
{
        fill();
        display_->dots = DISPLAY_DOT_BOTTOM;
}

void ShowTime::on_hide()
{
        if (_rtc_is_time_actual(time_ptr_->actual)) {
                fill();
                display_->dots = DISPLAY_DOT_HIDE;
        } else {
                display_clean();
        }
}

void ShowTime::on_key(const uint8_t _key)
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

void ShowTime::fill()
{
        display_->enabled = DISPLAY_ENABLED_ALL;
        display_->hours = time_ptr_->hours;
        display_->minutes = time_ptr_->minutes;
        display_->seconds = time_ptr_->seconds;
        display_->marks = display_make_mark(time_ptr_->week_day, alarm_is_on());
}

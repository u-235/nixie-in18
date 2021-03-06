/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "childs.hpp"
#include "../config.h"

struct rtc_tm ShowSetDate::time_;

void ShowSetDate::on_start()
{
        ShowSetter::on_start();
        rtc_copy(&time_, time_ptr_);
        time_.actual |= RTC_BITS_ACTUAL_DATE | RTC_BIT_ACTUAL_WDAY;
}

void ShowSetDate::on_stop()
{
        ShowSetter::on_stop();
        if (flag_ != 0)
                rtc_set_date(&time_);
}

void ShowSetDate::on_update()
{
        ShowSetter::on_update();
        display_->enabled = DISPLAY_ENABLED_ALL;
        display_->day = time_.day;
        display_->month = time_.month;
        display_->year = time_.year;
        display_->marks = display_make_mark(time_.week_day, 0);
}

void ShowSetDate::on_hide()
{
        uint8_t tmp;

        tms_run_timer(timer_update_);
        tmp = display_->enabled;
        switch (state_) {
        case 0:
                tmp &= ~DISPLAY_ENABLED_SECONDS;
                break;
        case 1:
                tmp &= ~DISPLAY_ENABLED_MINUTES;
                break;
        case 2:
                tmp &= ~DISPLAY_ENABLED_HOURS;
                break;
        }
        display_->enabled = tmp;
}

void ShowSetDate::on_key(const uint8_t _key)
{
        ShowSetter::on_key( _key);

        if (_key != VK_CHANGE_DOWN) {
                return;
        }

        flag_ = 1;

        switch (state_) {
        case 0:
                time_.year++;
                break;
        case 1:
                time_.month++;
                break;
        case 2:
                time_.day++;
                break;
        }

        rtc_validate_date(&time_, 0);
        refresh();
}


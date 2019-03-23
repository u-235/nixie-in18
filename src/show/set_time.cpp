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

struct rtc_tm ShowSetTime::time_;

void ShowSetTime::on_start()
{
        ShowSetter::on_start();
        rtc_copy(&time_, time_ptr_);
        time_.actual |= RTC_BITS_ACTUAL_TIME;
}

void ShowSetTime::on_stop()
{
        ShowSetter::on_stop();
        if (flag_ != 0)
                rtc_set_time(&time_);
}

void ShowSetTime::on_update()
{
        ShowSetter::on_update();
        display_->enabled = DISPLAY_ENABLED_ALL;
        display_->hours = time_.hours;
        display_->minutes = time_.minutes;
        display_->seconds = time_.seconds;
        display_->marks = 0;
        display_->dots = DISPLAY_DOT_BOTTOM;
}

void ShowSetTime::on_hide()
{
        ShowSetter::on_hide();
        display_->dots = DISPLAY_DOT_TOP;
}

void ShowSetTime::on_sync()
{
        time_.seconds = time_ptr_->seconds;
        if (flag_ != 0)
                return;
        time_.minutes = time_ptr_->minutes;
        time_.hours = time_ptr_->hours;
}

void ShowSetTime::on_key(const uint8_t _key)
{
        ShowSetter::on_key(_key);

        if (state_ == 2 && flag_ != 0) {
                flag_ = 0;
                rtc_set_time(&time_);
        }

        if (_key == VK_CHANGE_DOWN) {
                switch (state_) {
                case 0:
                        time_.hours++;
                        if (time_.hours >= 24) {
                                time_.hours = 0;
                        }
                        break;
                case 1:
                        time_.minutes++;
                        if (time_.minutes >= 60) {
                                time_.minutes = 0;
                        }
                        break;
                case 2:
                        return;
                }
                flag_ = 1;
        } else if (_key == VK_CHANGE_UP && state_ == 2) {
                if (time_.seconds >= 30) {
                        time_.minutes++;
                        if (time_.minutes >= 60) {
                                time_.minutes = 0;
                                time_.hours++;
                        }
                        if (time_.hours >= 24) {
                                time_.hours = 0;
                        }
                }
                time_.seconds = 0;
                rtc_set_time(&time_);
        } else return;

        refresh();
}


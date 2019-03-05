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
#include "../hal/mcu.h"

static alarm_t alarm;

void ShowSetAlarm::on_start()
{
        ShowSetter::on_start();
        alarm_get(&alarm);
        flag_ = alarm_is_on();
}

void ShowSetAlarm::on_stop()
{
        ShowSetter::on_stop();
        mcu_output_player(0);
        alarm_set(&alarm);
        if (flag_ != 0) {
                alarm_on();
        } else {
                alarm_off();
        }
}

void ShowSetAlarm::on_update()
{
        ShowSetter::on_update();
        display_->enabled =
        DISPLAY_ENABLED_TIME | DISPLAY_ENABLED_SECONDS_UNITS;
        display_->hours = alarm.hours;
        display_->minutes = alarm.minutes;
        display_->seconds = alarm.sound;
        if (flag_ != 0) {
                display_->marks = DISPLAY_MARK_ALARM;
        }
}

void ShowSetAlarm::on_hide()
{
        ShowSetter::on_hide();
}

void ShowSetAlarm::on_key(const key_t _key)
{
        ShowSetter::on_key(_key);

        if (_key == VK_SELECT_UP) {
                if (flag_ != 0) {
                        flag_ = 0;
                } else {
                        flag_ = 1;
                }
        } else if (_key == VK_CHANGE_DOWN) {
                switch (state_) {
                case 0:
                        alarm.hours++;
                        if (alarm.hours >= 24) {
                                alarm.hours = 0;
                        }
                        break;
                case 1:
                        alarm.minutes++;
                        if (alarm.minutes >= 60) {
                                alarm.minutes = 0;
                        }
                        break;
                case 2:
                        alarm.sound++;
                        if (alarm.sound >= 10) {
                                alarm.sound = 1;
                        }
                        mcu_output_player(alarm.sound);
                        break;
                }
        } else return;

        refresh();
}

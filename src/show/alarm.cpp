/**
 * @file
 * @brief
 * @details
 *
 * @date создан 07.01.2019
 * @author Nick Egorrov
 */

#include "childs.hpp"
#include "../alarm.h"
#include "../config.h"
#include "../hal/mcu.h"

static alarm_t alarm;

void ShowAlarm::on_start()
{
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back_);
        alarm_get(&alarm);
}

void ShowAlarm::on_stop()
{
        tms_stop_timer(timer_back_);
}

void ShowAlarm::on_update()
{
        display_->enabled = DISPLAY_ENABLED_TIME | DISPLAY_ENABLED_SECONDS_UNITS;
        display_->hours = alarm.hours;
        display_->minutes = alarm.minutes;
        display_->seconds = alarm.sound;
        if (alarm_is_on() != 0) {
                display_->marks = DISPLAY_MARK_ALARM;
        }
}

void ShowAlarm::on_key(const uint8_t _key)
{
        if (_key == VK_MENU_UP) {
                mode(SHOW_TIME);
        }
}


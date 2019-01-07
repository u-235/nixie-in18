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
        tms_set_timer(timer_back, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back);
        alarm_get(&alarm);
}

void ShowAlarm::on_stop()
{
        tms_stop_timer(timer_back);
}

void ShowAlarm::on_update()
{
        display_hours(alarm.hour);
        display_minutes(alarm.min);
        display_seconds(0xf0 | alarm.sound);
        if (alarm_is_on() != 0) {
                display_day_marks(DISPLAY_MARK_ALARM);
        }
}

void ShowAlarm::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                        mode(SHOW_TIME);
        }
}





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
static uint8_t flag;
static uint8_t state;

void ShowSetAlarm::on_start()
{
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back_);
        tms_set_timer(timer_update_, _ticks_from_ms(300));
        tms_set_timer(timer_hide_, _ticks_from_ms(700));
        alarm_get(&alarm);
        flag = alarm_is_on();
        state = 0;
}

void ShowSetAlarm::on_stop()
{
        tms_stop_timer(timer_back_);
        tms_stop_timer(timer_update_);
        tms_stop_timer(timer_hide_);
        alarm_set(&alarm);
        if (flag != 0) {
                alarm_on();
        } else {
                alarm_off();
        }
}

void ShowSetAlarm::on_update()
{
        display_->enabled = DISPLAY_ENABLED_TIME | DISPLAY_ENABLED_SECONDS_UNITS;
        display_->hours = alarm.hours;
        display_->minutes = alarm.minutes;
        display_->seconds = alarm.sound;
        if (flag != 0) {
                display_->marks = DISPLAY_MARK_ALARM;
        }
        tms_run_timer(timer_hide_);
}

void ShowSetAlarm::on_hide()
{
        switch (state) {
        case 0:
                display_->enabled = DISPLAY_ENABLED_MINUTES
                                | DISPLAY_ENABLED_SECONDS_UNITS;
                break;
        case 1:
                display_->enabled = DISPLAY_ENABLED_HOURS
                                | DISPLAY_ENABLED_SECONDS_UNITS;
                break;
        case 2:
                display_->enabled = DISPLAY_ENABLED_TIME;
                break;
        }
        tms_run_timer(timer_update_);
}

void ShowSetAlarm::on_key(const key_t _key)
{
        if (_key == VK_MENU_UP) {
                state++;
                if (state > 2) {
                        mcu_output_player(0);
                        mode(SHOW_SET_TIME);
                        return;
                }
                tms_start_timer(timer_back_);
                update();
                hide();
        }

        if (_key == VK_SELECT_DOWN) {
                if (flag != 0) {
                        flag = 0;
                } else {
                        flag = 1;
                }
                tms_start_timer(timer_back_);
                update();
        }

        if (_key != VK_CHANGE_DOWN) {
                return;
        }

        switch (state) {
        case 0:
                alarm.hours++;
                if (alarm.hours > 23) {
                        alarm.hours = 0;
                }
                break;
        case 1:
                alarm.minutes++;
                if (alarm.minutes > 59) {
                        alarm.minutes = 0;
                }
                break;
        case 2:
                alarm.sound++;
                if (alarm.sound > 9) {
                        alarm.sound = 1;
                }
                mcu_output_player(alarm.sound);
                break;
        }
        tms_start_timer(timer_back_);
        update();
}

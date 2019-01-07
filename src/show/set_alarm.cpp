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
        tms_set_timer(timer_back, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back);
        tms_set_timer(timer_update, _ticks_from_ms(300));
        tms_set_timer(timer_hide, _ticks_from_ms(700));
        alarm_get(&alarm);
        flag = alarm_is_on();
        state = 0;
}

void ShowSetAlarm::on_stop()
{
        tms_stop_timer(timer_back);
        tms_stop_timer(timer_update);
        tms_stop_timer(timer_hide);
        alarm_set(&alarm);
        if (flag != 0) {
                alarm_on();
        } else {
                alarm_off();
        }
}

void ShowSetAlarm::on_update()
{
        display_hours(alarm.hour);
        display_minutes(alarm.min);
        display_seconds(0xf0 | alarm.sound);
        if (flag != 0) {
                display_day_marks(DISPLAY_MARK_ALARM);
        }
        tms_run_timer(timer_hide);
}

void ShowSetAlarm::on_hide()
{
        switch (state) {
        case 0:
                display_hours(0xff);
                break;
        case 1:
                display_minutes(0xff);
                break;
        case 2:
                display_seconds(0xff);
                break;
        }
        tms_run_timer(timer_update);
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
                tms_start_timer(timer_back);
                update();
                hide();
        }

        if (_key == VK_SELECT_DOWN) {
                if (flag != 0) {
                        flag = 0;
                } else {
                        flag = 1;
                }
                tms_start_timer(timer_back);
                update();
        }

        if (_key != VK_CHANGE_DOWN) {
                return;
        }

        switch (state) {
        case 0:
                alarm.hour = bcd2_inc(alarm.hour);
                if (alarm.hour > 0x23) {
                        alarm.hour = 0;
                }
                break;
        case 1:
                alarm.min = bcd2_inc(alarm.min);
                if (alarm.min > 0x59) {
                        alarm.min = 0;
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
        tms_start_timer(timer_back);
        update();
}

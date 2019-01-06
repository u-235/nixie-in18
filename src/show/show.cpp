/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 */

#include "../alarm.h"
#include "../config.h"
#include "../hal/rtc.h"
#include "childs.hpp"

#define ERROR_CHECK_PERIOD      1200

Show::show_t Show::show = SHOW_VOID;
rtc_error_t Show::error;
const bcd_time_t *Show::time;
const bcd_date_t *Show::date;
struct Show::_flags Show::flags = {
                0
};
timer_id_t Show::timer_update, Show::timer_hide, Show::timer_check,
                Show::timer_back;

void Show::init(const bcd_time_t *pt, const bcd_date_t *pd)
{
        time = pt;
        date = pd;
        timer_hide = tms_create_timer(&hide);
        timer_update = tms_create_timer(&update);
        timer_check = tms_create_timer(&check_error);
        timer_back = tms_create_timer(&back_show_time);
        tms_set_timer(timer_check, _ticks_from_ms(ERROR_CHECK_PERIOD));
        tms_start_timer(timer_check);
        mode(SHOW_INTRO);
}

void Show::synchronize()
{
        if (show != SHOW_DATE && show != SHOW_TIME) {
                return;
        }

        tms_set_timer(timer_hide, _ticks_from_ms(CFG_SHOW_BLINK_DURATION));
        tms_run_timer(timer_hide);
        update();
}

void Show::mode(show_t _show)
{
        show_t old = show;
        if (show == _show) {
                return;
        }

        show = _show;

        switch (old) {
        case SHOW_ERROR:
                ShowError::on_stop();
                break;
        case SHOW_INTRO:
                ShowIntro::on_stop();
                break;
        case SHOW_DATE:
                ShowDate::on_stop();
                break;
        case SHOW_ALARM:
                ShowAlarm::on_stop();
                break;
        case SHOW_SET_ALARM:
                ShowSetAlarm::on_stop();
                break;
        case SHOW_SET_TIME:
                ShowSetTime::on_stop();
                break;
        case SHOW_SET_DATE:
                ShowSetDate::on_stop();
                break;
        case SHOW_SET_CALIBER:
                ShowSetCaliber::on_stop();
                break;
        default:
                ;
        }

        switch (show) {
        case SHOW_ERROR:
                ShowError::on_start();
                break;
        case SHOW_INTRO:
                ShowIntro::on_start();
                break;
        case SHOW_TIME:
                ShowTime::on_start();
                break;
        case SHOW_DATE:
                ShowDate::on_start();
                break;
        case SHOW_ALARM:
                ShowAlarm::on_start();
                break;
        case SHOW_SET_ALARM:
                ShowSetAlarm::on_start();
                break;
        case SHOW_SET_TIME:
                ShowSetTime::on_start();
                break;
        case SHOW_SET_DATE:
                ShowSetDate::on_start();
                break;
        case SHOW_SET_CALIBER:
                ShowSetCaliber::on_start();
                break;
        default:
                ;
        }
        update();
}

void Show::check_error()
{
        error = rtc_error();

        if (error == RTC_NO_ERROR) {
                return;
        }

        rtc_clear();

        if (error == RTC_POWER_ERROR) {
                flags.rtc_power_fail = 1;
                return;
        }

        if (show == SHOW_INTRO) {
                return;
        }

        mode(SHOW_ERROR);
        update();
}

void Show::back_show_time()
{
        mode(SHOW_TIME);
}

void Show::update()
{
        display_clean();

        switch (show) {
        case SHOW_ERROR:
                ShowError::on_update();
                break;
        case SHOW_INTRO:
                ShowIntro::on_update();
                break;
        case SHOW_TIME:
                ShowTime::on_update();
                break;
        case SHOW_DATE:
                ShowDate::on_update();
                break;
        case SHOW_ALARM:
                ShowAlarm::on_update();
                break;
        case SHOW_SET_ALARM:
                ShowSetAlarm::on_update();
                break;
        case SHOW_SET_TIME:
                ShowSetTime::on_update();
                break;
        case SHOW_SET_DATE:
                ShowSetDate::on_update();
                break;
        case SHOW_SET_CALIBER:
                ShowSetCaliber::on_update();
                break;
        default:
                ;
        }

        display_flush();
}

void Show::hide()
{
        switch (show) {
        case SHOW_TIME:
                ShowTime::on_hide();
                break;
        case SHOW_SET_ALARM:
                ShowSetAlarm::on_hide();
                break;
        case SHOW_SET_TIME:
                ShowSetTime::on_hide();
                break;
        case SHOW_SET_DATE:
                ShowSetDate::on_hide();
                break;
        case SHOW_SET_CALIBER:
                ShowSetCaliber::on_hide();
                break;
        default:
                ;
        }

        display_flush();
}

void Show::handle_key(const key_t _key)
{
        if (_key == VK_NO) {
                return;
        }

        if (alarm_is_jingle() != 0 && _key == VK_SELECT_DOWN) {
                alarm_off();
                return;
        }

        switch (show) {
        case SHOW_TIME:
                ShowTime::on_key(_key);
                break;
        case SHOW_DATE:
                ShowDate::on_key(_key);
                break;
        case SHOW_ALARM:
                ShowAlarm::on_key(_key);
                break;
        case SHOW_SET_ALARM:
                ShowSetAlarm::on_key(_key);
                break;
        case SHOW_SET_TIME:
                ShowSetTime::on_key(_key);
                break;
        case SHOW_SET_DATE:
                ShowSetDate::on_key(_key);
                break;
        case SHOW_SET_CALIBER:
                ShowSetCaliber::on_key(_key);
                break;
        default:
                ;
        };
}

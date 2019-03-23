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
#include "../sensor.h"
#include "childs.hpp"

struct display *Show::display_;
uint8_t Show::show_ = SHOW_VOID;
uint8_t Show::error_;
bool Show::enable_auto_bright_ = false;
bool Show::enable_auto_rate_ = false;
const struct rtc_tm *Show::time_ptr_;
timer_id_t Show::timer_update_, Show::timer_hide_, Show::timer_back_,
                Show::timer_sensor_;

void Show::init(const struct rtc_tm *p_tm)
{
        time_ptr_ = p_tm;
        display_ = display_get();
        timer_hide_ = tms_create_timer(&hide);
        timer_update_ = tms_create_timer(&update);
        timer_back_ = tms_create_timer(&back_show_time);
        timer_sensor_ = tms_create_timer(&check_sensor);
        tms_set_timer(timer_sensor_, _ticks_from_ms(CFG_SENSOR_REQUEST));
        tms_start_timer(timer_sensor_);
        mode(SHOW_INTRO);
}

void Show::synchronize()
{
        if (show_ == SHOW_DATE || show_ == SHOW_TIME) {
                tms_set_timer(timer_hide_,
                                _ticks_from_ms(CFG_SHOW_BLINK_DURATION));
                tms_run_timer(timer_hide_);
                update();
        } else if (show_ == SHOW_SET_TIME)
                ShowSetTime::on_sync();
}

void Show::mode(uint8_t _show)
{
        uint8_t old = show_;
        if (show_ == _show) {
                return;
        }

        show_ = _show;

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

        switch (show_) {
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

void Show::next_mode()
{
        uint8_t next;

        switch (show_) {
        case SHOW_INTRO:
                next = SHOW_DATE;
                break;
        case SHOW_TIME:
                next = SHOW_SET_ALARM;
                break;
        case SHOW_SET_ALARM:
                next = SHOW_SET_TIME;
                break;
        case SHOW_SET_TIME:
                next = SHOW_SET_DATE;
                break;
        case SHOW_SET_DATE:
                next = SHOW_SET_CALIBER;
                break;
        default:
                next = SHOW_TIME;
        }

        mode(next);
}

void Show::show_error(uint8_t _error)
{
        error_ = _error;

        if (show_ == SHOW_INTRO || _error == 0) {
                return;
        }

        mode(Show::SHOW_ERROR);
        update();
}

void Show::back_show_time()
{
        mode(Show::SHOW_TIME);
}

void Show::update()
{
        display_clean();

        switch (show_) {
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
        switch (show_) {
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

void Show::handle_key(const uint8_t _key)
{
        if (_key == VK_NO) {
                return;
        }

        if (alarm_is_jingle() != 0 && _key == VK_SELECT_DOWN) {
                alarm_off();
                return;
        }

        switch (show_) {
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

void Show::check_sensor()
{
        if (enable_auto_bright_)
                display_bright(sensor_get_bright());
        if (enable_auto_rate_)
                display_rate(sensor_get_rate());
}

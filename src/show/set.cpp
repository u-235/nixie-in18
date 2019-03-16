/**
 * @file
 * @brief
 * @details
 *
 * @date создан 02.03.2019
 * @author Nick Egorrov
 */

#include "childs.hpp"
#include "../config.h"

uint8_t ShowSetter::flag_;
uint8_t ShowSetter::state_;

void ShowSetter::on_start()
{
        enable_auto_rate_ = false;
        display_rate(DISPLAY_RATE_MAX);
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back_);
        tms_set_timer(timer_update_, _ticks_from_ms(300));
        tms_set_timer(timer_hide_, _ticks_from_ms(700));
        flag_ = 0;
        state_ = 0;
}

void ShowSetter::on_stop()
{
        enable_auto_rate_ = true;
        tms_stop_timer(timer_back_);
        tms_stop_timer(timer_update_);
        tms_stop_timer(timer_hide_);
}

void ShowSetter::on_update()
{
        tms_run_timer(timer_hide_);
}

void ShowSetter::on_hide()
{
        uint8_t tmp;

        tms_run_timer(timer_update_);
        tmp = display_->enabled;
        switch (state_) {
        case 0:
                tmp &= ~DISPLAY_ENABLED_HOURS;
                break;
        case 1:
                tmp &= ~DISPLAY_ENABLED_MINUTES;
                break;
        case 2:
                tmp &= ~DISPLAY_ENABLED_SECONDS;
                break;
        }
        display_->enabled = tmp;
}

void ShowSetter::on_key(const key_t _key)
{
        if (_key != VK_MENU_UP)
                return;

        state_++;
        if (state_ > 2) {
                next_mode();
                return;
        }
        refresh();
        hide();
}

void ShowSetter::refresh()
{
        tms_start_timer(timer_back_);
        update();
}

/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 */

#include "childs.hpp"
#include "../config.h"

void ShowIntro::on_start()
{
        tms_set_timer(timer_update_, _ticks_from_ms(CFG_SHOW_INTRO_PERIOD));
        tms_start_timer(timer_update_);
        enable_auto_bright_ = false;
        enable_auto_rate_ = false;
        display_bright(DISPLAY_BRIGHT_MAX - (DISPLAY_BRIGHT_MAX - DISPLAY_BRIGHT_MIN) / 3);
}

void ShowIntro::on_stop()
{
        enable_auto_bright_ = true;
        enable_auto_rate_ = true;
        tms_stop_timer(timer_update_);
}

void ShowIntro::on_update()
{
        static uint8_t phase = 0, count = 0;

        switch (phase) {
        case 0:
                display_->dots = DISPLAY_DOT_ALL;
                break;
        case 1:
                if (count < 8) {
                        uint8_t day = 1 << count;
                        display_->marks = day;
                        count++;
                        return;
                }
                break;
        case 2:
                if (count++ < 10) {
                        display_->enabled = DISPLAY_ENABLED_ALL;
                        uint8_t t = (10 - count) * 11;
                        display_->hours = t;
                        display_->minutes = t;
                        display_->seconds = t;
                        return;
                }
                break;
        default:
                count = 0;
                phase = 0;
                mode(SHOW_DATE);
                return;
        }

        count = 0;
        phase++;
}

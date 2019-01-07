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
        tms_set_timer(timer_update, _ticks_from_ms(CFG_SHOW_INTRO_PERIOD));
        tms_start_timer(timer_update);
}

void ShowIntro::on_stop()
{
        tms_stop_timer(timer_update);
}

void ShowIntro::on_update()
{
        static uint8_t phase = 0, count = 0;

        switch (phase) {
        case 0:
                display_dots(DISPLAY_DOT_ALL);
                break;
        case 1:
                if (count < 8) {
                        uint8_t day = 1 << count;
                        display_day_marks(day);
                        count++;
                        return;
                }
                break;
        case 2:
                if (count++ < 10) {
                        bcd2_t t = ((10 - count) << 4) | (10 - count);
                        display_hours(t);
                        display_minutes(t);
                        display_seconds(t);
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

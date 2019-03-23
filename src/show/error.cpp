/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "childs.hpp"
#include "../config.h"

void ShowError::on_start()
{
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_ERROR));
        tms_start_timer(timer_back_);
}

void ShowError::on_stop()
{
        error_ = 0;
        tms_stop_timer(timer_back_);
}

void ShowError::on_update()
{
        display_->enabled = DISPLAY_ENABLED_HOURS |
                                DISPLAY_ENABLED_SECONDS_UNITS;
        display_->hours = 6;
        display_->seconds = error_;
}

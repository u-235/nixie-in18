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

void ShowError::on_start()
{
        tms_set_timer(timer_back, _ticks_from_ms(CFG_SHOW_DURATION_ERROR));
        tms_start_timer(timer_back);
}

void ShowError::on_stop()
{
        tms_stop_timer(timer_back);
}

void ShowError::on_update()
{
        display_hours(_bcd2_make(06));
        display_seconds(0xa1);
}

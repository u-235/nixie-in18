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

void ShowSetTime::on_start()
{
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back_);
}

void ShowSetTime::on_stop()
{
        tms_stop_timer(timer_back_);
}

void ShowSetTime::on_update()
{
}

void ShowSetTime::on_hide()
{
}

void ShowSetTime::on_key(const key_t _key)
{
}


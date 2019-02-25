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

void ShowSetCaliber::on_start()
{
        tms_set_timer(timer_back_, _ticks_from_ms(CFG_SHOW_DURATION_SETTINGS));
        tms_start_timer(timer_back_);
}

void ShowSetCaliber::on_stop()
{
        tms_stop_timer(timer_back_);
}

void ShowSetCaliber::on_update()
{
}

void ShowSetCaliber::on_hide()
{
}

void ShowSetCaliber::on_key(const key_t _key)
{
}


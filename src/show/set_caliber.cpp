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

static int8_t caliber;

static uint8_t abs8(int8_t s);

void ShowSetCaliber::on_start()
{
        ShowSetter::on_start();
        state_ = 1;
        caliber = rtc_get_caliber();
}

void ShowSetCaliber::on_stop()
{
        ShowSetter::on_stop();
        if (caliber != rtc_get_caliber()) {
                rtc_set_caliber(caliber);
        }
}

void ShowSetCaliber::on_update()
{
        int8_t diff;
        uint8_t dots;

        ShowSetter::on_update();
        diff = rtc_get_deviation();
        display_->enabled = DISPLAY_ENABLED_MINUTES | DISPLAY_ENABLED_SECONDS;
        display_->minutes = abs8(diff);
        display_->seconds = abs8(caliber);
        display_->marks = 0;
        dots = diff >= 0 ? DISPLAY_DOT_LEFT_TOP : DISPLAY_DOT_LEFT_TOP;
        dots |= caliber >= 0 ? DISPLAY_DOT_RIGHT_TOP : DISPLAY_DOT_RIGHT_BOTTOM;
        display_->dots = dots;
}

void ShowSetCaliber::on_key(const key_t _key)
{
        ShowSetter::on_key(_key);

        switch (state_) {
        case 1:
                if (_key == VK_SELECT_UP) {
                        rtc_apply_corrector();
                        caliber = rtc_get_caliber();
                } else if (_key == VK_CHANGE_UP) {
                        rtc_reset_corrector();
                } else return;
                break;
        case 2:
                if (_key == VK_SELECT_DOWN) {
                        caliber--;
                } else if (_key == VK_CHANGE_DOWN) {
                        caliber++;
                } else return;
                caliber = rtc_limit_caliber(caliber);
                break;
        }

        refresh();
}

uint8_t abs8(int8_t s)
{
        return s < 0 ? static_cast<uint8_t>(-s) : s;
}

/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 */

#ifndef SRC_SHOW_HPP_
#define SRC_SHOW_HPP_

#include "hal/rtc.h"
#include "hal/display.h"
#include "hal/rtc.h"
#include "tms/tms.h"
#include "user.h"

class Show {
public:
        static void init(const rtc_tm *p_tm);

        static void synchronize();

        static void handle_key(const uint8_t _key);

        static void show_error(uint8_t _error);

protected:
        enum {
                SHOW_VOID,
                SHOW_INTRO,
                SHOW_ERROR,
                SHOW_TIME,
                SHOW_DATE,
                SHOW_ALARM,
                SHOW_SET_ALARM,
                SHOW_SET_TIME,
                SHOW_SET_DATE,
                SHOW_SET_CALIBER
        };

        static display_t *display_;
        static uint8_t show_;
        static uint8_t error_;
        static bool enable_auto_bright_;
        static bool enable_auto_rate_;
        static const rtc_tm *time_ptr_;
        static timer_id_t timer_update_, timer_hide_, timer_back_, timer_sensor_;
        static void mode(uint8_t _show);
        static void next_mode();
        static void update();
        static void hide();
private:
        static void back_show_time();
        static void check_sensor();
};

#endif /* SRC_SHOW_HPP_ */

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

        static void handle_key(const key_t _key);

        static void show_error(uint8_t _error);

protected:
        typedef enum {
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
        } show_t;

        static display_t *display_;
        static show_t show_;
        static uint8_t error_;
        static const rtc_tm *time_ptr_;
        static timer_id_t timer_update_, timer_hide_, timer_check_, timer_back_;

        static void mode(show_t _show);
        static void back_show_time();
        static void update();
        static void hide();
};

#endif /* SRC_SHOW_HPP_ */

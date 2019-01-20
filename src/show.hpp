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

#include "time/time.h"
#include "hal/display.h"
#include "hal/rtc.h"
#include "tms/tms.h"
#include "user.h"

class Show {
public:
        static void init(const stime *pt, const sdate *pd);

        static void synchronize();

        static void handle_key(const key_t _key);

        static void show_error(uint8_t _error);

        static void rtc_failture();

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

        static display_t *display;
        static show_t show;
        static uint8_t error;
        static const stime *time;
        static const sdate *date;
        static struct _flags {
                char rtc_power_fail :1;
        } flags;
        static timer_id_t timer_update, timer_hide, timer_check, timer_back;

        static void mode(show_t _show);
        static void back_show_time();
        static void update();
        static void hide();
};

#endif /* SRC_SHOW_HPP_ */

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

#include "bcd/bcd_time.h"
#include "hal/display.h"
#include "hal/rtc.h"
#include "tms/tms.h"
#include "user.h"

class Show {
public:
        static void init(const bcd_time_t *pt, const bcd_date_t *pd);

        static void synchronize();

        static void handle_key(const key_t _key);

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

        static show_t show;
        static rtc_error_t error;
        static const bcd_time_t *time;
        static const bcd_date_t *date;
        static struct _flags {
                char rtc_power_fail :1;
        } flags;
        static timer_id_t timer_update, timer_hide, timer_check, timer_back;

        static void mode(show_t _show);
        static void check_error();
        static void back_show_time();
        static void update();
        static void hide();
};

#endif /* SRC_SHOW_HPP_ */

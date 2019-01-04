/**
 * \file
 * \brief Режимы индикации и обработка нажатий кнопок
 * \details
 *
 * \date создан 13.11.2018
 * \author Nick Egorrov
 */

#include "config.h"
#include "show.h"
#include "tms/tms.h"
#include "hal/display.h"
#include "hal/rtc.h"

#define ERROR_CHECK_PERIOD      1200

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static show_t show = 0;
static rtc_error_t error;
static const bcd_time_t *time;
static const bcd_date_t *date;
static struct {
        char rtc_power_fail :1;
        char alarm_armed :1;
} flags = {
                0, 0
};
static timer_id_t timer_update, timer_hide, timer_check, timer_back;

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void check_error();

static void back_show_time();

static void update();

static void hide();

static void intro_update();

static void error_update();

static void time_update();

static void time_hide();

static void time_handle_key(key_t _key);

static void date_update();

static void date_handle_key(key_t _key);

/*************************************************************
 *      Public function
 *************************************************************/

extern void show_init(const bcd_time_t *pt, const bcd_date_t *pd)
{
        time = pt;
        date = pd;
        timer_hide = tms_create_timer(&hide);
        timer_update = tms_create_timer(&update);
        timer_check = tms_create_timer(&check_error);
        timer_back = tms_create_timer(&back_show_time);
        tms_start_timer(timer_check, _ticks_from_ms(ERROR_CHECK_PERIOD));
        show_set(SHOW_INTRO);
}

extern void show_set(show_t _show)
{
        show_t old = show;
        show = _show;

        if (old == show) {
                return;
        }

        /* Завершение показа старого режима. */
        switch (old) {
        case SHOW_INTRO:
                /* Завершение интро. Запуск таймера проверки состояния RTC. */
                tms_stop_timer(timer_update);
                break;
        default:
                ;
        }

        /* Подготовка перед показом нового режима.*/
        switch (show) {
        case SHOW_INTRO:
                tms_start_timer(timer_update,
                                _ticks_from_ms(CFG_SHOW_INTRO_PERIOD));
                tms_stop_timer(timer_back);
                break;
        case SHOW_TIME:
                tms_stop_timer(timer_back);
                break;
        case SHOW_ERROR:
                tms_start_timer(timer_back,
                                _ticks_from_ms(CFG_SHOW_DURATION_ERROR));
                break;
        case SHOW_DATE:
                tms_start_timer(timer_back,
                                _ticks_from_ms(CFG_SHOW_DURATION_DATE));
                break;
        default:
                ;
        }

        update();
}

extern void show_handle_key(const key_t _key)
{
        switch (show) {
        case SHOW_TIME:
                time_handle_key(_key);
                break;
        case SHOW_DATE:
                date_handle_key(_key);
                break;
        default:
                return;
        }
}

/**
 * Синхронизация времени с RTC
 */
extern void show_synchronize()
{
        tms_run_timer(timer_hide, _ticks_from_ms(CFG_SHOW_BLINK_DURATION));
        if (show != SHOW_DATE && show != SHOW_TIME) {
                return;
        }

        update();
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void check_error()
{
        error = rtc_error();

        if (error == RTC_NO_ERROR) {
                return;
        }

        rtc_clear();

        if (error == RTC_POWER_ERROR) {
                flags.rtc_power_fail = 1;
                return;
        }

        if (show == SHOW_INTRO) {
                return;
        }

        show_set(SHOW_ERROR);
        update();
}

static void back_show_time()
{
        show_set(SHOW_TIME);
}

static void update()
{
        display_clean();

        switch (show) {
        case SHOW_INTRO:
                intro_update();
                break;
        case SHOW_ERROR:
                error_update();
                break;
        case SHOW_DATE:
                date_update();
                break;
        default:
                time_update();
        }

        display_flush();
}

static void hide()
{
        switch (show) {
        case SHOW_TIME:
                time_hide();
                break;
        default:
                return;
        }

        display_flush();
}

/*************************************************************
 *      Show intro.
 *************************************************************/

static void intro_update()
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
                show_set(SHOW_DATE);
                return;
        }

        count = 0;
        phase++;
}

/*************************************************************
 *      Show errors.
 *************************************************************/

static void error_update()
{
        display_hours(_bcd2_make(06));
        display_seconds(0xa1);
}

/*************************************************************
 *      Show time.
 *************************************************************/

static void time_update()
{
        display_hours(time->hour);
        display_minutes(time->min);
        display_seconds(time->sec);
        display_day(date->day, flags.alarm_armed);
        display_dots(DISPLAY_DOT_BOTTOM);
}

static void time_hide()
{
        if (flags.rtc_power_fail == 0) {
                display_dots(DISPLAY_DOT_HIDE);
        } else {
                display_clean();
        }
}

static void time_handle_key(const key_t _key)
{
        switch (_key) {
        default:
                ;
        }
}

/*************************************************************
 *      Show date.
 *************************************************************/

static void date_update()
{
        display_hours(date->date);
        display_minutes(date->month);
        display_seconds(date->year);
        display_day(date->day, flags.alarm_armed);
        display_dots(DISPLAY_DOT_ALL);
}

static void date_handle_key(const key_t _key)
{
        switch (_key) {
        default:
                ;
        }
}

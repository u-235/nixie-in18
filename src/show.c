/**
 * \file
 * \brief Режимы индикации и обработка нажатий кнопок
 * \details
 *
 * \date создан 13.11.2018
 * \author Nick Egorrov
 */

#include "show.h"
#include "hal/display.h"
#include "hal/rtc.h"

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static show_t show;
static bcd_time_t time;
static bcd_date_t date;

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void update_intro();

static void update_time();

static void update_date();

/*************************************************************
 *      Public function
 *************************************************************/

extern void show_init()
{

}

extern void show_set(show_t _show)
{
        show_t old = show;
        show = _show;

        if (old != show) {
                show_update();
        }
}

extern void show_handle_key(key_t _key)
{

}

/**
 * Синхронизация времени с RTC
 */
extern void show_synchronize()
{
        rtc_get_time(&time);
        rtc_get_date(&date);
}

extern void show_update()
{
        switch (show) {
        case SHOW_INTRO:
                update_intro();
                break;
        case SHOW_DATE:
                update_date();
                break;
        default:
                update_time();
        }
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void update_intro()
{

}

static void update_time()
{

}

static void update_date()
{

}

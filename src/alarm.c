/**
 * @file
 * @brief Будильник
 * @details
 *
 * @date создан 04.01.2019
 * @author Nick Egorrov
 */

#include "config.h"
#include "hal/rtc.h"
#include "hal/mcu.h"
#include "tms/tms.h"
#include "alarm.h"

#define RTC_ADR_ALARM   0

typedef struct {
        alarm_t publ;
        char run :1;
        char hit :1;
        char jingle :1;
} ex_alarm_t;

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static ex_alarm_t alarm;
static timer_id_t timer_sound;

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void load();

static void save();

/*************************************************************
 *      Public function
 *************************************************************/

extern void alarm_init()
{
        timer_sound = tms_create_timer(alarm_off);
        tms_set_timer(timer_sound, _ticks_from_ms(CFG_ALARM_DURATION_SOUND));
        load();
}

extern void alarm_get(alarm_t *pa)
{
        pa->hours = alarm.publ.hours;
        pa->minutes = alarm.publ.minutes;
        pa->sound = alarm.publ.sound;
}

extern void alarm_set(alarm_t *pa)
{
        alarm.publ.hours = pa->hours;
        alarm.publ.minutes = pa->minutes;
        alarm.publ.sound = pa->sound;
        save();
}

extern char alarm_is_on()
{
        return alarm.run;
}

extern void alarm_on()
{
        alarm.run = 1;
        alarm.hit = 0;
        save();
}

extern void alarm_off()
{
        alarm.run = 0;
        alarm.hit = 0;
        alarm_stop();
        save();
}

extern char alarm_is_jingle()
{
        return alarm.jingle;
}

extern void alarm_start()
{
        alarm.jingle = 1;
        tms_run_timer(timer_sound);
        mcu_output_player(alarm.publ.sound);
}

extern void alarm_stop()
{
        alarm.jingle = 0;
        tms_stop_timer(timer_sound);
        mcu_output_player(0);
}

extern void alarm_check(const stime *pt)
{
        if (alarm.run == 0) {
                return;
        }

        if (alarm.publ.hours != pt->hours
                        || alarm.publ.minutes != pt->minutes) {
                alarm.hit = 0;
                return;
        }

        if (alarm.hit != 0) {
                return;
        }

        alarm.hit = 1;
        alarm_start();
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void load()
{
        rtc_mem_read(&alarm, RTC_ADR_ALARM, sizeof(ex_alarm_t));
        if (rtc_error() != RTC_NO_ERROR) {
                alarm.publ.hours = 7;
                alarm.publ.minutes = 40;
                alarm.publ.sound = 1;
                alarm.run = 0;
        }
        alarm.hit = 0;
        alarm.jingle = 0;
}

static void save()
{
        rtc_mem_write(&alarm, RTC_ADR_ALARM, sizeof(ex_alarm_t));
}

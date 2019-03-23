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

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static const struct rtc_tm *tm_ptr;
static struct {
        struct alarm_t publ;
        char run :1;
        char hit :1;
        char jingle :1;
} alarm;
static timer_id_t timer_sound;

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void load(void);

static void save(void);

/*************************************************************
 *      Public function
 *************************************************************/

void alarm_init(const struct rtc_tm *p_tm)
{
        tm_ptr = p_tm;
        timer_sound = tms_create_timer(alarm_off);
        tms_set_timer(timer_sound, _ticks_from_ms(CFG_ALARM_DURATION_SOUND));
        load();
}

void alarm_get(struct alarm_t *pa)
{
        pa->hours = alarm.publ.hours;
        pa->minutes = alarm.publ.minutes;
        pa->sound = alarm.publ.sound;
}

void alarm_set(struct alarm_t *pa)
{
        alarm.publ.hours = pa->hours;
        alarm.publ.minutes = pa->minutes;
        alarm.publ.sound = pa->sound;
        save();
}

char alarm_is_on()
{
        return alarm.run;
}

void alarm_on()
{
        alarm.run = 1;
        alarm.hit = 0;
        save();
}

void alarm_off()
{
        alarm.run = 0;
        alarm.hit = 0;
        alarm_stop();
        save();
}

char alarm_is_jingle()
{
        return alarm.jingle;
}

void alarm_start()
{
        alarm.jingle = 1;
        tms_run_timer(timer_sound);
        mcu_output_player(alarm.publ.sound);
}

void alarm_stop()
{
        alarm.jingle = 0;
        tms_stop_timer(timer_sound);
        mcu_output_player(0);
}

void alarm_check()
{
        if (alarm.run == 0) {
                return;
        }

        if (alarm.publ.hours != tm_ptr->hours
                        || alarm.publ.minutes != tm_ptr->minutes) {
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

void load()
{
        rtc_mem_read(&alarm, RTC_ADR_ALARM, sizeof(alarm));
        if (rtc_error() != RTC_NO_ERROR) {
                alarm.publ.hours = 7;
                alarm.publ.minutes = 40;
                alarm.publ.sound = 1;
                alarm.run = 0;
        }
        alarm.hit = 0;
        alarm.jingle = 0;
}

void save()
{
        rtc_mem_write(&alarm, RTC_ADR_ALARM, sizeof(alarm));
}

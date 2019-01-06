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
        load();
}

extern void alarm_get(alarm_t *pa)
{
        pa->hour = alarm.publ.hour;
        pa->min = alarm.publ.min;
        pa->sound = alarm.publ.sound;
}

extern void alarm_set(alarm_t *pa)
{
        alarm.publ.hour = pa->hour;
        alarm.publ.min = pa->min;
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

extern char alarm_is_jingle(){
        return alarm.jingle;
}

extern void alarm_start()
{
        mcu_output_player(alarm.publ.sound);
}

extern void alarm_stop()
{
        alarm.jingle = 0;
        mcu_output_player(0);
}

extern void alarm_check(const bcd_time_t *pt)
{
        if (alarm.run == 0) {
                return;
        }

        if (alarm.publ.hour != pt->hour || alarm.publ.min != pt->min) {
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
        rtc_mem_read((int8_t*) &alarm, RTC_ADR_ALARM, sizeof(ex_alarm_t));
        alarm.hit = 0;
        alarm.jingle = 0;
}

static void save()
{
        rtc_mem_write((int8_t*) &alarm, RTC_ADR_ALARM, sizeof(ex_alarm_t));
}

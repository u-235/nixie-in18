/*
 * tms.c
 * Timers & Messages Service
 *
 *  Created on: 16.04.2016
 *      Author: Николай
 */

#include "tms.h"

typedef struct {
        char booked :1; /* Флаг занятости. */
        char runing :1;
        char single :1;
        timer_counter_t top;
        timer_counter_t count;
        void (*timer_callback)();
} timer_t;

static timer_t lTimer[TIMER_NUMBER];

extern void tms_init()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                lTimer[i].booked = 0;
                lTimer[i].runing = 0;
        }
}

extern timer_id_t tms_create_timer(void (*timer_callback)())
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].booked != 0) continue;
                lTimer[i].timer_callback = timer_callback;
                lTimer[i].booked = 1;
                return i;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_delete_timer(timer_id_t timer_id)
{
        if (timer_id < TIMER_NUMBER) {
                lTimer[timer_id].booked = 0;
                lTimer[timer_id].runing = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_start_timer(timer_id_t timer_id, timer_counter_t ticks)
{
        if ((timer_id < TIMER_NUMBER) && (lTimer[timer_id].booked != 0)) {
                lTimer[timer_id].single = 0;
                lTimer[timer_id].runing = 1;
                lTimer[timer_id].top = ticks;
                lTimer[timer_id].count = lTimer[timer_id].top;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_run_timer(timer_id_t timer_id, timer_counter_t ticks)
{
        if ((timer_id < TIMER_NUMBER) && (lTimer[timer_id].booked != 0)) {
                lTimer[timer_id].single = 1;
                lTimer[timer_id].runing = 1;
                lTimer[timer_id].top = ticks;
                lTimer[timer_id].count = lTimer[timer_id].top;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_stop_timer(timer_id_t timer_id)
{
        if ((timer_id < TIMER_NUMBER) && (lTimer[timer_id].booked != 0)) {
                lTimer[timer_id].runing = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern void tms_tick()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].runing == 0) {
                        continue;
                }

                lTimer[i].count--;
                if (lTimer[i].count == 0) {
                        if (lTimer[i].single == 0) {
                                lTimer[i].count = lTimer[i].top;
                        } else {
                                lTimer[i].runing = 0;
                        }
                        lTimer[i].timer_callback();
                }
        }
        return;
}

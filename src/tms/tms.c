/*
 * tms.c
 * Timers & Messages Service
 *
 *  Created on: 16.04.2016
 *      Author: Николай
 */

#include "tms.h"

typedef struct {
        timer_counter_t count;
        void (*timer_callback)();
} timer_t;

static timer_t lTimer[TIMER_NUMBER];

extern void tms_init()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                lTimer[i].timer_callback = 0;
                lTimer[i].count = 0;
        }
}

extern timer_id_t tms_create_timer(void (*timer_callback)())
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].timer_callback != 0) continue;
                lTimer[i].timer_callback = timer_callback;
                return i;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_delete_timer(timer_id_t timer_id)
{
        if (timer_id < TIMER_NUMBER) {
                lTimer[timer_id].count = 0;
                lTimer[timer_id].timer_callback = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_start_timer(timer_id_t timer_id, timer_counter_t ticks)
{
        if ((timer_id < TIMER_NUMBER)
                        && (lTimer[timer_id].timer_callback != 0)) {
                lTimer[timer_id].count = ticks;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern timer_id_t tms_stop_timer(timer_id_t timer_id)
{
        if ((timer_id < TIMER_NUMBER)
                        && (lTimer[timer_id].timer_callback != 0)) {
                lTimer[timer_id].count = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

extern void tms_tick()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].count != 0) {
                        lTimer[i].count--;
                        if (lTimer[i].count == 0) {
                                lTimer[i].timer_callback();
                        }
                }
        }
        return;
}


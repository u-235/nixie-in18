/*
 * tms.c
 * Timers & Messages Service
 *
 *  Created on: 16.04.2016
 *      Author: Николай
 */

#include "tms.h"

static struct {
        char booked :1; /* Флаг занятости. */
        char runing :1;
        char single :1;
        timer_counter_t top;
        timer_counter_t count;
        void (*timer_callback)(void);
} timers[TIMER_NUMBER];

/**
 * Запуск таймера.
 * \param timer_id Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \param single 0 для повторяющегося запуска, 1 - для однократного.
 * \return Идентификатор запущенного таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 */
static timer_id_t tms_launch_timer(timer_id_t timer_id, char single);

void tms_init()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                timers[i].booked = 0;
                timers[i].runing = 0;
        }
}

timer_id_t tms_create_timer(void (*timer_callback)(void))
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (timers[i].booked != 0) continue;
                timers[i].timer_callback = timer_callback;
                timers[i].booked = 1;
                return i;
        }
        return TIMER_ERROR;
}

timer_id_t tms_delete_timer(timer_id_t timer_id)
{
        if (timer_id < TIMER_NUMBER) {
                timers[timer_id].booked = 0;
                timers[timer_id].runing = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

timer_id_t tms_set_timer(timer_id_t timer_id, timer_counter_t ticks)
{
        if ((timer_id < TIMER_NUMBER) && (timers[timer_id].booked != 0)) {
                timers[timer_id].top = ticks;
                return timer_id;
        }
        return TIMER_ERROR;
}

timer_id_t tms_start_timer(timer_id_t timer_id)
{
        return tms_launch_timer(timer_id, 0);
}

timer_id_t tms_run_timer(timer_id_t timer_id)
{
        return tms_launch_timer(timer_id, 1);
}

timer_id_t tms_stop_timer(timer_id_t timer_id)
{
        if ((timer_id < TIMER_NUMBER) && (timers[timer_id].booked != 0)) {
                timers[timer_id].runing = 0;
                return timer_id;
        }
        return TIMER_ERROR;
}

void tms_tick()
{
        timer_id_t i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (timers[i].runing == 0) {
                        continue;
                }

                timers[i].count--;
                if (timers[i].count == 0) {
                        if (timers[i].single == 0) {
                                timers[i].count = timers[i].top;
                        } else {
                                timers[i].runing = 0;
                        }
                        timers[i].timer_callback();
                }
        }
        return;
}

timer_id_t tms_launch_timer(timer_id_t timer_id, char single)
{
        if ((timer_id < TIMER_NUMBER) && (timers[timer_id].booked != 0)) {
                timers[timer_id].single = single;
                timers[timer_id].runing = 1;
                timers[timer_id].count = timers[timer_id].top;
                return timer_id;
        }
        return TIMER_ERROR;
}

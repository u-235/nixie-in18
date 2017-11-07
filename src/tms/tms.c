/*
 * tms.c
 * Timers & Messages Serves
 *
 *  Created on: 16.04.2016
 *      Author: Николай
 */
#define _TMS_INSIDE_ 1
#include "tms.h"

typedef struct _msg {
        tms_handler_t idTask;
        tms_msg_t idMessage;
        tms_prm_t wParam;
        struct _msg *next;
} message_t;

typedef struct {
        unsigned int set;
        unsigned int count;
        tms_prm_t wParam;
        tms_handler_t task;
        unsigned char flags;
} timer_t;

#define TASK_FREE 0xFF

#define TF_ACTIVE       0b00100000
#define TF_AT_ONCE      0b01000000
#define TF_NO_EMPTY     0b10000000

static message_t msg[QUEUE_SIZE];
static message_t * first;

static timer_t lTimer[TIMER_NUMBER];

void tms_init()
{
        tms_clear();
        tms_post_all(MSG_INIT, 0);
        return;
}

/*
 * Очистка очереди сообщений.
 */
void tms_clear()
{
        tms_handler_t t;

        first = msg;
        for (t = 0; t < (sizeof(call_backs) / sizeof(tms_msg_handler)); t++) {
                first->next = 0;
                first->idTask = TASK_FREE;
                first++;
        }
        first = 0;
}

/*
 * Разгрузка очереди сообщений. Очередное сообщение передается и удаляется
 * из очереди. Один вызов этой функции обрабатывает одно сообщение.
 */
extern unsigned char tms_dispatch()
{
        tms_handler_t t, m;
        tms_prm_t p;
        message_t *old;

        if (first) {
                mcu_interrupt_lock();

                t = first->idTask;
                m = first->idMessage;
                p = first->wParam;
                old = first;
                first = first->next;
                old->next = 0;
                old->idTask = TASK_FREE;

                mcu_interrupt_unlock();
                call_backs[t](m, p);

                return 1;
        }

        return 0;
}

/* Функция постановки сообщения в очередь службы. Глубина очереди задаётся
 * макросом QUEUE_SIZE в файле custom.h.
 * Параметры
 *  handler_id - идентификатор обработчика сообщений, попросту - индекс в массиве
 *      call_backs, определенном в файле custom.h.
 *  idMessage - идентификатор сообщения, значения 0-15 закреплены за службой.
 *  mParam - параметр, передаваемый с сообщением.
 */
void tms_post(tms_handler_t handler_id, tms_msg_t idMessage, tms_prm_t wParam)
{
        unsigned char i;
        message_t *free, *prev, *curr;

        mcu_interrupt_lock();

        curr = msg;
        for (i = 0; i < (sizeof(call_backs) / sizeof(tms_msg_handler)); i++) {
                if (curr->idTask == TASK_FREE) {
                        free = curr;
                        break;
                }
                curr++;
        }

        free->idTask = handler_id;
        free->idMessage = idMessage;
        free->wParam = wParam;

        prev = 0;
        curr = first;

        while (curr) {
                prev = curr;
                curr = curr->next;
        }

        if (prev == 0) {
                first = free;
        } else {
                prev->next = free;
        }

        free->next = curr;

        mcu_interrupt_unlock();
        return;
}

/* Функция постановки сообщения в очередь службы. Глубина очереди задаётся
 * макросом QUEUE_SIZE в файле custom.h.
 * Параметры
 *  idMessage - идентификатор сообщения, значения 0-15 закреплены за службой.
 *  mParam - параметр, передаваемый с сообщением.
 */
void tms_post_all(tms_msg_t idMessage, tms_prm_t wParam)
{
        tms_handler_t t;

        for (t = 0; t < (sizeof(call_backs) / sizeof(tms_msg_handler)); t++) {
                tms_post(t, idMessage, 0);
        }
        return;
}

unsigned char tms_create_timer(tms_handler_t task, unsigned int tics)
{
        unsigned char i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].flags & TF_NO_EMPTY) continue;
                lTimer[i].flags = TF_NO_EMPTY;
                lTimer[i].count = 0;
                lTimer[i].set = tics;
                i++;
                return i;
        }
        return 0;
}

unsigned char tms_delete_timer(unsigned char idTimer)
{
        if (idTimer > TIMER_NUMBER) return 0;
        lTimer[idTimer - 1].flags = 0;
        return idTimer;
}

unsigned char tms_start_timer(unsigned char idTimer, tms_prm_t wParam)
{
        if ((idTimer <= TIMER_NUMBER)
                        && (lTimer[idTimer - 1].flags & TF_NO_EMPTY)) {
                lTimer[idTimer - 1].flags = TF_NO_EMPTY | TF_ACTIVE;
                lTimer[idTimer - 1].wParam = wParam;
                lTimer[idTimer].count = 0;
                return idTimer;
        }
        return 0;
}

unsigned char tms_start_timer_once(unsigned char idTimer, tms_prm_t wParam)
{
        if ((idTimer <= TIMER_NUMBER)
                        && (lTimer[idTimer - 1].flags & TF_NO_EMPTY)) {
                lTimer[idTimer - 1].flags =
                                TF_NO_EMPTY | TF_ACTIVE | TF_AT_ONCE;
                lTimer[idTimer].count = 0;
                return idTimer;
        }
        return 0;
}

unsigned char tms_stop_timer(unsigned char idTimer)
{
        if ((idTimer <= TIMER_NUMBER)
                        && (lTimer[idTimer - 1].flags & TF_NO_EMPTY)) {
                lTimer[idTimer - 1].flags = TF_NO_EMPTY;
                lTimer[idTimer].count = 0;
                return idTimer;
        }
        return 0;
}

void tms_tick()
{
        unsigned char i;

        for (i = 0; i < TIMER_NUMBER; i++) {
                if (lTimer[i].flags & TF_ACTIVE) {
                        lTimer[i].count++;
                        if (lTimer[i].count == lTimer[i].set) {
                                lTimer[i].count = 0;
                                if (lTimer[i].flags & TF_AT_ONCE) {
                                        lTimer[i].flags = TF_NO_EMPTY;
                                }
                                tms_post(lTimer[i].task, MSG_TIMER,
                                                lTimer[i].wParam);
                        }
                }
        }
        return;
}


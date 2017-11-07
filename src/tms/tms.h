/*
 * tms.h
 * Зависимости и настройки для
 * Timers & Messages Serves
 *
 *  Created on: 16.04.2016
 *      Author: Nick
 */

#ifndef TMS_H_
#define TMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TMS_INSIDE_
        extern void mcu_interrupt_lock();
        extern void mcu_interrupt_unlock();
#endif

        /* Идентификатор сообщения */
        typedef unsigned char tms_msg_t;
        /* Идентификатор задачи */
        typedef unsigned char tms_handler_t;
        /* Параметр сообщения */
        typedef unsigned int tms_prm_t;

        extern void tms_init();

        extern void tms_clear();

        extern unsigned char tms_dispatch();

        extern void tms_tick();

        extern void tms_post(tms_handler_t handler_id, tms_msg_t idMessage,
                        tms_prm_t mParam);
        extern void tms_post_all(tms_msg_t idMessage, tms_prm_t mParam);

        /*===================================
         *  Предопределённые сообщения.
         *===================================*/

        /* Сообщение при запуске системы
         * idMessage = MSG_INIT
         * mParam = идентификатор обработчика */
#define MSG_INIT    0
        /* При срабатывании таймера */
#define MSG_TIMER   1

#define MSG_TMS_LAST 1

        extern unsigned char tms_create_timer(tms_handler_t task,
                        unsigned int tics);

        extern unsigned char tms_delete_timer(unsigned char idTimer);

        extern unsigned char tms_start_timer(unsigned char idTimer,
                        tms_prm_t wParam);

        extern unsigned char tms_start_timer_once(unsigned char idTimer,
                        tms_prm_t wParam);

        extern unsigned char tms_stop_timer(unsigned char idTimer);

        /* Прототип функции задачи, вызываемой
         * диспечером задач. Список функций всех
         * задач должен быть определён в файле custom.h
         *
         * Параметры
         *   idMessage - идентификатор сообщения.
         *   wParam - дополнительный параметр.
         **************************************/
        typedef void (*tms_msg_handler)(tms_msg_t idMessage, tms_prm_t wParam);

#ifdef __cplusplus
}
#endif

#include "custom.h"

#endif /* TMS_H_ */

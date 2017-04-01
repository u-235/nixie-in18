/*
 * Пользовательские настройки службы таймеров и сообщений.
 */

#ifndef CUSTOM_H_
#define CUSTOM_H_

#ifdef _TMS_INSIDE_

/* Максимальное количество сообщений в очереди службы. */
#define QUEUE_SIZE  8
/* Количество таймеров. */
#define TIMER_NUMBER    3

/* Объявление функций, вызываемых службой для передачи сообщений.
 * Подробнее о этих функциях смотрите в tms.h
 */
extern void main_handler(tms_msg_t idMessage, tms_prm_t wParam);

/* Массив указателей на функции, объявленные выше. */
static const tms_msg_handler call_backs[] = {
        main_handler
};
#endif // _TMS_INSIDE_

#define _tms_ms(m) (m/10)

#define MSG_SLEEP       (MSG_TMS_LAST+1)
#define MSG_WAKEUP      (MSG_TMS_LAST+2)
#define MSG_COMMON_LAST (MSG_TMS_LAST+2)

#endif // CUSTOM_H_

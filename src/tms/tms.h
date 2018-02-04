/**
 * \file tms.h
 * \brief Служба таймеров.
 * \details Служба предоставляет простой способ для организации множества
 * таймеров.
 *
 * \date created 16.04.2016
 * \author Nick Egorrov
 */

#ifndef TMS_H_
#define TMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Тип идентификатора таймера.
 */
typedef unsigned char timer_id_t;
/**
 * Тип счётчика таймера.
 */
typedef unsigned int timer_counter_t;
/**
 * Тип параметра, передаваемого функции обратного вызова таймера.
 */
typedef void* timer_param_t;
/**
 * Значение, возвращаемое в случае ошибки.
 */
#define TIMER_ERROR -1

/**
 * Инициализация сервиса.
 */
extern void tms_init();

/**
 * Обновление внутреннего состояния сервиса. Счётчики таймеров
 * уменьшаются и, при достижении нуля, происходит вызов функции
 * timer_callback, указанной при создании таймера.
 * \see tms_create_timer() tms_start_timer()
 */
extern void tms_tick();

/**
 * Создание таймера. По окончаниии использования таймера нужно
 * освободить его вызовом функции tms_delete_timer().
 * \param timer_callback Функция, вызываемая при срабатывании
 *      таймера.
 * \param param Значение, передаваемое функции timer_callback() при
 *      её вызове.
 * \return Идентификатор таймера в случае успеха или #TIMER_ERROR
 *      если израсходована вся память для организации таймеров.
 */
extern timer_id_t tms_create_timer(void (*timer_callback)(timer_param_t param),
                timer_param_t param);

/**
 * Освобождение таймера.
 * \param timer Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \return Идентификатор освобождённого таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 */
extern timer_id_t tms_delete_timer(timer_id_t timer);

/**
 * Запуск таймера. Во внутренний счётчик записывается ticks.
 * \param timer Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \param ticks Количество вызовов функции tms_tick(), необходимое для
 *      срабатывания таймера.
 * \return Идентификатор запущенного таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 */
extern timer_id_t tms_start_timer(timer_id_t timer, timer_counter_t ticks);

/**
 * Останов таймера. Внутренний счётчик сбрасывается и таймер
 * останавливается без вызова функции timer_callback().
 * \param timer Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \return Идентификатор остановленного таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 */
extern timer_id_t tms_stop_timer(timer_id_t timer);

#ifdef __cplusplus
}
#endif

/**
 * Число таймеров, которое может быть использовано одновременно.
 */
#define TIMER_NUMBER    3

#endif /* TMS_H_ */

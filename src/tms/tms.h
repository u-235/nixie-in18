/**
 * \file
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
 * Значение, возвращаемое в случае ошибки.
 */
#define TIMER_ERROR -1

/**
 * Инициализация сервиса.
 */
extern void tms_init(void);

/**
 * Обновление внутреннего состояния сервиса. Счётчики таймеров
 * уменьшаются и, при достижении нуля, происходит вызов функции
 * timer_callback, указанной при создании таймера.
 * \see tms_create_timer() tms_start_timer()
 */
extern void tms_tick(void);

/**
 * Создание таймера. По окончаниии использования таймера нужно
 * освободить его вызовом функции tms_delete_timer().
 * \param timer_callback Функция, вызываемая при срабатывании
 *      таймера.
 * \return Идентификатор таймера в случае успеха или #TIMER_ERROR
 *      если израсходована вся память для организации таймеров.
 */
extern timer_id_t tms_create_timer(void (*timer_callback)(void));

/**
 * Освобождение таймера.
 * \param timer Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \return Идентификатор освобождённого таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 */
extern timer_id_t tms_delete_timer(timer_id_t timer);

/**
 * Загрузка периода таймера.
 * \param timer_id Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \param ticks Количество вызовов функции tms_tick(), необходимое для
 *      срабатывания таймера. Используйте макрос #_ticks_from_ms для
 *      преобразования из миллисекунд.
 * \return Идентификатор таймера в случае успеха или #TIMER_ERROR если был
 *      использован недопустимый идентификатор.
 */
extern timer_id_t tms_set_timer(timer_id_t timer_id, timer_counter_t ticks);

/**
 * Повторяющийся запуск таймера.
 * \param timer_id Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \return Идентификатор запущенного таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 * \see tms_set_timer()
 */
extern timer_id_t tms_start_timer(timer_id_t timer_id);

/**
 * Однократный запуск таймера.
 * \param timer_id Идентификатор таймера, полученный при вызове функции
 *      tms_create_timer().
 * \return Идентификатор запущенного таймера в случае успеха или
 *      #TIMER_ERROR если был использован недопустимый идентификатор.
 * \see tms_set_timer()
 */
extern timer_id_t tms_run_timer(timer_id_t timer_id);

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
 * Получение количества вызовов функции tms_tick() из времени в миллисекундах.
 */
#ifdef __cplusplus
#define _ticks_from_ms(p)\
                static_cast<timer_counter_t>(\
                        static_cast<uint32_t>(p) * TIMER_FREQUENCY / 1000UL\
                        )
#else
#define _ticks_from_ms(p)\
        ((timer_counter_t)(\
                        ((uint32_t) p) * TIMER_FREQUENCY / 1000UL)\
                        )
#endif

/*************************************************************
 *      Настройки службы таймеров.
 *************************************************************/

/**
 * Число таймеров, которое может быть использовано одновременно.
 */
#define TIMER_NUMBER    7

/**
 * Частота, с которой происходит вызов функции tms_tick().
 */
#define TIMER_FREQUENCY 100u

#endif /* TMS_H_ */

/**
 * \file events.h
 * \brief Определение асинхронных событий.
 * \details
 *
 * \date created on: 04.01.2018
 * \author: Nick Egorrov
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#ifdef _cplusplus
extern "C" {
#endif

/**
 * \brief Общие события.
 * \details
 */
typedef enum {
        EVENT_START,            ///< запуск системы
        EVENT_INTRO_END,        ///< показ приветствия завершён
        EVENT_ALARM,            ///< сработал будильник
        EVENT_ERROR             ///< произошла ошибка
} event_t;

/**
 * \brief Нажатие кнопок.
 * \details Возможно состояние с несколькими нажатыми кнопками.
 */
typedef enum {
        KEY_MODE = 1 << 0,      ///< кнопка "режим"
        KEY_ALARM = 1 << 1,     ///< кнопка "будильник"
        KEY_CHANGE = 1 << 2     ///< кнопка "изменить"
} event_key_t;

/**
 * \brief Обработчик общиих событий.
 * \details
 * \param event событие.
 */
extern void event_handler(event_t event);

/**
 * \brief Обработчик нажатий кнопок.
 * \details
 * \param key состоние кнопок.
 */
extern void event_key_handler(event_key_t key);

#ifdef _cplusplus
extern "C" {
#endif

#endif /* EVENTS_H_ */

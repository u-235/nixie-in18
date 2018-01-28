/***************************************************************************//**
 \file rtc.h
 \defgroup RTC <rtc.h> Модуль RTC

 \date создан: 26 окт. 2015 г.
 \author Nick Egorrov
 @{
 ******************************************************************************/

#ifndef HAL_RTC_H_
#define HAL_RTC_H_

#include "../bcd/bcd_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Перечисление ошибок RTC.
 */
typedef enum {
        RTC_NO_ERROR,           ///< Нет ошибок
        RTC_TIMEOUT,            ///< Нет ответа от микросхемы RTC.
        RTC_INVALID_TIME,       ///<
        RTC_INVALID_DATE,       ///<
        RTC_UNCNOWN_ERROR       ///<
} rtc_error_t;

/**
 * \brief Инициализация RTC.
 * \details
 * \param update Указатель на функцию реакции на обновление счётчиков RTC. Эта функция
 *      может быть вызвана в результате работы rtc_check().
 * \param alarm Указатель на функцию обработки срабатывания будильника.
 * \param error Указатель на функцию обработки ошибок.
 */
extern void rtc_init(void (*update)(), void (*alarm)(unsigned char alarm_id),
                void (*error)(rtc_error_t error));

extern rtc_error_t rtc_error();

extern void rtc_clear();

/**
 * \brief Проверка обновления RTC.
 * \details Функция проверяет, было ли обновлено состояние счётчиков RTC.
 */
extern void rtc_check();

extern rtc_error_t rtc_get_time(bcd_time_t * time);

extern rtc_error_t rtc_set_time(bcd_time_t * time);

extern rtc_error_t rtc_get_date(bcd_date_t * date);

extern rtc_error_t rtc_set_date(bcd_date_t * date);

#ifdef __cplusplus
}
#endif

/** @} */

#include "rtc/chips.h"

#endif /* HAL_RTC_H_ */

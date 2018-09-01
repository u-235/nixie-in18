/***************************************************************************//**
 \file rtc.h
 \defgroup RTC <rtc.h> Модуль RTC

 \date создан: 26 окт. 2015 г.
 \author Nick Egorrov
 @{
 ******************************************************************************/

#ifndef HAL_RTC_H_
#define HAL_RTC_H_

/**
 * Перечисление ошибок RTC.
 */
typedef enum {
        RTC_NO_ERROR,  ///< Нет ошибок
        RTC_UNCNOWN_ERROR,  ///<
        RTC_TIMEOUT,  ///< Нет ответа от микросхемы RTC.
        RTC_INVALID_TIME,  ///<
        RTC_INVALID_DATE  ///<
} rtc_error_t;

#include "../bcd/bcd_time.h"

#ifdef __RTC_DC1307__
#       include "rtc/chip_dc1307.h"
#elif defined __RTC_M41T56__
#       include "rtc/chip_m41t56.h"
#else
#       warning "rtc chip not defined"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Инициализация RTC.
 * \details
 */
extern void rtc_init();

extern rtc_error_t rtc_error();

extern void rtc_clear();

/**
 * \brief Проверка обновления RTC.
 * \details Функция проверяет, было ли обновлено состояние счётчиков RTC.
 * \return Ноль есди обновления не было.
 */
extern uint8_t rtc_check();

extern rtc_error_t rtc_get_time(bcd_time_t * time);

extern rtc_error_t rtc_set_time(bcd_time_t * time);

extern rtc_error_t rtc_get_date(bcd_date_t * date);

extern rtc_error_t rtc_set_date(bcd_date_t * date);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* HAL_RTC_H_ */

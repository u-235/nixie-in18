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
        /* Нет ошибок. */
        RTC_NO_ERROR,
        /*  */
        RTC_UNKNOWN_ERROR,
        /* Функция не реализована. */
        RTC_NOT_IMPLEMENTED,
        /* Ошибка микросхемы RTC */
        RTC_CHIP_ERROR
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
extern uint8_t rtc_init();

extern rtc_error_t rtc_error();

extern void rtc_clear();

extern void rtc_start();

extern void rtc_stop();

/**
 * \brief Проверка обновления RTC.
 * \details Функция проверяет, было ли обновлено состояние счётчиков RTC.
 * \return Ноль если обновления не было.
 */
extern uint8_t rtc_check();

extern void rtc_get_time(bcd_time_t * time);

extern void rtc_set_time(const bcd_time_t * time);

extern void rtc_get_date(bcd_date_t * date);

extern void rtc_set_date(const bcd_date_t * date);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* HAL_RTC_H_ */

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
        /** Нет ошибок. */
        RTC_NO_ERROR,
        /**  */
        RTC_UNKNOWN_ERROR,
        /** Функция не реализована. */
        RTC_NOT_IMPLEMENTED,
        /** Ошибка микросхемы RTC */
        RTC_CHIP_ERROR,
        /** Сбой питания микросхемы RTC */
        RTC_POWER_ERROR
} rtc_error_t;

#include "../time/time.h"

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

extern void rtc_start();

extern void rtc_stop();

/**
 * \brief Проверка обновления RTC.
 * \details Функция проверяет, было ли обновлено состояние счётчиков RTC.
 * \return Ноль если обновления не было.
 */
extern uint8_t rtc_check();

extern void rtc_get_time(stime * time);

extern void rtc_set_time(const stime * time);

extern void rtc_get_date(sdate * date);

extern void rtc_set_date(const sdate * date);

/**
 * Чтение массива данных из RTC.
 * @param dst Указатель на буфер для приёма данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для чтения.
 */
extern void rtc_mem_read(void *dst, const uint8_t adr, const uint8_t sz);

/**
 * Запись массива данных в RTC.
 * @param src Указатель на буфер для передачи данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для записи.
 */
extern void rtc_mem_write(const void *src, const uint8_t adr, const uint8_t sz);

/**
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
extern int8_t rtc_get_caliber();

/**
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного.
 * @param clb Коррекция хода в ppm.
 */
extern void rtc_set_caliber(int8_t clb);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* HAL_RTC_H_ */

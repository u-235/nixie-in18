/**
 * @file bcd_time.h
 *
 * @date Created on: 16.12.2017
 * @author Nick Egorrov
 */

#ifndef SRC_BCD_BCD_TIME_H_
#define SRC_BCD_BCD_TIME_H_

#include "bcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Перечисление дней недели.
 */
typedef enum {
        /** Понедельник */
        MONDAY = 1,
        /** Вторник */
        TUESDAY,
        /** Среда */
        WEDNESDAY,
        /** Четверг */
        THURSDAY,
        /** Пятница */
        FRIDAY,
        /** Суббота */
        SATURDAY,
        /** Воскресенье */
        SUNDAY
} bcd_day_t;

/**
 * Структура для представления времени суток в двоично-десятичном
 * формате.
 */
typedef struct {
        /** Часы, 0-23 */
        bcd2_t hour;
        /** Минуты, 0-59 */
        bcd2_t min;
        /** Секунды, 0-59 */
        bcd2_t sec;
} bcd_time_t;

/**
 * Структура для представления календарной даты в двоично-десятичном
 * формате.
 */
typedef struct {
        /** Год, 0-99 */
        bcd2_t year;
        /** Месяц, 1-12 */
        bcd2_t month;
        /** День месяца, 1-31 */
        bcd2_t date;
        /** День недели. ::bcd_day_t */
        bcd_day_t day;
} bcd_date_t;

/**
 * Сравнение двух времён суток. Функция не проверяет корректность полей
 * структур.
 * @param time1 первое время.
 * @param time2 второе время.
 * @return 0 если времена равны, отрицательное значение если time1
 *      раньше time2 и положительное значение если time1 позже.
 */
extern int bcd_time_cmp(bcd_time_t *time1, bcd_time_t *time2);

/**
 * Сравнение двух дат. Дни недели в сравнении не участвуют. Функция не
 * проверяет корректность полей структур.
 * @param date1 первая дата.
 * @param date2 вторая дата.
 * @return 0 если даты равны, отрицательное значение если date1 раньше
 *      date2 и положительное значение если date1 позже.
 */
extern int bcd_date_cmp(bcd_date_t *date1, bcd_date_t *date2);

#ifdef __cplusplus
}
#endif

#endif /* SRC_BCD_BCD_TIME_H_ */

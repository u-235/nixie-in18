/**
 * @file
 * @brief
 * @details
 *
 * @date создан 11.01.2019
 * @author Nick Egorrov
 */

#ifndef TIME_TIME_H_
#define TIME_TIME_H_

#include <stdint.h>

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
} day_t;

/**
 * Структура для представления времени суток в двоичном формате.
 */
typedef struct {
        /** Часы, 0-23 */
        uint8_t hours;
        /** Минуты, 0-59 */
        uint8_t minutes;
        /** Секунды, 0-59 */
        uint8_t seconds;
} time_t;

/**
 * Структура для представления календарной даты в двоичном формате.
 */
typedef struct {
        /** Год, 0-99 */
        uint8_t year;
        /** Месяц, 1-12 */
        uint8_t month;
        /** День месяца, 1-31 */
        uint8_t day;
        /** День недели. ::day_t */
        uint8_t week_day;
} date_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void date_adjust(date_t *date);

extern uint32_t days_from_millenium(const date_t *date);

extern uint32_t seconds_from_midnight(const time_t *time);

extern uint32_t seconds_from_millenium(const date_t *date, const time_t *time);

#ifdef __cplusplus
}
#endif

#endif /* TIME_TIME_H_ */

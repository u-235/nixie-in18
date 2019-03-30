/**
 * @file
 * @brief
 * @details
 *
 * @date создан 13.01.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <string.h>
#include "../rtc.h"

void rtc_copy(struct rtc_tm *dst, struct rtc_tm const *src)
{
        memcpy(dst, src, sizeof(struct rtc_tm));
}

#ifdef CFG_RTC_USE_CTIME_H
#include <time.h>

/**
 * Конвертирует структуру rtc_tm в стандартную tm.
 * @note Эта функция доступна только если определён макрос CFG_RTC_USE_CTIME_H
 *      в файле <config.h>
 * @param dst Указатель на заполняемую структуру.
 * @param src Указатель на исходную структуру.
 */
void rtc_get_tm(struct tm *dst, struct rtc_tm const *src)
{
        dst->tm_sec = src->seconds;
        dst->tm_min = src->minutes;
        dst->tm_hour = src->hours;
        dst->tm_wday = src->week_day;
        dst->tm_mday = src->day;
        dst->tm_mon = src->month - 1;
        dst->tm_year = src->year + 100;
        dst->tm_yday = rtc_get_day_of_year(src);
}

/**
 * Конвертирует стандартную структуру tm в rtc_tm.
 * @note Эта функция доступна только если определён макрос CFG_RTC_USE_CTIME_H
 *      в файле <config.h>
 * @param dst Указатель на заполняемую структуру.
 * @param src Указатель на исходную структуру.
 */
void rtc_set_tm(struct rtc_tm *dst, struct tm const *src)
{
        dst->seconds = src->tm_sec;
        dst->minutes = src->tm_min;
        dst->hours = src->tm_hour;
        dst->week_day = src->tm_wday;
        dst->day = src->tm_mday;
        dst->month = src->tm_mon + 1;
        dst->year = src->tm_year - 100;
}

#endif /* CFG_RTC_USE_CTIME_H */

/*
 * @brief Нормализация времени.
 * @details Поля _time->seconds, _time->minutes и _time->hours обнуляются при
 *      превышении максимального значения.
 *
 *      Если параметр _how не равен нулю, то при обнулении секунд инкриминируются
 *      минуты; при обнулении минут инкриминируются часы и при обнулении часов
 *      функция возвращает положительное значение.
 *
 * @param _time Указатель на структуру для нормализации времени.
 * @param _how Ноль если при переполнении не нужно делать переноса в следующее
 *      поле.
 *
 * @return Если параметр _how равен нулю, то возвращается всегда ноль.
 *      Если параметр _how не равен нулю, то при переполнении поля _time->hours
 *      возвращается положительное значение; если переполнения не было, то
 *      возвращается отрицательное значение.
 */
int8_t rtc_validate_time(struct rtc_tm *_time, int8_t _how)
{
        uint8_t h, m, s;
        int8_t result;

        result = _how != 0 ? -1 : 0;
        h = _time->hours;
        m = _time->minutes;
        s = _time->seconds;

        if (s >= 60) {
                s = 0;
                if (_how != 0)
                        m++;
        }

        if (m >= 60) {
                m = 0;
                if (_how != 0)
                        h++;
        }

        if (h >= 24) {
                h = 0;
                if (_how != 0)
                        result = 1;
        }

        _time->hours = h;
        _time->minutes = m;
        _time->seconds = s;

        return result;
}

/*
 * @brief Нормализация даты.
 * @details Поля _date->days, _date->month и _date->year сбрасываются при
 *      превышении максимального значения.
 *
 *      Если параметр _how больше нуля, то перед всеми проверками день месяца
 *      увеличивается на 1.
 *
 *      Если параметр _how не равен нулю, то при сбросе дней инкриминируются
 *      месяцы; при сбросе месяцев инкриминируются года и при обнулении лет
 *      функция возвращает положительное значение.
 *
 *      После проверок поле _date->week_day устанавливается в соответствии с
 *      нормализованной датой.
 *
 * @param [in, out] _date Указатель на структуру для нормализации даты.
 * @param [in]      _how Ноль если при переполнении не нужно делать переноса в
 *      следующее поле.
 *
 * @return Если параметр @p _how равен нулю, то возвращается всегда ноль.
 * @return Если параметр _how не равен нулю, то при переполнении поля
 *      _date->year возвращается положительное значение; если переполнения не
 *      было, то возвращается отрицательное значение.
 */
int8_t rtc_validate_date(struct rtc_tm *_date, int8_t _how)
{
        uint8_t y, m, d, lim;
        int8_t result;

        result = _how != 0 ? -1 : 0;
        y = _date->year;
        m = _date->month;
        d = _date->day;

        if (_how > 0)
                d++;

        switch (m) {
        case 2:
                if ((y & 0x03) != 0) {
                        lim = 28;
                } else {
                        lim = 29;
                }
                break;
        case 4:
        case 6:
        case 9:
        case 11:
                lim = 30;
                break;
        default:
                lim = 31;
        }

        if (d == 0)
                d = 1;

        if (d > lim) {
                d = 1;
                if (_how != 0)
                        m++;
        }

        if (m == 0)
                m = 1;

        if (m > 12) {
                m = 1;
                if (_how != 0)
                        y++;
        }

        if (y > 99) {
                y = 0;
                if (_how != 0)
                        result = 1;
        }

        _date->day = d;
        _date->month = m;
        _date->year = y;

        if (m > 2) {
                m -= 2;
        } else {
                m += 10;
                y--;
        }

        /* Прежний код работал немного быстрее, но занимал больше места в памяти
         * программ.
         * _date->week_day = (d + ((uint16_t) (31 * m) / 12) + y + y / 4) % 7;
         **********************************************************************/
        _date->week_day = (rtc_get_days(_date) + 6) % 7;

        return result;
}

/*
 * @brief Нормализация даты и времени.
 * @details Все поля кроме _date->week_days и _date->actuals сбрасываются при
 *      превышении максимального значения.
 *
 *      Если параметр _how не равен нулю, то при обнулении секунд инкриминируются
 *      минуты; при обнулении минут инкриминируются часы; при обнулении часов
 *      инкриминируются дни; при сбросе дней инкриминируются месяцы; при сбросе
 *      месяцев инкриминируются года и при обнулении лет функция возвращает
 *      положительное значение.
 *
 *      После проверок поле _date->week_day устанавливается в соответствии с
 *      нормализованной датой.
 *
 * @param [in, out] _date Указатель на структуру для нормализации даты и времени.
 * @param [in]      _how Ноль если при переполнении не нужно делать переноса в
 *      следующее поле.
 *
 * @return Если параметр @p _how равен нулю, то возвращается всегда ноль.
 * @return Если параметр _how не равен нулю, то при переполнении поля
 *      _date->year возвращается положительное значение; если переполнения не
 *      было, то возвращается отрицательное значение.
 */
int8_t rtc_validate(struct rtc_tm *_date, int8_t _how)
{
        return rtc_validate_date(_date, rtc_validate_time(_date, _how));
}

uint_fast16_t rtc_get_day_of_year(const struct rtc_tm *_date)
{
        uint_fast16_t retval;
        uint8_t y, m;

        y = _date->year;
        m = _date->month;

        retval = _date->day - 1;
        retval += (uint16_t) 30 * (m - 1);

        if (m > 2) {
                retval--;
                if ((y & 0x03) != 0)
                        retval--;
        }

        if (m >= 9)
                m++;

        retval += m / 2;

        return retval;
}

rtc_utime_t rtc_get_days(const struct rtc_tm *_date)
{
        rtc_utime_t result;
        uint8_t y;

        y = _date->year;
        result = 365 * y + ((uint8_t) (y + 3) / 4);
        result += rtc_get_day_of_year(_date);

        return result;
}

rtc_utime_t rtc_get_seconds_from_midnight(const struct rtc_tm *_time)
{
        rtc_utime_t retval;

        retval = _time->seconds;
        retval += (uint16_t) _time->minutes * 60;
        retval += (rtc_utime_t) _time->hours * 3600;

        return retval;
}

rtc_utime_t rtc_get_seconds(const struct rtc_tm *_time)
{
        rtc_utime_t retval;

        retval = rtc_get_seconds_from_midnight(_time);
        retval += (rtc_utime_t) 86400 * rtc_get_days(_time);

        return retval;
}

void rtc_set_seconds(struct rtc_tm *_dst, rtc_utime_t _sec)
{
        uint8_t sec, min, hour, mday, mon, year, wday;
        uint16_t yday;
        rtc_utime_t days;
        rtc_utime_t n;

        n = _sec;

        sec = n % 60;
        n /= 60;
        min = n % 60;
        n /= 60;
        hour = n % 24;
        n /= 24;
        days = n;

        /* 1 января 2000 года - суббота. */
        wday = (6 + days) % 7;

        days *= 4;
        year = days / (365 * 4 + 1);
        yday = days % (365 * 4 + 1);
        yday /= 4;

        if ((year & 0x03) == 0) {
                if (yday >= 60)
                        yday += 2;
        } else {
                if (yday >= 59)
                        yday += 3;
        }

        if (yday >= 123)
                yday++;

        if (yday >= 185)
                yday++;

        if (yday >= 278)
                yday++;

        if (yday >= 340)
                yday++;

        mon = yday / 31;
        mday = yday % 31;
        mon++;
        mday++;

        _dst->seconds = sec;
        _dst->minutes = min;
        _dst->hours = hour;
        _dst->day = mday;
        _dst->month = mon;
        _dst->year = year;
        _dst->week_day = wday;
}

/**
 * @file
 * @brief
 * @details
 *
 * @date создан 13.01.2019
 * @author Nick Egorrov
 */
#include <string.h>
#include "../rtc.h"

void rtc_copy(struct rtc_tm *dst, struct rtc_tm const *src)
{
        memcpy(dst, src, sizeof(struct rtc_tm));
}

void rtc_to_tm(struct tm *dst, struct rtc_tm const *src)
{
        dst->tm_sec = src->seconds;
        dst->tm_min = src->minutes;
        dst->tm_hour = src->hours;
        dst->tm_wday = src->week_day;
        dst->tm_mday = src->day;
        dst->tm_mon = src->month - 1;
        dst->tm_year = src->year + 100;
        dst->tm_yday = rtc_day_in_year(src);
}

void rtc_date_adjust(struct rtc_tm *_date)
{
        uint8_t y, m, d, lim;

        y = _date->year;
        m = _date->month;
        d = _date->day;

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
                //m++;
        }

        if (m == 0)
                m = 1;

        if (m > 12) {
                m = 1;
                //y++;
        }

        if (y > 99)
                y = 0;

        _date->day = d;
        _date->month = m;
        _date->year = y;

        if (m > 2) {
                m -= 2;
        } else {
                m += 10;
                y--;
        }

        _date->week_day = (d + ((uint16_t) (31 * m) / 12) + y + y / 4) % 7;
}

uint_fast16_t rtc_day_in_year(const struct rtc_tm *_date)
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

rtc_utime_t rtc_day_in_millenium(const struct rtc_tm *_date)
{
        rtc_utime_t result;
        uint8_t y;

        y = _date->year;
        result = (rtc_utime_t) 365 * y + (y + 3) / 4;
        result += rtc_day_in_year(_date);

        return result;
}

rtc_utime_t rtc_seconds_from_midnight(const struct rtc_tm *_time)
{
        rtc_utime_t retval;

        retval = _time->seconds;
        retval += (uint16_t) _time->minutes * 60;
        retval += (rtc_utime_t) _time->hours * 3600;

        return retval;
}

rtc_utime_t rtc_seconds_from_millenium(const struct rtc_tm *_time)
{
        rtc_utime_t retval;

        retval = rtc_seconds_from_midnight(_time);
        retval += (rtc_utime_t) 86400 * rtc_day_in_millenium(_time);

        return retval;
}

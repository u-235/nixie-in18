/**
 * @file
 * @brief
 * @details
 *
 * @date создан 13.01.2019
 * @author Nick Egorrov
 */

#include "time.h"

extern void date_adjust(date_t *date)
{
        uint8_t y, m, d, lim;

        y = date->year;
        m = date->month;
        d = date->day;

        if (y > 99)
                y = 0;

        if (m > 12)
                m = 1;

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

        if (d > lim)
                d = 1;

        date->day = d;
        date->month = m;
        date->year = y;

        if (m > 2) {
                m -= 2;
        } else {
                m += 10;
                y--;
        }

        date->week_day = (d + y + (y >> 2) + ((uint16_t) (31 * m) / 12)) % 7;
}

extern uint32_t days_from_millenium(const date_t *date)
{
        uint32_t retval;
        uint8_t y, m;

        y = date->year;
        m = date->month;
        retval = date->day;
        retval += (uint16_t) 30 * (m - 1);
        if (m > 2) {
                retval--;
                if ((y & 0x03) != 0)
                        retval--;
        }
        if (m >= 9)
                m++;
        retval += m / 2;
        retval += (uint32_t) 365 * y + (y + 3) / 4;

        return retval;
}

extern uint32_t seconds_from_midnight(const time_t *time)
{
        uint32_t retval;

        retval = time->seconds;
        retval += (uint16_t) time->minutes * 60;
        retval += (uint32_t) time->hours * 3600;

        return retval;
}

extern uint32_t seconds_from_millenium(const date_t *date, const time_t *time)
{
        uint32_t retval;

        retval = seconds_from_midnight(time);
        retval += (uint32_t) 86400 * days_from_millenium(date);

        return retval;
}

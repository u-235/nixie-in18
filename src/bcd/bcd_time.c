/**
 * @file bcd_time.c
 *
 * @date Created on: 24.12.2017
 * @author Nick Egorrov
 */

#include "bcd_time.h"

extern int bcd_time_cmp(bcd_time_t *time1, bcd_time_t *time2)
{
        if (time1->hour < time2->hour) {
                return -1;
        }

        if (time1->min < time2->min) {
                return -1;
        }

        return time1->sec - time2->sec;
}

extern int bcd_date_cmp(bcd_date_t *date1, bcd_date_t *date2)
{
        if (date1->year < date2->year) {
                return -1;
        }

        if (date1->month < date2->month) {
                return -1;
        }

        return date1->date - date2->date;
}

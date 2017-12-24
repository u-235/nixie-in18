/**
 * \file bcd_time.c
 *
 * \date Created on: 24.12.2017
 * \author Nick Egorrov
 */

#include "bcd_time.h"

extern int bcd_time_cmp(bcd_time_t *time1, bcd_time_t *time2)
{
        int retval = time1->hour - time2->hour;
        if (retval != 0) return retval;
        retval = time1->min - time2->min;
        if (retval != 0) return retval;
        retval = time1->sec - time2->sec;
        return retval;
}

extern int bcd_date_cmp(bcd_date_t *date1, bcd_date_t *date2)
{
        int retval = date1->year - date2->year;
        if (retval != 0) return retval;
        retval = date1->month - date2->month;
        if (retval != 0) return retval;
        retval = date1->date - date2->date;
        return retval;
}

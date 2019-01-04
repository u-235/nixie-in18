/**
 * @file
 * @brief
 * @details
 *
 * @date создан 04.01.2019
 * @author Nick Egorrov
 */

#ifndef ALARM_H_
#define ALARM_H_

#include "stdint.h"
#include "bcd/bcd_time.h"

typedef struct _alarm{
        bcd2_t  hour;
        bcd2_t  min;
        uint8_t sound;
}alarm_t;

#ifdef _cplusplus
extern "C" {
#endif

extern void alarm_init();

extern void alarm_get(alarm_t *pa);

extern void alarm_set(alarm_t *pa);

extern char alarm_is_on();

extern void alarm_on();

extern void alarm_off();

extern void alarm_start();

extern void alarm_stop();

extern void alarm_check(const bcd_time_t *pt);

#ifdef _cplusplus
}
#endif

#endif /* ALARM_H_ */

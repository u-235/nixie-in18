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
#include "hal/rtc.h"

typedef struct _alarm {
        uint8_t hours;
        uint8_t minutes;
        uint8_t sound;
} alarm_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void alarm_init(const rtc_tm *p_tm);

extern void alarm_get(alarm_t *pa);

extern void alarm_set(alarm_t *pa);

extern char alarm_is_on(void);

extern void alarm_on(void);

extern void alarm_off(void);

extern char alarm_is_jingle(void);

extern void alarm_start(void);

extern void alarm_stop(void);

extern void alarm_check(void);

#ifdef __cplusplus
}
#endif

#endif /* ALARM_H_ */

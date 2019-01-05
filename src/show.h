/**
 * \file
 * \brief
 * \details
 *
 * \date создан 13.11.2018
 * \author Nick Egorrov
 */

#ifndef SHOW_H_
#define SHOW_H_

#include "bcd/bcd_time.h"
#include "user.h"

typedef enum {
        SHOW_INTRO = 1,
        SHOW_ERROR,
        SHOW_TIME,
        SHOW_DATE
} show_t;

#ifdef __cplusplus
extern "C" {
#endif

extern void show_init(const bcd_time_t *pt, const bcd_date_t *pd);

extern void show_set(show_t _show);

extern void show_handle_key(const key_t _key);

extern void show_synchronize();

#ifdef __cplusplus
}
#endif

#endif /* SHOW_H_ */

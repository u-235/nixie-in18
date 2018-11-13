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

#include "user.h"

typedef enum {
        SHOW_INTRO,
        SHOW_TIME,
        SHOW_DATE
} show_t;

#ifdef _cplusplus
extern "C" {
#endif

extern void show_init();

extern void show_set(show_t _show);

extern void show_handle_key(key_t _key);

extern void show_synchronize();

extern void show_update();

#ifdef _cplusplus
}
#endif

#endif /* SHOW_H_ */

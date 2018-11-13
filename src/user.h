/**
 * \file
 * \brief
 * \details
 *
 * \date создан 13.11.2018
 * \author Nick Egorrov
 */

#ifndef USER_H_
#define USER_H_

typedef enum {
        VK_NO
} key_t;

#ifdef _cplusplus
extern "C" {
#endif

extern void user_init();

extern void user_scan_key();

extern key_t user_get_key();

#ifdef _cplusplus
}
#endif

#endif /* USER_H_ */

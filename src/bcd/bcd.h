/**
 * @file
 * @ingroup bcd
 * @brief Реализация BCD арифметики.
 * @details
 *
 * @date создан 17.10.2015
 * @author Nick Egorrov
 */

#ifndef BCD_H_
#define BCD_H_

#include <stdint.h>

/**
 * @brief BCD число на две декады.
 * @details Этот тип может вмещать числа от 0 до 99 включительно.
 */
typedef uint8_t bcd2_t;

/**
 * Макрос для получения BCD числа из константы.
 * @param i число от 0 до 99 включительно.
 */
#define _bcd2_make(i) ((bcd2_t)(i%10 + ((i/10)*16)))

#ifdef __cplusplus
extern "C" {
#endif

extern bcd2_t bcd_from_uint8(uint8_t tc);

extern uint8_t bcd_to_uint8(bcd2_t tc);

extern bcd2_t bcd2_inc(bcd2_t a);

extern bcd2_t bcd2_dec(bcd2_t a);

extern bcd2_t bcd_add2(bcd2_t a, bcd2_t b);

extern bcd2_t bcd_sub2(bcd2_t a, bcd2_t b);

#ifdef __cplusplus
}
#endif

#endif /* BCD_H_ */

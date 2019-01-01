/**
 * @file
 * @ingroup bcd
 * @brief Реализация BCD арифметики.
 * @details
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

bcd2_t bcd_from_uint8(uint8_t tc);
uint8_t bcd_to_uint8(bcd2_t tc);

bcd2_t bcd_add2(bcd2_t a, bcd2_t b);
bcd2_t bcd_sub2(bcd2_t a, bcd2_t b);

#ifdef __cplusplus
}
#endif

#endif /* BCD_H_ */

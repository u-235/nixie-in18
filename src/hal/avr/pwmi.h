/**
 * @file
 *
 * @date создан 03 янв. 2015 г.
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef HAL_AVR_PWMI_H_
#define HAL_AVR_PWMI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void pwmi_init(void);
void pwmi_load(uint8_t data[]);

#ifdef __cplusplus
}
#endif

#endif /* HAL_AVR_PWMI_H_ */

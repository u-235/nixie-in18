/**
 * @file
 * @brief
 * @details
 *
 * @date создан 10.03.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include "stdint.h"

#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void adc_init(void);

extern void adc_set_channel(uint8_t ch);

extern uint8_t adc_get_channel(void);

extern uint8_t adc_is_busy(void);

extern void adc_start(void (*on_complete)(uint8_t ch, uint8_t res));

extern uint8_t adc_get_result(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_H_ */

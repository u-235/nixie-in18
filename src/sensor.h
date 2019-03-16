/**
 * @file
 * @brief
 * @details
 *
 * @date создан 11.03.2019
 * @author Nick Egorrov
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "hal/display.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void sensor_init(void);

extern uint8_t sensor_get_bright(void);

extern uint8_t sensor_get_rate(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_H_ */

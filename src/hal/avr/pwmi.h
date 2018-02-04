#ifndef HAL_AVR_PWMI_H_
#define HAL_AVR_PWMI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void pwmi_init();
void pwmi_load(uint8_t data[]);

#ifdef __cplusplus
}
#endif

#endif // HAL_AVR_PWMI_H_

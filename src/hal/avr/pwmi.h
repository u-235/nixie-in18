#ifndef HAL_AVR_PWMI_H_
#define HAL_AVR_PWMI_H_

#include <stdint.h>

void pwmi_init();
void pwmi_load(uint8_t data[]);

#endif // HAL_AVR_PWMI_H_

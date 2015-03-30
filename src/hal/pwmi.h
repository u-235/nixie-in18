#ifndef HAL_PWMI_H_
#define HAL_PWMI_H_

#include <stdint.h>

void pwmi_init();
void pwmi_load(uint8_t data[]);

#endif // HAL_PWMI_H_

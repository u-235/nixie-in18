#ifndef HAL_PWMI_H_
#define HAL_PWMI_H_

void pwmi_init();
void pwmi_load(unsigned char data[]);

#include "pwmi-hl.h"
void pwmi_bright(unsigned char lvl);
void pwmi_rate(unsigned char lvl);

#endif // HAL_PWMI_H_

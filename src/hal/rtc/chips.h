/**
 * \file chips.h
 * \defgroup RTC_CHIPS <chips.h> : Микросхемы RTC
 * \ingroup RTC
 *
 * \date created on: 21.01.2018
 * \author: Nick Egorrov
 */

#ifndef SRC_HAL_RTC_CHIPS_H_
#define SRC_HAL_RTC_CHIPS_H_

#ifndef HAL_RTC_H_
#       error "include <hal/>rtc.h> instead of this file"
#endif

#define INSIDE_SRC_HAL_RTC_CHIPS_H_

#ifdef __RTC_DC1307__
#       include "chip_dc1307.h"
#elif defined __RTC_M41T56__
#       include "chip_m41t56.h"
#else
#       warning "rtc chip not defined"
#endif

#undef INSIDE_SRC_HAL_RTC_CHIPS_H_

#endif /* SRC_HAL_RTC_CHIPS_H_ */

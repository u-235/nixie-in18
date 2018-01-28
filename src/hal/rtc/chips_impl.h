/**
 * \file chips_impl.h
 * \brief Реализация особенностей RTC
 * \details Различные ИМС могут иметь дополнительные возможности. Этот файл
 * подключает файл chip_XXX_impl.h в зависимости от используемой RTC.
 *
 * \date created on: 21.01.2018
 * \author: Nick Egorrov
 */

#ifndef HAL_RTC_CHIPS_IMPL_H_
#define HAL_RTC_CHIPS_IMPL_H_

#ifndef HAL_RTC_CHIPS_IMPL_C_
#       error "this file most be included only in rtc.c"
#endif

#define INSIDE_SRC_HAL_RTC_CHIPS_IMPL_H_

#ifdef __RTC_DC1307__
#       include "chip_dc1307_impl.h"
#elif defined __RTC_M41T56__
#       include "chip_m41t56_impl.h"
#else
#       warning "rtc chip not defined"
#endif

#undef INSIDE_SRC_HAL_RTC_CHIPS_IMPL_H_

#endif /* HAL_RTC_CHIPS_IMPL_H_ */

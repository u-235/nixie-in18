/**
 * \file chip_m41t56.h
 * \defgroup RTC_CHIPS_M41T56 Микросхема RTC M41T56
 * \ingroup RTC_CHIPS
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

#ifndef HAL_RTC_CHIP_M41T56_H_
#define HAL_RTC_CHIP_M41T56_H_

#define rtc_init        rtc_init_m41t56
#define rtc_error       rtc_error_m41t56
#define rtc_clear       rtc_clear_m41t56
#define rtc_check       rtc_check_m41t56
#define rtc_get_time    rtc_get_time_m41t56
#define rtc_set_time    rtc_set_time_m41t56
#define rtc_get_date    rtc_get_date_m41t56
#define rtc_set_date    rtc_set_date_m41t56

#ifdef _cplusplus
extern "C" {
#endif

#ifdef _cplusplus
}
#endif
#endif /* HAL_RTC_CHIP_M41T56_H_ */

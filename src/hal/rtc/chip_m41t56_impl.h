/**
 * \file chip_m41t56_impl.h
 * \brief Реализация RTC M41T56
 * \details
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

#ifndef HAL_RTC_CHIP_M41T56_IMPL_H_
#define HAL_RTC_CHIP_M41T56_IMPL_H_

#ifndef INSIDE_SRC_HAL_RTC_CHIPS_IMPL_H_
#       error "include chips_impl.h instead of this file"
#endif

#include "../iic.h"

#define IIC_ADDRESS 0xd0

static void impl_index(uint8_t i)
{
}

static void impl_begin()
{
        iic_set_address(IIC_ADDRESS);
}

static void check_start()
{

}

static uint8_t impl_read(uint8_t last)
{
        check_start();
        return iic_ll_read(last);
}

static void impl_write(uint8_t d)
{
        check_start();
        iic_ll_write(d);
}

static void impl_end()
{
        iic_ll_stop();
}

#endif /* HAL_RTC_CHIP_M41T56_IMPL_H_ */

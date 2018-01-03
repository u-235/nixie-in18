/**
 * \file main.c
 *
 * \date created on: 02 нояб. 2015 г.
 * \author Nick Egorrov
 */

#include "hal/mcu.h"
#include "hal/display.h"
#include "bcd/bcd.h"
#include "tms/tms.h"

int main(void)
{
        mcu_init();
        display_init();
        tms_init();
        mcu_interrupt_unlock();

        while (1) {
                if (mcu_get_timer_fire() != 0) {
                        tms_tick();
                }
        }

        return 0;
}

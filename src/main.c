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
#include "events.h"

int main(void)
{
        mcu_init();
        display_init();
        tms_init();
        mcu_interrupt_unlock();

        while (1) {
                if (mcu_get_timer_fire() != 0) {
                        // TODO опрос кнопок
                        tms_tick();
                } else {
                        // TODO проверка обновления RTC
                }
        }

        return 0;
}

enum mode{
SHOW_INTRO,
SHOW_TIME,
SHOW_DATE,
SHOW_ERROR
} current_mode;

extern void event_handler(event_t ev){
        switch(ev){
        case EVENT_START:
                break;
        case EVENT_INTRO_END:
                break;
        case EVENT_ALARM:
                break;
        case EVENT_ERROR:
                break;
        }
}

extern void event_key_handler(event_key_t key){

}

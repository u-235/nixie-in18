/**
 * @file
 * @brief Взаимодействие с пользователем
 * @details  Сканирование кнопок и управление звуком.
 *
 * @date создан 13.11.2018
 * @author Nick Egorrov
 */

#include "config.h"
#include "hal/mcu.h"
#include "tms/tms.h"
#include "user.h"

#define SCAN_PERIOD     20
#define _from_ms(ms) ((unsigned long)ms/SCAN_PERIOD)

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static timer_id_t timer_scan;
static key_t key = VK_NO;

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void scan_key(void);

static void send_key(uint8_t keys);

/*************************************************************
 *      Public function
 *************************************************************/

void user_init()
{
        timer_scan = tms_create_timer(&scan_key);
        tms_set_timer(timer_scan, _ticks_from_ms(SCAN_PERIOD));
        tms_start_timer(timer_scan);
}

key_t user_get_key()
{
        key_t retval = key;
        key = VK_NO;
        return retval;
}

/*************************************************************
 *      Private function.
 *************************************************************/

typedef enum {
        SCANER_WAIT,
        SCANER_MENU,
        SCANER_SELECT,
        SCANER_CHANGE,
        SCANER_CHANGE_FAST
} scan_state_t;

void scan_key()
{
        static scan_state_t state = SCANER_WAIT;
        static uint8_t bounce = 0, delay = 0, repeat;
        uint8_t keys;

        if (delay != 0) {
                delay--;
        }

        if (bounce != 0) {
                bounce--;
                return;
        }

        keys = mcu_input_keys();
        switch (state) {
        case SCANER_WAIT:
                if ((keys & KEY_MENU) != 0) {
                        state = SCANER_MENU;
                        send_key(VK_MENU_DOWN);
                } else if ((keys & KEY_SELECT) != 0) {
                        delay = _from_ms(CFG_USER_KEY_REPEAT_DELAY);
                        repeat = CFG_USER_KEY_REPEAT_NUMBER;
                        state = SCANER_SELECT;
                        send_key(VK_SELECT_DOWN);
                } else if ((keys & KEY_CHANGE) != 0) {
                        delay = _from_ms(CFG_USER_KEY_REPEAT_DELAY);
                        repeat = CFG_USER_KEY_REPEAT_NUMBER;
                        state = SCANER_CHANGE;
                        send_key(VK_CHANGE_DOWN);
                }
                break;
        case SCANER_MENU:
                if ((keys & KEY_MENU) == 0) {
                        state = SCANER_WAIT;
                        send_key(VK_MENU_UP);
                }
                break;
        case SCANER_SELECT:
                if ((keys & KEY_SELECT) == 0) {
                        state = SCANER_WAIT;
                        send_key(VK_SELECT_UP);
                } else if (delay == 0) {
                        if (repeat > 0) {
                                repeat--;
                                delay = _from_ms(CFG_USER_KEY_REPEAT_PERIOD);
                        } else {
                                delay = _from_ms(CFG_USER_KEY_REPEAT_FAST);
                        }
                        send_key(VK_SELECT_DOWN);
                }
                break;
        case SCANER_CHANGE:
                if ((keys & KEY_CHANGE) == 0) {
                        state = SCANER_WAIT;
                        send_key(VK_CHANGE_UP);
                } else if (delay == 0) {
                        if (repeat > 0) {
                                repeat--;
                                delay = _from_ms(CFG_USER_KEY_REPEAT_PERIOD);
                        } else {
                                delay = _from_ms(CFG_USER_KEY_REPEAT_FAST);
                        }
                        send_key(VK_CHANGE_DOWN);
                }
                break;
        default:
                ;
        }
        bounce = _from_ms(CFG_USER_KEY_BOUNCE);
}

/*************************************************************
 *      Private function.
 *************************************************************/

void send_key(uint8_t keys)
{
        key = keys;
}

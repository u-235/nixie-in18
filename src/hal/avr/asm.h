/**
 * @file
 *
 * @date created on: 07 янв. 2015 г.
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef HAL_AVR_ASM_H_
#define HAL_AVR_ASM_H_

#define _nop(vd)\
        do {\
                asm volatile("nop");\
        } while (0)

#define mcu_watchdog_reset(vd)\
        do {\
                asm("wdr");\
        } while (0)
#define mcu_interrupt_enable(vd)\
        do {\
                asm("sei");\
        } while (0)
#define mcu_interrupt_disable(vd)\
        do {\
                asm("cli");\
        } while (0)

#endif // HAL_AVR_ASM_H_

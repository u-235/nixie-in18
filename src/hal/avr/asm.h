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

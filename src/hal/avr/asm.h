#ifndef HAL_AVR_ASM_H_
#define HAL_AVR_ASM_H_

#define _nop asm volatile("nop")

#define mcu_watchdog_reset() asm("wdr")
#define mcu_interrupt_enable() asm("sei")
#define mcu_interrupt_disable() asm("cli")

#endif // HAL_AVR_ASM_H_

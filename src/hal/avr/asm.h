#ifndef HAL_AVR_ASM_H_
#define HAL_AVR_ASM_H_

#define _nop asm volatile("nop")
#define _sei asm volatile("sei")
#define _cli asm volatile("cli")

#endif // HAL_AVR_ASM_H_

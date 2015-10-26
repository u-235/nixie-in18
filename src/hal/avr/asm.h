#ifndef AVR_ASM_H_
#define AVR_ASM_H_

#define _nop asm volatile("nop")
#define _sei asm volatile("sei")
#define _cli asm volatile("cli")

#endif // AVR_ASM_H_

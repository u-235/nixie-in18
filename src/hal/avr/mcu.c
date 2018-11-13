/**
 * \file mcu.c
 *
 * \date created on: 03.01.2018
 * \author Nick Egorrov
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "asm.h"
#include "../mcu.h"
#include "spi.h"
#include "pwmi.h"
#include "../iic.h"

// Несколько макросов в целях совместимости с Atmega8
#ifdef __AVR_ATmega8__
/*
 * При тактовой частоте в 8 МГц, делителе = 1024 для работы системного таймера
 * с частотой 50Гц надо считать до 156 или от 100 до 256, поскольку таймер 0
 * делает прерывание только при переполнении.
 */
#define _init_timer() \
        {\
        TCCR0 = (1 << CS02) | (1 << CS00);\
        TIMSK |= 1 << TOIE0;\
        }
// Вектор прерывания при переполнении таймера 0
#define TIMER_FIRE TIMER0_OVF_vect
#define _update_timer() TCNT0=100

/*
 * Дальше вариант для Atmega168.
 * Хотя таймер 0 в Atmega168 имеет больше возможностей чем в Atmega8,
 * используется тот же режим прерывания при переполнении счётчика.
 */
#elif defined __AVR_ATmega168__
#define _init_timer() \
        {\
        TCCR0A = 0;\
        TCCR0B = (1 << CS02) | (1 << CS00);\
        TIMSK0 |= 1 << TOIE0;\
        }

#define TIMER_FIRE TIMER0_OVF_vect
#define _update_timer() TCNT0=100

#else
#error invalid MCU
#endif

static volatile unsigned char timer_fire = 0;

extern void mcu_init()
{
        spi_init();
        pwmi_init();
        iic_init();

        _init_timer()
        ;
}

extern unsigned char mcu_get_timer_fire()
{
        unsigned char result;

        mcu_interrupt_lock();
        result = timer_fire;
        if (timer_fire > 0) {
                timer_fire--;
        }
        mcu_interrupt_unlock();

        return result;
}

static unsigned char lock_counter = 0;

extern void mcu_interrupt_lock()
{
        mcu_interrupt_disable();
        lock_counter++;
}

extern void mcu_interrupt_unlock()
{
        if (lock_counter != 0) {
                lock_counter--;
                if (lock_counter == 0) {
                        mcu_interrupt_enable();
                }
        }
}

/*
 * Прерывание при совпадении таймера.
 */

ISR(TIMER_FIRE)
{
        _update_timer();
        timer_fire++;
}


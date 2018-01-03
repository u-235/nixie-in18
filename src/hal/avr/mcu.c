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

static volatile unsigned char timer_fire = 0;

extern void mcu_init()
{
        spi_init();
        pwmi_init();

        //TODO init system timer;
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
        if(lock_counter !=0){
                lock_counter--;
                if(lock_counter ==0){
                        mcu_interrupt_enable();
                }
        }
}

/*
 * Прерывание при совпадении таймера.
 */

ISR(TIMER0_COMPA_vect)
{
        timer_fire++;
}


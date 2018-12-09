/**
 * @file mcu.c
 * @details
 *
 * @par Виртуальный порт.
 * Часы используют отдельную ИМС с плеером Чана, которая принимает номер песни
 * в виде 6 линий. Эти линии подключены к порту D так, что бы было удобно
 * разводить плату. Однако, это делает неудобным программирование. И для
 * удобства программирования используется виртуальный порт. Виртуальный порт -
 * это пара переменных, которые после модификаций функциями копируются в тот
 * или иной железный порт. Это происходит при помощи макросов #_virt_set и
 * #_virt_mask. Если бы функция mcu_output_player() работала бы непосредственно с
 * железным портом, это привело бы к лишним записям в этот порт (он объявлен
 * как volatile и компилятор не может оптимизировать) и раздутию кода.
 *
 * @date created on: 03.01.2018
 * @author Nick Egorrov
 */

#include "../../config.h"
#include "../../bit-helper.h"
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
 * При тактовой частоте в 16 МГц, делителе = 1024 для работы системного таймера
 * с частотой 100Гц надо считать до 156 или от 100 до 256, поскольку таймер 0
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

/*
 * Макросы для организации виртуального порта.
 */
#define _virt_mask(bit) _complex_on(mask, bit)
#define _virt_set(bit) _complex_on(virt, bit)
#define _complex_on(pre, su, pin) _bit_set(pre##su, pin)
#define _virt_prepare(l) static uint8_t mask##l = 0, virt##l
#define _virt_clean(l) mask##l = 0; virt##l =0
#define _virt_apply(l) PORT##l = (PORT##l & ~mask##l) | virt##l

/*************************************************************
 *      Private function prototype.
 *************************************************************/
/*
 * Подготовка вывода с использованием виртуального порта.
 */
static void out_begin();
/*
 * Завершение вывода с использованием виртуального порта.
 */
static void out_end();
/*
 * Запись маски виртуального порта.
 */
static void mask_player();

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static uint8_t timer_fire = 0;
/*
 * Виртуальный порт D.
 */
_virt_prepare(D);

/*************************************************************
 *      Public function
 *************************************************************/

extern void mcu_init()
{
        _dir_in(CFG_KEY_MENU);
        _dir_in(CFG_KEY_SELECT);
        _dir_in(CFG_KEY_CHANGE);

        mask_player();
        DDRD |= maskD;

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

/**
 * @brief Получение кода нажатых кнопок.
 * @return Любая комбинация #KEY_MENU, #KEY_SELECT и #KEY_CHANGE.
 */
extern uint8_t mcu_input_keys()
{
        uint8_t keys = 0;

        if (_is_pin_clean(CFG_KEY_MENU)) {
                keys |= KEY_MENU;
        }

        if (_is_pin_clean(CFG_KEY_SELECT)) {
                keys |= KEY_SELECT;
        }

        if (_is_pin_clean(CFG_KEY_CHANGE)) {
                keys |= KEY_CHANGE;
        }

        return keys;
}

/**
 * Выставление номера песни на выводы управления плеером.
 * @param sel Номер песни. 0 - песня не проигрывается.
 */
extern void mcu_output_player(uint8_t sel)
{
        out_begin();
        mask_player();

        if (sel & (1 << 0)) {
                _virt_set(CFG_PLAYER_ADR1);
        }
        if (sel & (1 << 1)) {
                _virt_set(CFG_PLAYER_ADR2);
        }
        if (sel & (1 << 2)) {
                _virt_set(CFG_PLAYER_ADR4);
        }
        if (sel & (1 << 3)) {
                _virt_set(CFG_PLAYER_ADR8);
        }
        if (sel & (1 << 4)) {
                _virt_set(CFG_PLAYER_ADR16);
        }
        if (sel & (1 << 5)) {
                _virt_set(CFG_PLAYER_ADR32);
        }

        out_end();
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

/*************************************************************
 *      Interrupt handler
 *************************************************************/

/*
 * Прерывание при совпадении таймера.
 */
ISR(TIMER_FIRE)
{
        _update_timer();
        timer_fire++;
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void out_begin()
{
        mcu_interrupt_lock();
        /*_virt_clean(B); порт B */
        /*_virt_clean(C); порт C */
        _virt_clean(D); /* порт D */
}

static void out_end()
{
        /*_virt_apply(B); порт B */
        /*_virt_apply(C); порт C */
        _virt_apply(D); /* порт D */
        mcu_interrupt_unlock();
}

static void mask_player()
{
        _virt_mask(CFG_PLAYER_ADR1);
        _virt_mask(CFG_PLAYER_ADR2);
        _virt_mask(CFG_PLAYER_ADR4);
        _virt_mask(CFG_PLAYER_ADR8);
        _virt_mask(CFG_PLAYER_ADR16);
        _virt_mask(CFG_PLAYER_ADR32);
}

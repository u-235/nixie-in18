/**
 * @file
 * @brief Модуль ШИМ индикации.
 * @details Этот модуль реализует программный ШИМ индикации,
 *  позволяющий регулировать яркость свечения дисплея и осуществлять
 *  эффект плавной смены старых показаний на новые.
 *
 *  Программный ШИМ для синхронизации использует прерывания при
 *  совпадении таймера 2. В общем случае цикл ШИМ может состоять из следующих
 *  фаз:
 *  - #STEP_OLD - показ старых данных. Длительность фазы непостоянна.
 *  - #STEP_NEW - показ новых данных. Длительность фазы непостоянна.
 *  - #STEP_AND - показ данных, которые есть и в старых и в новых. Длительность
 *    фазы непостоянна. Это нужно для устранения притухания неизменной части
 *    показаний из-за нелинейной характеристики регулировки яркости и баланса.
 *  - #STEP_HIDE - индикатор погашен. Длительность фазы непостоянна. Этот шаг
 *    нужен для сохранения постоянства периода ШИМ при неполной яркости.
 *  - #STEP_CALK - подготовка данных для следующего цикла, индикатор погашен.
 *    Имеет фиксированную длительность.
 *
 *  Важно знать, что длительность цикла неизменна и что цикл содержит любую
 *  комбинацию из первых четырёх фаз и всегда содержит последнюю фазу.
 *
 * @date создан 03 янв. 2015 г.
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../../config.h"
#include "../display.h"
#include "pwmi.h"
#include "spi.h"

// Несколько макросов в целях совместимости с Atmega8
#ifdef __AVR_ATmega8__
// При тактовой частоте в 8 МГц, делителе = 256 и
// PWM_CYCLE_DURATION = 260 получаем частоту ШИМ около 120 Гц.
#define TIMER_PRESLALER 256U
#define _init_timer(vd)\
        do {\
                TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21);\
                TIMSK |= 1 << OCIE2;\
        } while (0)
// Вектор прерывания при совпадении в таймере 2
#define TIMER_COMPARE TIMER2_COMP_vect
#define TIMER_COUNTER OCR2
// Дальше вариант для Atmega168
#elif defined __AVR_ATmega168__
#define TIMER_PRESLALER 256U
#define _init_timer(vd)\
        do {\
                TCCR2A = (1 << WGM21);\
                TCCR2B = (1 << CS22) | (1 << CS21);\
                TIMSK2 |= 1 << OCIE2A;\
        } while (0)

#define TIMER_COMPARE TIMER2_COMPA_vect
#define TIMER_COUNTER OCR2A

#else
#error invalid MCU
#endif

/**
 * Длительность цикла ШИМ в тактах таймера.
 */
#define PWM_CYCLE_DURATION 260
/**
 * Длительность последнего шага ШИМ в тактах таймера.
 */
#define LAST_STEP_DURATION 5
/**
 * Частота ШИМ. Вычисляется как F_CPU/TIMER_PRESLALER/PWM_CYCLE_DURATION
 */
#define F_PWM (F_CPU/TIMER_PRESLALER/PWM_CYCLE_DURATION)

#if (PWM_CYCLE_DURATION - LAST_STEP_DURATION) > 255
#error PWM_CYCLE_DURATION - LAST_STEP_DURATION most be <= 255
#endif
/**
 * Максимальное значение внутреннего представления баланса старых и новых
 * показаний индикатора.
 */
#define BALANCE_MAX (F_PWM * 32U)

/**
 * @def STEP_OLD
 *  Фаза показа старых значений.
 *
 * @def STEP_NEW
 * Фаза показа новых значений.
 *
 * @def STEP_AND
 * Фаза "дожигания" неизменных значений.
 *
 * @def STEP_HIDE
 * Фаза без индикации.
 *
 * @def STEP_CALK
 * Фаза вычислений.
 */
#define STEP_OLD  0
#define STEP_NEW  1
#define STEP_AND  2
#define STEP_HIDE 3
#define STEP_CALK 4

/**
 * @brief Приведение баланса к диапазону от 0 до #DISPLAY_BRIGHT_MAX.
 * @details Внутри обработчика прерывания баланс старых и новых показаний
 *      индикатора представлен переменной с диапазоном от 0 до #BALANCE_MAX,
 *      который избыточен для вычисления длительности этапа ШИМ.
 * @param wide_bal внутреннее представление баланса.
 * @return Баланс в диапазоне от 0 до #DISPLAY_BRIGHT_MAX включительно.
 */
static uint8_t abjust_balance(uint16_t wide_bal);

/**
 * @brief Вычисление длительности этапа ШИМ.
 * @param bright Яркость в диапазоне от #DISPLAY_BRIGHT_MIN до #DISPLAY_BRIGHT_MAX
 *      включительно.
 * @param balance Баланс в диапазоне от 0 до #DISPLAY_BRIGHT_MAX включительно.
 */
static uint8_t calculate_duration(uint8_t bright, uint8_t balance);

/**
 * Вычисление количества тактов таймера для части цикла ШИМ.
 * @param d Длительность части цикла в диапазоне от 0.0 до 1.0
 *      Поскольку это макрос, проверки допустимости нет.
 */
#define _ticks(d) ((PWM_CYCLE_DURATION - LAST_STEP_DURATION)* d)

#define _steps(r) (BALANCE_MAX/(F_PWM*r))

static uint8_t bright;  //! Яркость индикации.
static uint16_t rate;  //! Скорость смены старых показаний на новые.
static uint8_t step;  //! Текущий шаг цикла ШИМа.
static uint8_t loaded;  //! Флаг загрузки значений.
/**
 * @var pNewValue
 * Указатель на буфер с новыми данными для индикации.
 * @var pOldValue
 * Указатель на буфер со старыми данными для индикации.
 * @var pAndValue
 * Указатель на буфер данными, общими для новых и старых значений.
 * @var pTmpValue
 * Указатель на буфер для временного хранения данных, загруженных функцией
 * pwmi_load().
 */
static uint8_t *pNewValue, *pOldValue, *pAndValue, *pTmpValue;

/**
 * @brief Инициализация ШИМа.
 * @details Эта функция должна быть вызвана до вызова любой другой
 * функции.
 */
void pwmi_init()
{
        /*
         * Этот трюк с буферами нужен что бы скрыть их от прямого доступа, а
         * так же что бы не морочиться с названиями.)) Однако это может быть
         * проблемой при вызове функции pwmi_load до вызова pwmi_init.
         */
        static uint8_t buff0[SPI_ARRAY_SIZE], buff1[SPI_ARRAY_SIZE],
                        buff2[SPI_ARRAY_SIZE], buff3[SPI_ARRAY_SIZE];
        pNewValue = buff0;
        pOldValue = buff1;
        pAndValue = buff2;
        pTmpValue = buff3;

        step = STEP_CALK;
        TIMER_COUNTER = 64;
        _init_timer();
}

/**
 * @brief Установка яркости индикации.
 * @param lvl Яркость в диаппазоне от #DISPLAY_BRIGHT_MIN до #DISPLAY_BRIGHT_MAX
 *      включительно.
 */
void display_bright(uint8_t lvl)
{
        if (lvl > DISPLAY_BRIGHT_MAX) {
                lvl = DISPLAY_BRIGHT_MAX;
        } else if (lvl < DISPLAY_BRIGHT_MIN) {
                lvl = DISPLAY_BRIGHT_MIN;
        }
        bright = lvl;
}

/**
 * Установка скорости смены показаний индикатора со старых на новые.
 * @param lvl Скорость в диаппазоне от #DISPLAY_RATE_MIN до #DISPLAY_RATE_MAX
 *      включительно.
 */
void display_rate(uint8_t lvl)
{
        static const PROGMEM uint16_t steps[] = {
                _steps(1.0), _steps(0.83), _steps(0.69), _steps(0.58),
                _steps(0.48), _steps(0.40), _steps(0.34), BALANCE_MAX
        };

        if (lvl > DISPLAY_RATE_MAX) {
                lvl = DISPLAY_RATE_MAX;
        } else if (lvl < DISPLAY_RATE_MIN) {
                lvl = DISPLAY_RATE_MIN;
        }
        rate = pgm_read_word(&steps[lvl]);
}

/**
 * @brief Загрузка данных для индикации.
 * @details Вызов этой функции возможен @b только @b после @b инициализации
 *  модуля функцией pwmi_init().
 * @param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void pwmi_load(uint8_t data[])
{
        /*
         * По хорошему нужно проверять pTmpValue на ноль из-за особенностей
         * инициализации указателей.
         */
        memcpy(pTmpValue, data, SPI_ARRAY_SIZE);
        loaded = 1;
}

/**
 * Обработчик прерывания при совпадении в таймере 2.
 */
ISR(TIMER_COMPARE)
{
        static uint16_t balance;  // Баланс яркости нового и старого значения.
        static uint8_t duration[4];
        uint8_t bal;

        switch (step) {
        case STEP_OLD:
                step++;
                if (duration[STEP_OLD] != 0) {
                        TIMER_COUNTER = duration[STEP_OLD];
                        spi_send_array(pOldValue);
                        break;
                }
                /* no break */
        case STEP_NEW:
                step++;
                if (duration[STEP_NEW] != 0) {
                        TIMER_COUNTER = duration[STEP_NEW];
                        spi_send_array(pNewValue);
                        break;
                }
                /* no break */
        case STEP_AND:
                step++;
                if (duration[STEP_AND] != 0) {
                        TIMER_COUNTER = duration[STEP_AND];
                        spi_send_array(pAndValue);
                        break;
                }
                /* no break */
        case STEP_HIDE:
                step++;
                if (duration[STEP_HIDE] != 0) {
                        TIMER_COUNTER = duration[STEP_HIDE];
                        spi_clean();
                        break;
                }
                /* no break */
        case STEP_CALK:
                step++;
                TIMER_COUNTER = LAST_STEP_DURATION;
                spi_clean();

                if (loaded != 0) {
                        /*
                         * Что бы избежать копирования массивов при
                         * поступлении новых данных используется ротация
                         * указателей. Кроме того, обновляются данные в
                         * массиве для шага #STEP_AND.
                         */
                        uint8_t *pOld = pNewValue;
                        uint8_t *pNew = pTmpValue;
                        uint8_t *pAnd = pAndValue;
                        pTmpValue = pOldValue;
                        pOldValue = pOld;
                        pNewValue = pNew;

                        for (uint8_t i = SPI_ARRAY_SIZE; i > 0; i--) {
                                *pAnd++ = *pNew++ & *pOld++;
                        }
                        loaded = 0;
                        balance = 0;
                }

                if (balance < BALANCE_MAX) {
                        balance += rate;
                }

                bal = abjust_balance(balance);
                duration[STEP_NEW] = calculate_duration(bright, bal);
                duration[STEP_OLD] = calculate_duration(bright,
                DISPLAY_BRIGHT_MAX - bal);
                duration[STEP_AND] = calculate_duration(bright,
                DISPLAY_BRIGHT_MAX);
                duration[STEP_HIDE] = PWM_CYCLE_DURATION - LAST_STEP_DURATION
                                - duration[STEP_AND];
                duration[STEP_AND] -= duration[STEP_NEW] + duration[STEP_OLD];
                if (duration[STEP_AND] == 1) {
                        duration[STEP_AND] = 0;
                        duration[STEP_HIDE]++;
                }
                step = 0;
        }
}

uint8_t abjust_balance(uint16_t wide_bal)
{
        uint16_t steps = 0;
        uint8_t i;
        for (i = 0; i < DISPLAY_BRIGHT_MAX; i++) {
                if (steps >= wide_bal) {
                        break;
                }
                steps += BALANCE_MAX / DISPLAY_BRIGHT_MAX;
        }
        return i;
}

#ifdef CFG_DISPLAY_LOGARITHMIC_BALANCE
uint8_t calculate_duration(uint8_t bright, uint8_t balance)
{
        static const PROGMEM uint8_t levels[] = {
                0, _ticks(0.010), _ticks(0.012), _ticks(0.014), _ticks(0.016),
                _ticks(0.018), _ticks(0.021), _ticks(0.024), _ticks(0.028),
                _ticks(0.033), _ticks(0.038), _ticks(0.044), _ticks(0.051),
                _ticks(0.060), _ticks(0.069), _ticks(0.080), _ticks(0.093),
                _ticks(0.108), _ticks(0.125), _ticks(0.145), _ticks(0.168),
                _ticks(0.195), _ticks(0.227), _ticks(0.263), _ticks(0.305),
                _ticks(0.354), _ticks(0.410), _ticks(0.476), _ticks(0.552),
                _ticks(0.641), _ticks(0.743), _ticks(0.862), _ticks(1.000)
        };
        return pgm_read_byte(&levels[bright * balance / 8]);
}
#else
uint8_t calculate_duration(uint8_t bright, uint8_t balance)
{
        static const PROGMEM uint8_t levels[] = {
                0, _ticks(0.012), _ticks(0.016), _ticks(0.021), _ticks(0.028),
                _ticks(0.038), _ticks(0.051), _ticks(0.069), _ticks(0.093),
                _ticks(0.125), _ticks(0.168), _ticks(0.227), _ticks(0.305),
                _ticks(0.410), _ticks(0.552), _ticks(0.743), _ticks(1.000)
        };
        return pgm_read_byte(&levels[bright]) * balance / DISPLAY_BRIGHT_MAX;
}
#endif

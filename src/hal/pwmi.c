#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "../display.h"
#include "pwmi.h"
#include "spi.h"
#include "asm.h"

/**
 * \file pwmi.c
 * \brief Модуль ШИМ индикации.
 * \details Этот модуль реализует программный ШИМ индикации,
 *  позволяющий регулировать яркость свечения дисплея и осуществлять
 *  эффект плавной смены старых показаний на новые.
 *
 *  Программный ШИМ для синхронизации использует прерывания при
 *  совпадении таймера 2. В общем случае цикл ШИМ может состоять из следующих
 *  фаз:
 *  - Показ старых данных. Длительность фазы непостоянна.
 *  - Показ новых данных. Длительность фазы непостоянна.
 *  - Индикатор погашен. Длительность фазы непостоянна.
 *  - Подготовка данных для следующего цикла, индикатор погашен. Имеет
 *    фиксированную длительность.
 *
 *  Важно знать, что длительность цикла неизменна и что цикл содержит любую
 *  комбинацию из первых трёх фаз и всегда содержит последнюю фазу.
 */

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
#define F_PWM 120

#if (PWM_CYCLE_DURATION - LAST_STEP_DURATION) > 255
#error PWM_CYCLE_DURATION - LAST_STEP_DURATION most be <= 255
#endif

#define BALANCE_MAX (F_PWM * 32)

/**
 * Вычисление количества тактов таймера для части цикла ШИМ.
 * \param d Длительность части цикла в диаппазоне от 0.0 до 1.0
 *      Поскольку это макрос, проверки допустимости нет.
 */
#define _ticks(d) ((uint8_t)((uint16_t)(PWM_CYCLE_DURATION - LAST_STEP_DURATION)* d))

#define _steps(r) ((BALANCE_MAX/F_PWM)/r)

#ifdef BRIGHT_SCHEME_POW
// Таблица степенных уровней яркости.
static prog_int8_t levels[] = {
                _ticks(0.000), _ticks(0.009), _ticks(0.013), _ticks(0.018),
                _ticks(0.025), _ticks(0.035), _ticks(0.048), _ticks(0.068),
                _ticks(0.095), _ticks(0.133), _ticks(0.186), _ticks(0.260),
                _ticks(0.364), _ticks(0.510), _ticks(0.714), _ticks(1.000)
};
#else
// Таблица линейных уровней яркости.
static prog_int8_t levels[] = {
                _ticks(0.000), _ticks(0.010), _ticks(0.081), _ticks(0.151),
                _ticks(0.222), _ticks(0.293), _ticks(0.364), _ticks(0.434),
                _ticks(0.505), _ticks(0.576), _ticks(0.646), _ticks(0.717),
                _ticks(0.788), _ticks(0.859), _ticks(0.929), _ticks(1.000)
};
#endif

static uint8_t bright; //! Яркость индикации.
static uint16_t rate; //! Скорость смены старых показаний на новые.
static uint8_t step = 0; //! Текущий шаг цикла ШИМа.
static uint8_t loaded = 0; //! Флаг загрузки значений.
static uint8_t tmp_array[SPI_ARRAY_SIZE];

/**
 * Инициализация ШИМа.
 */
void pwmi_init()
{
        // При тактовой частоте в 8 МГц, делителе = 256 и
        // PWM_CYCLE_DURATION = 260 получаем частоту ШИМ около 120 Гц.
        TCCR2 = 1 << WGM21 | 1 << CS22 | 1 << CS21;
        OCR2 = 64;
        TIMSK |= 1 << OCIE2;
}

/**
 * Установка яркости индикации.
 * \param lvl Яркость в диаппазоне от #DISPLAY_BRIGHT_MIN до #DISPLAY_BRIGHT_MAX
 *      включительно.
 */
void display_bright(uint8_t lvl)
{
        if (lvl > DISPLAY_BRIGHT_MAX) {
                lvl = DISPLAY_BRIGHT_MAX;
        } else if (lvl < DISPLAY_BRIGHT_MIN) {
                lvl = DISPLAY_BRIGHT_MIN;
        }
        bright = pgm_read_byte(&levels[lvl]);
}

/**
 * Установка скорости смены показаний индикатора со старых на новые.
 * \param lvl Скорость в диаппазоне от #DISPLAY_RATE_MIN до #DISPLAY_RATE_MAX
 *      включительно.
 */
void display_rate(uint8_t lvl)
{
        static prog_int16_t steps[] = {
                        _steps(1.0), _steps(0.83), _steps(0.69), _steps(0.58),
                        _steps(0.48), _steps(0.40), _steps(0.34), _steps(0.01)

        };

        if (lvl > DISPLAY_RATE_MAX) {
                lvl = DISPLAY_RATE_MAX;
        } else if (lvl < DISPLAY_RATE_MIN) {
                lvl = DISPLAY_RATE_MIN;
        }
        rate = pgm_read_word(&steps[lvl]);
}

/**
 * Загрузка данных для индикации.
 * @param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void pwmi_load(uint8_t data[])
{
        _cli;
        memcpy(tmp_array, data, SPI_ARRAY_SIZE);
        loaded = 1;
        _sei;
}

/**
 * Обработчик прерывания при совпадении в таймере 2.
 */
ISR(TIMER2_COMP_vect)
{
        /**
         * Баланс яркости нового и старого значения.
         */
        static uint16_t balance;
        static uint8_t old_duration, new_duration, hide_duration;
        static uint8_t old_value[SPI_ARRAY_SIZE], new_value[SPI_ARRAY_SIZE];
        uint8_t bal;

        switch (step) {
        case 0:
                step++;
                if (old_duration != 0) {
                        OCR2 = old_duration;
                        spi_send_array(old_value);
                        break;
                }
                /* no break */
        case 1:
                step++;
                if (new_duration != 0) {
                        OCR2 = new_duration;
                        spi_send_array(new_value);
                        break;
                }
                /* no break */
        case 2:
                step++;
                if (hide_duration != 0) {
                        OCR2 = hide_duration;
                        spi_clean();
                        break;
                }
                /* no break */
        case 3:
                step++;
                OCR2 = LAST_STEP_DURATION;
                spi_clean();
                if (loaded != 0) {
                        memcpy(old_value, new_value, SPI_ARRAY_SIZE);
                        memcpy(new_value, tmp_array, SPI_ARRAY_SIZE);
                        loaded = 0;
                        balance = 0;
                }

                if (balance < BALANCE_MAX) {
                        balance += rate;
                        if (balance > BALANCE_MAX) {
                                balance = BALANCE_MAX;
                        }
                }
                bal = (uint16_t) balance * DISPLAY_BRIGHT_MAX / BALANCE_MAX;

                new_duration = (uint16_t) pgm_read_byte(&levels[bal]) * bright
                                / (PWM_CYCLE_DURATION - LAST_STEP_DURATION);
                // Округление, длительность меньше двух нежелательна.
                new_duration &= 0xfe;
                bal = DISPLAY_BRIGHT_MAX - bal;
                old_duration = (uint16_t) pgm_read_byte(&levels[bal]) * bright
                                / (PWM_CYCLE_DURATION - LAST_STEP_DURATION);
                old_duration &= 0xfe;
                hide_duration = PWM_CYCLE_DURATION - LAST_STEP_DURATION
                                - new_duration - old_duration;
                step = 0;
        }
}

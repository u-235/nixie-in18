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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "pwmi.h"
#include "spi.h"
#include "asm.h"

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

/**
 * Вычисление количества тактов таймера для части цикла ШИМ.
 * \param d Длительность части цикла в диаппазоне от 0.0 до 1.0
 *      Поскольку это макрос, проверки допустимости нет.
 */
#define _ticks(d) ((PWM_CYCLE_DURATION - LAST_STEP_DURATION)* d)

static prog_uchar levels[] = {
                _ticks(0.000), _ticks(0.078), _ticks(0.093), _ticks(0.112),
                _ticks(0.135), _ticks(0.162), _ticks(0.194), _ticks(0.233),
                _ticks(0.279), _ticks(0.335), _ticks(0.402), _ticks(0.482),
                _ticks(0.579), _ticks(0.694), _ticks(0.833), _ticks(1.000)
};

static unsigned char bright; /// Яркость индикации.
static unsigned char rate; /// Скорость смены старых показаний на новые.
static unsigned char step = 0; /// Текущий шаг цикла ШИМа.
static unsigned char loaded = 0; /// Флаг загрузки значений.
/**
 * Баланс яркости нового и старого значения.
 */
static unsigned char balance;
static unsigned char old_duration, new_duration, hide_duration;
static unsigned char tmp_array[SPI_ARRAY_SIZE];
static unsigned char old_value[SPI_ARRAY_SIZE], new_value[SPI_ARRAY_SIZE];

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
        pwmi_bright(DISPLAY_BRIGHT_MAX);
        pwmi_rate(DISPLAY_RATE_MAX);
}

/**
 * Установка яркости индикации.
 * \param lvl Яркость в диаппазоне от #DISPLAY_BRIGHT_MIN до #DISPLAY_BRIGHT_MAX
 *      включительно.
 */
void pwmi_bright(unsigned char lvl)
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
 * \param lvl Скорость в диаппазоне от #DISPLAY_RATE_MIN до #DISPLAY_RATE_MAX
 *      включительно.
 */
void pwmi_rate(unsigned char lvl)
{
        if (lvl > DISPLAY_RATE_MAX) {
                lvl = DISPLAY_RATE_MAX;
        } else if (lvl < DISPLAY_RATE_MIN) {
                lvl = DISPLAY_RATE_MIN;
        }
        rate = F_PWM / DISPLAY_RATE_MAX * lvl + 1;
}

/**
 * Загрузка данных для индикации.
 * @param data Массив данных, состоящий из #SPI_ARRAY_SIZE элементов.
 */
void pwmi_load(unsigned char data[])
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
        unsigned char lvl, bal;

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

                if (balance < F_PWM) {
                        balance += rate;
                        if (balance > F_PWM) {
                                balance = F_PWM;
                        }
                }
                bal = balance * DISPLAY_BRIGHT_MAX / F_PWM;

                lvl = pgm_read_byte(&levels[bright]);
                new_duration = pgm_read_byte(&levels[bal]) * lvl
                                / (PWM_CYCLE_DURATION - LAST_STEP_DURATION);
                new_duration&=0xfe;
                bal = DISPLAY_BRIGHT_MAX - bal;
                old_duration = pgm_read_byte(&levels[bal]) * lvl
                                / (PWM_CYCLE_DURATION - LAST_STEP_DURATION);
                old_duration&=0xfe;
                hide_duration = PWM_CYCLE_DURATION - LAST_STEP_DURATION
                                - new_duration - old_duration;
                step = 0;
        }
}

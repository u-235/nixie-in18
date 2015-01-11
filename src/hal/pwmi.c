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

#define PWM_CYCLE_DURATION 256
#define LAST_STEP_DURATION 15

static prog_uchar levels[] = {
                0, 17, 20, 24, 29, 34, 41, 49, 59, 71, 85, 102, 123, 148, 177,
                212, 255
};

static unsigned char bright; /// Яркость индикации.
static unsigned char rate; /// Скорость смены старых показаний на новые.
static unsigned char step = 0; /// Текущий шаг цикла ШИМа.
static unsigned char tmp_bright;
static unsigned char tmp_array[SPI_ARRAY_SIZE];
/**
 * Баланс яркости нового и старого значения.
 */
static unsigned char balance;
static unsigned char old_duration, new_duration, hide_duration;
static unsigned char old_value[SPI_ARRAY_SIZE], new_value[SPI_ARRAY_SIZE];

/**
 * Инициализация ШИМа.
 */
void pwmi_init()
{
        TCCR2 = 1 << WGM21 | 1 << CS21 | 1 << CS20;
        OCR2 = 64;
        TIMSK |= 1 << OCIE2;
}

/**
 * Установка яркости индикации.
 * \param lvl Яркость в диаппазоне от #DISPLAY_BRIGHT_MIN до #DISPLAY_BRIGHT_MAX
 *      включительно.
 */
void pwmi_bright(unsigned char lvl)
{
        _cli;
        if (lvl > DISPLAY_BRIGHT_MAX) {
                lvl = DISPLAY_BRIGHT_MAX;
        } else if (lvl < DISPLAY_BRIGHT_MIN) {
                lvl = DISPLAY_BRIGHT_MIN;
        }
        lvl++;
        tmp_bright = lvl;
        _sei;
}

/**
 * Установка скорости смены показаний индикатора со старых на новые.
 * \param lvl Скорость в диаппазоне от #DISPLAY_RATE_MIN до #DISPLAY_RATE_MAX
 *      включительно.
 */
void pwmi_rate(unsigned char lvl)
{
        _cli;
        if (lvl > DISPLAY_RATE_MAX) {
                lvl = DISPLAY_RATE_MAX;
        } else if (lvl < DISPLAY_RATE_MIN) {
                lvl = DISPLAY_RATE_MIN;
        }
        rate = lvl;
        _sei;
}

void pwmi_load(unsigned char data[])
{
        _cli;
        memcpy(tmp_array, data, SPI_ARRAY_SIZE);
        spi_send_array(data);
        balance = DISPLAY_BRIGHT_MIN;
        _sei;
}

/**
 * Обработчик прерывания при совпадении в таймере 2.
 */
ISR(TIMER2_COMP_vect)
{
        unsigned char lvl;

        step++;

        switch (step) {
        case 1:
                if (old_duration != 0) {
                        OCR2 = old_duration;
                        spi_send_array(old_value);
                        break;
                }
                /* no break */
        case 2:
                if (new_duration != 0) {
                        OCR2 = new_duration;
                        spi_send_array(new_value);
                        break;
                }
                /* no break */
        case 3:
                if (hide_duration != 0) {
                        OCR2 = hide_duration;
                        spi_clean();
                        break;
                }
                /* no break */
        case 4:
                OCR2 = LAST_STEP_DURATION;
                spi_clean();
                bright = tmp_bright;
                memcpy(old_value, new_value, SPI_ARRAY_SIZE);
                memcpy(new_value, tmp_array, SPI_ARRAY_SIZE);

                if (balance <= DISPLAY_BRIGHT_MAX) {
                        balance += rate;
                        if (balance <= DISPLAY_BRIGHT_MAX) {
                                balance = DISPLAY_BRIGHT_MAX + 1;
                        }
                }
                lvl = pgm_read_byte(&levels[bright]);
                new_duration = pgm_read_byte(&levels[balance]) * lvl / 256;
                old_duration = pgm_read_byte(&levels[16 - balance]) * lvl / 256;
                new_duration = 64;
                old_duration = 64;
                hide_duration = PWM_CYCLE_DURATION - new_duration
                                - old_duration;
                step = 0;
        }
}

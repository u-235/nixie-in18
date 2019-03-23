/**
 * @file
 * @brief
 * @details
 *
 * @date создан 24.02.2019
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef _RTC_INSIDE
#error Dont include this file
#endif

#include "../rtc.h"

static rtc_corrector corr;

/*************************************************************
 *      Chip-specific function prototypes.
 *************************************************************/

/*
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
static int8_t chip_get_caliber(void);

/*
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного. Если RTC спешат,
 * то загружаемое значение должно быть меньше нуля.
 * @param clb Коррекция хода в ppm.
 */
static void chip_set_caliber(int8_t clb);

/*************************************************************
 *      Public function
 *************************************************************/

const rtc_corrector *rtc_get_corrector()
{
        return &corr;
}

void rtc_set_corrector(const rtc_corrector *_corr)
{
        corr.start = _corr->start;
        corr.last = _corr->last;
        corr.diff = _corr->diff;
        rtc_store_corrector(&corr);
}

/*
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
int8_t rtc_get_caliber()
{
        return chip_get_caliber();
}

/*
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного. Если RTC спешат,
 * то загружаемое значение должно быть меньше нуля.
 * @param clb Коррекция хода в ppm.
 */
void rtc_set_caliber(int8_t clb)
{
        chip_set_caliber(clb);
        rtc_reset_corrector();
}

int8_t rtc_limit_caliber(const int8_t clb)
{
        if (clb < CALIBER_LIMIT_BOTTOM)
                return CALIBER_LIMIT_BOTTOM;
        if (clb > CALIBER_LIMIT_TOP)
                return CALIBER_LIMIT_TOP;
        return clb;
}

/*
 * Получение отклонения хода RTC, вычисленного на основе разницы между
 * временем RTC и временем, переданным в rtc_set_time(). Если RTC отстаёт, то
 * возвращаемое значение меньше нуля.
 * @return Вычисленное отклонение хода в ppm.
 */
int8_t rtc_get_deviation()
{
        rtc_utime_t period;

        rtc_restore_corrector(&corr);
        period = corr.last - corr.start;
        if (period == 0)
                return 0;

        /* Максимальное абсолютное значение corr.diff равно 2147. Это значение
         * наберётся за 194 дня при максимальном абсолютном отклонении в
         * 128 ppm. При 4 ppm максимальный период составляет 6213 дней, это
         * около 17 лет. */
        return corr.diff * 1000000 / (period);
}

/*
 * Применение вычисленного отклонения хода RTC. Значение, полученное
 * rtc_get_deviation() вычитается из текущего значения коррекции и
 * записывается в RTC.
 */
void rtc_apply_corrector()
{
        rtc_utime_t period, current;

        chip_set_caliber(chip_get_caliber() - rtc_get_deviation());

        period = corr.last - corr.start;
        current = rtc_seconds_from_millenium(&local_time);
        if (period != 0) {
                corr.diff = corr.diff * (corr.last - current) / (period);
        } else {
                corr.diff = 0;
        }

        corr.start = corr.last;
        rtc_store_corrector(&corr);
}

/*
 * Сброс вычисленного отклонения хода RTC.
 */
void rtc_reset_corrector()
{
        corr.start = corr.last;
        corr.diff = 0;
        rtc_store_corrector(&corr);
}

/*************************************************************
 *      Private function.
 *************************************************************/

/*
 * Обновление вычисленного отклонения хода RTC в соответствии с новыми
 * значениями времени и даты.
 * @param _time Указатель на структуру с новым временем.
 */
void corrector_update(struct rtc_tm const *_time)
{
        if (!_rtc_is_actual(_time->actual))
                return;

        rtc_restore_corrector(&corr);
        corr.last = rtc_seconds_from_millenium(_time);
        if (_rtc_is_actual(local_time.actual)) {
                corr.diff += rtc_seconds_from_millenium(&local_time)
                                - corr.last;
        }
        rtc_store_corrector(&corr);
}

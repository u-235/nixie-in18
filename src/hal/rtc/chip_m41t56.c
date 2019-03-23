/**
 * \file
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#undef CFG_RTC_CHIP
#define CFG_RTC_CHIP    _RTC_M41T56

/* Адрес микросхемы на шине IIC. */
#define RTC_ADDRESS             0xd0
#define _is_power_false() (bits.out == 1 && bits.test == 0)
/* Количество регистров. */
#define NUMBER_OF_REGISTER      8
/* Адрес (индекс) счетчика секунд и бита остановки. */
#define ADDR_SECONDS    0
/* Адрес (индекс) счетчика дней недели. */
#define ADDR_WEEK_DAY   3
/* Адрес (индекс) первого байта RAM RTC. */
#define ADDR_RAM_BEGIN          8
/* Адрес (индекс) последнего байта RAM RTC. */
#define ADDR_RAM_END            63
/* Количество байт RAM RTC. */
#define RAM_SIZE                (ADDR_RAM_END - ADDR_RAM_BEGIN + 1)
/**/
#define NUMBER_OF_CELL  (NUMBER_OF_REGISTER + RAM_SIZE)
/* Микросхема RTC имеет корректор хода. */
#define CHIP_HAS_CORRECTOR
#define CALIBER_LIMIT_TOP       124
#define CALIBER_LIMIT_BOTTOM    -62

#define _RTC_INSIDE
#include "classic.h"
#include "corrector.h"

/* Маска для получения только секунд. */
#define MASK_SECONDS    ((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
/* Номер бита остановки в байте счетчика секунд. */
#define BIT_STOP        7

/* Адрес (индекс) счетчика минут. */
#define ADDR_MINUTES    1
/* Маска для получения только минут. */
#define MASK_MINUTES    ((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/* Адрес (индекс) счетчика часов. */
#define ADDR_HOURS      2
/* Маска для получения только часов. */
#define MASK_HOURS      ((1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/* Номер бита смены столетия в байте счетчика часов. */
#define BIT_CENTURY             6
/* Номер бита разрешения отслеживания смены столетия в байте счетчика. */
#define BIT_CENTURY_ENABLE      7

/* Маска для получения только дня недели. */
#define MASK_WEEK_DAY   ((1 << 2) |(1 << 1) | (1 << 0))

/* Адрес (индекс) счетчика дня месяца. */
#define ADDR_DAY        4
/* Маска для получения только дня месяца. */
#define MASK_DAY        ((1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/* Адрес (индекс) счетчика месяцев. */
#define ADDR_MONTH      5
/* Маска для получения только месяцев. */
#define MASK_MONTH       ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/* Адрес (индекс) счетчика лет. */
#define ADDR_YEAR       6
/* Маска для получения только лет. */
#define MASK_YEAR       0xff

/* Адрес (индекс) регистра управления */
#define ADDR_CONTROL            7
/* Номер бита остановки в байте регистра управления. */
#define BIT_TEST                6
/* Номер бита остановки в байте регистра управления. */
#define BIT_OUT                 7
/* Номер бита остановки в байте регистра управления. */
#define BIT_SIGN                5
/* Маска для получения только значения калибровки без знака. */
#define MASK_CALIBR             ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
/* Маска для получения только значения калибровки без знака. */
#define MASK_CALIBR_SIGN        (1 << BIT_SIGN)
/* Маска для получения значения калибровки со знаком. */
#define MASK_SIGNED_CALIBR      (MASK_CALIBR_SIGN | MASK_CALIBR)

static uint8_t caliber;

/*************************************************************
 *      Chip-specific function.
 *************************************************************/

/*
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
int8_t chip_get_caliber()
{
        int8_t ret = caliber & MASK_CALIBR;

        ret = (uint8_t) ret * 2;
        if ((caliber & MASK_CALIBR_SIGN) != 0) {
                ret = -ret;
        } else {
                ret = (uint8_t) ret * 2;
        }

        return ret;
}

/*
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного. Если RTC спешат,
 * то загружаемое значение должно быть меньше нуля.
 * @param clb Коррекция хода в ppm.
 */
void chip_set_caliber(int8_t clb)
{
        uint8_t i, s;

        if (clb < 0) {
                i = (uint8_t) (-clb + 1) / 2;
                s = MASK_CALIBR_SIGN;
        } else {
                i = (uint8_t) (clb + 2) / 4;
                s = 0;
        }

        caliber = s | (i & MASK_CALIBR);
        rtc_store_caliber(caliber);
        apply_control_bits();
}

void chip_init()
{
        caliber = rtc_restore_caliber();
}

void chip_reset()
{
        rtc_reset_corrector();
}

uint8_t pure_seconds(const uint8_t s)
{
        bits.stop = s & (1 << BIT_STOP);
        return from_bcd(s & MASK_SECONDS);
}

uint8_t complete_seconds(const uint8_t s)
{
        uint8_t ret;

        ret = to_bcd(s);
        if (bits.stop == 1) {
                ret |= (1 << BIT_STOP);
        }
        return ret;
}

uint8_t pure_minutes(const uint8_t m)
{
        return from_bcd(m & MASK_MINUTES);
}

uint8_t complete_minutes(const uint8_t m)
{
        return to_bcd(m);
}

uint8_t pure_hours(const uint8_t h)
{
        bits.centuryEnable = h & (1 << BIT_CENTURY_ENABLE);
        bits.century = h & (1 << BIT_CENTURY);
        return from_bcd(h & MASK_HOURS);
}

uint8_t complete_hours(const uint8_t h)
{
        uint8_t ret;

        ret = to_bcd(h);

        if (bits.century == 1) {
                ret |= (1 << BIT_CENTURY);
        }

        if (bits.centuryEnable == 1) {
                ret |= (1 << BIT_CENTURY_ENABLE);
        }

        return ret;
}

uint8_t pure_week_days(const uint8_t d)
{
        return d & MASK_WEEK_DAY;
}

uint8_t complete_week_days(const uint8_t d)
{
        return pure_week_days(d);
}

uint8_t pure_day(const uint8_t d)
{
        return from_bcd(d & MASK_DAY);
}

uint8_t complete_day(const uint8_t d)
{
        return to_bcd(d);
}

uint8_t pure_month(const uint8_t m)
{
        return from_bcd(m & MASK_MONTH);
}

uint8_t complete_month(const uint8_t m)
{
        return to_bcd(m);
}

uint8_t pure_year(const uint8_t y)
{
        return from_bcd(y & MASK_YEAR);
}

uint8_t complete_year(const uint8_t y)
{
        return to_bcd(y);
}

void parse_control(const uint8_t c)
{
        bits.out = 0;
        bits.test = 0;
        if (c & (1 << BIT_OUT))
                bits.out = 1;
        if (c & (1 << BIT_TEST))
                bits.test = 1;
        caliber = c & MASK_SIGNED_CALIBR;
}

/*
 * Подготовка байта управления.
 * @return Байт управления, готовый к отправке в микросхему.
 */
uint8_t complete_control()
{
        uint8_t ret;

        ret = caliber;
        if (bits.out == 1) {
                ret |= (1 << BIT_OUT);
        }
        if (bits.test == 1) {
                ret |= (1 << BIT_TEST);
        }
        return ret;
}

void apply_stop_bit()
{
        begin_io(IIC_WRITE, ADDR_SECONDS);
        iic_ll_write(complete_seconds(local_time.seconds));
        end_io();
}

void apply_century_bits()
{
        begin_io(IIC_WRITE, ADDR_HOURS);
        iic_ll_write(complete_hours(local_time.hours));
        end_io();
}

void apply_control_bits()
{
        begin_io(IIC_WRITE, ADDR_CONTROL);
        iic_ll_write(complete_control());
        end_io();
}

/**
 * \file
 * \brief Реализация RTC M41T56
 * \details
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

#ifndef __RTC_M41T56__
#define __RTC_M41T56__
#endif

#include <stdint.h>
#include "../rtc.h"
#include "../iic.h"

/**
 * Адрес микросхемы на шине IIC.
 */
#define RTC_ADDRESS 0xd0

/** Адрес (индекс) счетчика секунд и бита остановки. */
#define ADDR_SECONDS    0
/** Маска для получения только секунд. */
#define MASK_SECONDS    ((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
/** Номер бита остановки в байте счетчика секунд. */
#define BIT_STOP        7

/** Адрес (индекс) счетчика минут. */
#define ADDR_MINUTES    1
/** Маска для получения только минут. */
#define MASK_MINUTES    ((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/** Адрес (индекс) счетчика часов. */
#define ADDR_HOURS      2
/** Маска для получения только часов. */
#define MASK_HOURS      ((1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/**
 * \brief Номер бита смены столетия в байте счетчика часов.
 * \details Этот бит меняет своё состояние если бит #BIT_CENTURY_ENABLE
 *  установлен и счётчик лет переполнился.
 */
#define BIT_CENTURY             6
/** Номер бита разрешения отслеживания смены столетия в байте счетчика. */
#define BIT_CENTURY_ENABLE      7

/**
 * \brief Адрес (индекс) счетчика дней недели.
 * \details Счёт идёт от 1 до 7 включительно. Что означают эти цифры решает
 *  программист. Иными словами "1" может быть понедельником (как это принято в
 *  России), воскресеньем или любым другим днём недели.
 */
#define ADDR_DAY        3
/** Маска для получения только дня недели. */
#define MASK_DAY        ((1 << 1) | (1 << 0))

/**
 * \brief Адрес (индекс) счетчика дня месяца.
 * \details Счёт идёт от 1 до 31 включительно.
 */
#define ADDR_DATE       4
/** Маска для получения только дня месяца. */
#define MASK_DATE        ((1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/** Адрес (индекс) счетчика месяцев. */
#define ADDR_MONTH      5
/** Маска для получения только месяцев. */
#define MASK_MONTH       ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

/** Адрес (индекс) счетчика лет. */
#define ADDR_YEAR       6
/** Маска для получения только лет. */
#define MASK_YEAR       0xff

/** Адрес (индекс) регистра управления */
#define ADDR_CONTROL            7
/** Маска для получения только значения калибровки без знака. */
#define MASK_CALIBR             ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
/** Маска для получения только секунд. */
#define MASK_SIGNED_CALIBR      ((1 << 5) | MASK_CALIBR)
/** Номер бита остановки в байте счетчика. */
#define BIT_SIGN                5
/** Номер бита остановки в байте счетчика. */
#define BIT_TEST                6
/** Номер бита остановки в байте счетчика. */
#define BIT_OUT                 7

/** Адрес (индекс) первого байта RAM RTC. */
#define ADDR_RAM_BEGIN  8
/** Адрес (индекс) последнего байта RAM RTC. */
#define ADDR_RAM_END    55
/** Количество байт RAM RTC. */
#define ADDR_RAM_SIZE   (ADDR_RAM_END - ADDR_RAM_BEGIN + 1)

/*************************************************************
 *      Variable in RAM
 *************************************************************/

static rtc_error_t error;

/**
 * Копия управляющих битов микросхемы.
 */
static struct {
        uint8_t stop :1;
        uint8_t century :1;
        uint8_t centuryEnable :1;
        uint8_t test :1;
        uint8_t out :1;
} bits = {
        0, 0, 0, 0, 0
};

/*************************************************************
 *      Private function prototype.
 *************************************************************/

static void reset_chip();

static uint8_t pure_seconds(const uint8_t s);

static uint8_t complete_seconds(const uint8_t s);

static uint8_t pure_minutes(const uint8_t m);

static uint8_t complete_minutes(const uint8_t m);

static uint8_t pure_hours(const uint8_t h);

static uint8_t complete_hours(const uint8_t h);

static uint8_t pure_days(const uint8_t d);

static uint8_t complete_days(const uint8_t d);

static uint8_t pure_date(const uint8_t d);

static uint8_t complete_date(const uint8_t d);

static uint8_t pure_month(const uint8_t m);

static uint8_t complete_month(const uint8_t m);

static uint8_t pure_year(const uint8_t y);

static uint8_t complete_year(const uint8_t y);

static uint8_t pure_calibr(const uint8_t c);

static void set_error(rtc_error_t e);

static void check_chip();

static void begin_io(iic_mode_t mode, uint8_t addr);

static void end_io();

static uint8_t complete_control(const uint8_t caliber);

static void apply_stop_bit();

static void apply_century_bits();

static void apply_out_bits();

/*************************************************************
 *      Public function
 *************************************************************/

/**
 * \brief Инициализация микросхемы RTC.
 * \details Проверяется состояние микросхемы и если был сбой резервного питания,
 *  то устанавливается ошибка #RTC_POWER_ERROR.
 *  Функция не делает инициализации IIC.
 */
extern void rtc_init_m41t56()
{
        uint8_t sec;

        begin_io(IIC_READ, ADDR_SECONDS);
        sec = iic_ll_read(0);
        end_io();

        if (rtc_error() != RTC_NO_ERROR) {
                return;
        }

        /* Если биты OUT и TEST сброшены, то сбоев не было. */
        if ((sec & ((1 < BIT_OUT) | (1 < BIT_TEST))) == 0) {
                return;
        }

        set_error(RTC_POWER_ERROR);
}

extern rtc_error_t rtc_error_m41t56()
{
        return error;
}

extern void rtc_clear_m41t56()
{
        rtc_error_t err = rtc_error();
        set_error(RTC_NO_ERROR);

        switch (err) {
        case RTC_CHIP_ERROR:
                iic_clear();
                break;
        case RTC_POWER_ERROR:
                reset_chip();
                break;
        default:
                ;
        }
}

extern void rtc_start_m41t56()
{
        bits.stop = 0;
        apply_stop_bit();
}

extern void rtc_stop_m41t56()
{
        bits.stop = 1;
        apply_stop_bit();
}

extern uint8_t rtc_check_m41t56()
{
        uint8_t curr;
        static uint8_t old = 0;

        begin_io(IIC_READ, ADDR_SECONDS);
        curr = pure_seconds(iic_ll_read(0));
        end_io();

        if (rtc_error() != RTC_NO_ERROR) {
                return 0;
        }

        if (curr != old) {
                old = curr;
                return 1;
        }

        return 0;
}

extern void rtc_get_time_m41t56(bcd_time_t * time)
{
        begin_io(IIC_READ, ADDR_SECONDS);
        time->sec = pure_seconds(iic_ll_read(1));
        time->min = pure_minutes(iic_ll_read(1));
        time->hour = pure_hours(iic_ll_read(0));
        end_io();
}

extern void rtc_set_time_m41t56(const bcd_time_t * time)
{
        begin_io(IIC_WRITE, ADDR_SECONDS);
        iic_ll_write(complete_seconds(time->sec));
        iic_ll_write(complete_minutes(time->min));
        iic_ll_write(complete_hours(time->hour));
        end_io();
}

extern void rtc_get_date_m41t56(bcd_date_t * date)
{
        begin_io(IIC_READ, ADDR_DAY);
        date->day = pure_days(iic_ll_read(1));
        date->date = pure_date(iic_ll_read(1));
        date->month = pure_month(iic_ll_read(1));
        date->year = pure_year(iic_ll_read(0));
        end_io();
}

extern void rtc_set_date_m41t56(const bcd_date_t * date)
{
        begin_io(IIC_WRITE, ADDR_DAY);
        iic_ll_write(complete_days(date->day));
        iic_ll_write(complete_date(date->date));
        iic_ll_write(complete_month(date->month));
        iic_ll_write(complete_year(date->year));
        end_io();
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void reset_chip()
{
        char i;

        begin_io(IIC_WRITE, ADDR_SECONDS);
        iic_ll_write(complete_seconds(_bcd2_make(12)));
        iic_ll_write(complete_minutes(_bcd2_make(12)));
        iic_ll_write(complete_hours(_bcd2_make(12)));
        iic_ll_write(complete_days(WEDNESDAY));
        iic_ll_write(complete_date(_bcd2_make(12)));
        iic_ll_write(complete_month(_bcd2_make(12)));
        iic_ll_write(complete_year(_bcd2_make(18)));
        iic_ll_write(complete_control(0));
        /* Clean chip's RAM. */
        for (i = ADDR_RAM_SIZE; i > 0; i--) {
                iic_ll_write(0);
        }
        end_io();
}

static uint8_t pure_seconds(const uint8_t s)
{
        return s & MASK_SECONDS;
}

static uint8_t complete_seconds(const uint8_t s)
{
        uint8_t ret;

        ret = pure_seconds(s);
        if (bits.stop == 1) {
                ret |= (1 << BIT_STOP);
        }
        return ret;
}

static uint8_t pure_minutes(const uint8_t m)
{
        return m & MASK_MINUTES;
}

static uint8_t complete_minutes(const uint8_t m)
{
        return pure_minutes(m);
}

static uint8_t pure_hours(const uint8_t h)
{
        return h & MASK_HOURS;
}

static uint8_t complete_hours(const uint8_t h)
{
        uint8_t ret;

        ret = pure_hours(h);

        if (bits.century == 1) {
                ret |= (1 << BIT_CENTURY);
        }

        if (bits.centuryEnable == 1) {
                ret |= (1 << BIT_CENTURY_ENABLE);
        }

        return ret;
}

static uint8_t pure_days(const uint8_t d)
{
        return d & MASK_DAY;
}

static uint8_t complete_days(const uint8_t d)
{
        return pure_days(d);
}

static uint8_t pure_date(const uint8_t d)
{
        return d & MASK_DATE;
}

static uint8_t complete_date(const uint8_t d)
{
        return pure_date(d);
}

static uint8_t pure_month(const uint8_t m)
{
        return m & MASK_MONTH;
}

static uint8_t complete_month(const uint8_t m)
{
        return pure_month(m);
}

static uint8_t pure_year(const uint8_t y)
{
        return y & MASK_YEAR;
}

static uint8_t complete_year(const uint8_t y)
{
        return pure_year(y);
}

static uint8_t pure_calibr(const uint8_t c)
{
        return c & MASK_SIGNED_CALIBR;
}

/**
 * Подготовка байта управления.
 * @param caliber Калибровочное слово, пять младших бит - значение,
 *        шестой бит - знак.
 * @return Байт управления, готовый к отправке в микросхему.
 */
static uint8_t complete_control(const uint8_t caliber)
{
        uint8_t ret;

        ret = pure_calibr(caliber);
        if (bits.out == 1) {
                ret |= (1 << BIT_OUT);
        }
        if (bits.test == 1) {
                ret |= (1 << BIT_TEST);
        }
        return ret;
}

static void apply_stop_bit()
{
        iic_set_address(RTC_ADDRESS);
        iic_cmd_write(ADDR_SECONDS,
                        complete_seconds(iic_cmd_read(ADDR_SECONDS)));
        check_chip();
}

static void apply_century_bits()
{
        iic_set_address(RTC_ADDRESS);
        iic_cmd_write(ADDR_HOURS, complete_hours(iic_cmd_read(ADDR_HOURS)));
        check_chip();
}

static void apply_out_bits()
{
        iic_set_address(RTC_ADDRESS);
        iic_cmd_write(ADDR_CONTROL,
                        complete_control(iic_cmd_read(ADDR_CONTROL)));
        check_chip();
}

static void set_error(rtc_error_t e)
{
        error = e;
}

/**
 * Проверка успешности обмена с микросхемой RTC. При сбое устанавливается
 * ошибка.
 */
static void check_chip()
{
        if (iic_error() == IIC_NO_ERROR) {
                return;
        }

        set_error(RTC_CHIP_ERROR);
}

/**
 * Подготовка к обмену с микросхемой RTC.
 * \param mode Тип обмена: запись (#IIC_WRITE) или чтение (#IIC_READ).
 * \param addr адрес регистра или ячейки памяти микросхемы.
 */
static void begin_io(iic_mode_t mode, uint8_t addr)
{
        iic_set_address(RTC_ADDRESS);
        iic_ll_start(IIC_WRITE);
        iic_ll_write(addr);
        /* Вообще, можно сделать iic_ll_start(mode) без проверки. */
        if (mode != IIC_WRITE) {
                iic_ll_start(IIC_READ);
        }
}

/**
 * Завершение обмена с микросхемой RTC.
 */
static void end_io()
{
        iic_ll_stop();
        check_chip();
}

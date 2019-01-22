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
#define ADDR_WEEK_DAY   3
/** Маска для получения только дня недели. */
#define MASK_WEEK_DAY   ((1 << 2) |(1 << 1) | (1 << 0))

/**
 * \brief Адрес (индекс) счетчика дня месяца.
 * \details Счёт идёт от 1 до 31 включительно.
 */
#define ADDR_DAY        4
/** Маска для получения только дня месяца. */
#define MASK_DAY        ((1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

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
/** Номер бита остановки в байте регистра управления. */
#define BIT_SIGN                5
/** Номер бита остановки в байте регистра управления. */
#define BIT_TEST                6
/** Номер бита остановки в байте регистра управления. */
#define BIT_OUT                 7
/** Маска для получения только значения калибровки без знака. */
#define MASK_CALIBR             ((1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
/** Маска для получения только значения калибровки без знака. */
#define MASK_CALIBR_SIGN        (1 << BIT_SIGN)
/** Маска для получения значения калибровки со знаком. */
#define MASK_SIGNED_CALIBR      (MASK_CALIBR_SIGN | MASK_CALIBR)

/** Адрес (индекс) первого байта RAM RTC. */
#define ADDR_RAM_BEGIN  8
/** Адрес (индекс) последнего байта RAM RTC. */
#define ADDR_RAM_END    63
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

static void check_power_fail();

static void reset_chip();

static uint8_t to_bcd(const uint8_t i);

static uint8_t from_bcd(const uint8_t b);

static uint8_t pure_seconds(const uint8_t s);

static uint8_t complete_seconds(const uint8_t s);

static uint8_t pure_minutes(const uint8_t m);

static uint8_t complete_minutes(const uint8_t m);

static uint8_t pure_hours(const uint8_t h);

static uint8_t complete_hours(const uint8_t h);

static uint8_t pure_week_days(const uint8_t d);

static uint8_t complete_week_days(const uint8_t d);

static uint8_t pure_day(const uint8_t d);

static uint8_t complete_day(const uint8_t d);

static uint8_t pure_month(const uint8_t m);

static uint8_t complete_month(const uint8_t m);

static uint8_t pure_year(const uint8_t y);

static uint8_t complete_year(const uint8_t y);

static uint8_t pure_calibr(const uint8_t c);

static uint8_t complete_control(const uint8_t caliber);

static void apply_stop_bit();

static void apply_century_bits();

static void apply_out_bits();

static void set_error(rtc_error_t e);

static void check_chip();

static void begin_io(iic_mode_t mode, uint8_t addr);

static void end_io();

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
        begin_io(IIC_READ, ADDR_SECONDS);
        iic_ll_read(0);
        end_io();

        if (rtc_error() != RTC_NO_ERROR) {
                return;
        }

        check_power_fail();
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

        if (rtc_error() == RTC_CHIP_ERROR) {
                return 0;
        }

        check_power_fail();

        if (curr != old) {
                old = curr;
                return 1;
        }

        return 0;
}

extern void rtc_get_time_m41t56(stime * time)
{
        begin_io(IIC_READ, ADDR_SECONDS);
        time->seconds = pure_seconds(iic_ll_read(1));
        time->minutes = pure_minutes(iic_ll_read(1));
        time->hours = pure_hours(iic_ll_read(0));
        end_io();
}

extern void rtc_set_time_m41t56(const stime * time)
{
        begin_io(IIC_WRITE, ADDR_SECONDS);
        iic_ll_write(complete_seconds(time->seconds));
        iic_ll_write(complete_minutes(time->minutes));
        iic_ll_write(complete_hours(time->hours));
        end_io();
}

extern void rtc_get_date_m41t56(sdate * date)
{
        begin_io(IIC_READ, ADDR_WEEK_DAY);
        date->week_day = pure_week_days(iic_ll_read(1));
        date->day = pure_day(iic_ll_read(1));
        date->month = pure_month(iic_ll_read(1));
        date->year = pure_year(iic_ll_read(0));
        end_io();
}

extern void rtc_set_date_m41t56(const sdate * date)
{
        begin_io(IIC_WRITE, ADDR_WEEK_DAY);
        iic_ll_write(complete_week_days(date->week_day));
        iic_ll_write(complete_day(date->day));
        iic_ll_write(complete_month(date->month));
        iic_ll_write(complete_year(date->year));
        end_io();
}

/**
 * Чтение массива данных из RTC.
 * @param dst Указатель на буфер для приёма данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для чтения.
 */
void rtc_mem_read_m41t56(void *dst, const uint8_t adr, const uint8_t sz)
{
        uint8_t c = sz;
        uint8_t *ptr = dst;
        begin_io(IIC_READ, ADDR_RAM_BEGIN + adr);
        while (c != 0) {
                *ptr++ = iic_ll_read(--c);
        }
        end_io();
}

/**
 * Запись массива данных в RTC.
 * @param src Указатель на буфер для передачи данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для записи.
 */
void rtc_mem_write_m41t56(const void *src, const uint8_t adr, const uint8_t sz)
{
        uint8_t c = sz;
        const uint8_t *ptr = src;
        begin_io(IIC_WRITE, ADDR_RAM_BEGIN + adr);
        while (c != 0) {
                c--;
                iic_ll_write(*ptr++);
        }
        end_io();
}

/**
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
extern int8_t rtc_get_caliber()
{
        int8_t tmp, ret;
        begin_io(IIC_READ, ADDR_CONTROL);
        tmp = iic_ll_read(0);
        end_io();

        ret = tmp & MASK_CALIBR;

        if ((tmp & MASK_CALIBR_SIGN) != 0) {
                ret = -ret;
                ret *= 2;
        } else {
                ret *= 4;
        }

        return ret;
}

/**
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного.
 * @param clb Коррекция хода в ppm.
 */
extern void rtc_set_caliber(int8_t clb)
{
        uint8_t tmp;
        if (clb < 0) {
                tmp = (-clb - 1) / 2;
                tmp |= MASK_CALIBR_SIGN;
        } else {
                tmp = (uint8_t) (clb + 2) / 4;
        }

        begin_io(IIC_WRITE, ADDR_CONTROL);
        iic_ll_write(complete_control(tmp));
        end_io();
}

/*************************************************************
 *      Private function.
 *************************************************************/

static void check_power_fail()
{
        uint8_t ctrl;
        begin_io(IIC_READ, ADDR_CONTROL);
        ctrl = iic_ll_read(0);
        end_io();

        if (rtc_error() == RTC_CHIP_ERROR) {
                return;
        }

        /* При сбое питания RTC бит OUT установлен, а бит TEST (FT) сброшен. */
        if ((ctrl & ((1 << BIT_OUT) | (1 << BIT_TEST)))
                        == ((1 << BIT_OUT) | (0 << BIT_TEST))) {
                set_error(RTC_POWER_ERROR);
        }
}

static void reset_chip()
{
        char i;

        begin_io(IIC_WRITE, 0);
        for (i = ADDR_RAM_END + 1; i > 0; i--) {
                iic_ll_write(0);
        }
        end_io();
}

static uint8_t to_bcd(const uint8_t i)
{
        return ((i / 10) * 16) | (i % 10);
}

static uint8_t from_bcd(const uint8_t b)
{
        return ((b / 16) * 10) + (b & 0x0f);
}

static uint8_t pure_seconds(const uint8_t s)
{
        return from_bcd(s & MASK_SECONDS);
}

static uint8_t complete_seconds(const uint8_t s)
{
        uint8_t ret;

        ret = to_bcd(s);
        if (bits.stop == 1) {
                ret |= (1 << BIT_STOP);
        }
        return ret;
}

static uint8_t pure_minutes(const uint8_t m)
{
        return from_bcd(m & MASK_MINUTES);
}

static uint8_t complete_minutes(const uint8_t m)
{
        return to_bcd(m);
}

static uint8_t pure_hours(const uint8_t h)
{
        return from_bcd(h & MASK_HOURS);
}

static uint8_t complete_hours(const uint8_t h)
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

static uint8_t pure_week_days(const uint8_t d)
{
        return d & MASK_WEEK_DAY;
}

static uint8_t complete_week_days(const uint8_t d)
{
        return pure_week_days(d);
}

static uint8_t pure_day(const uint8_t d)
{
        return from_bcd(d & MASK_DAY);
}

static uint8_t complete_day(const uint8_t d)
{
        return to_bcd(d);
}

static uint8_t pure_month(const uint8_t m)
{
        return from_bcd(m & MASK_MONTH);
}

static uint8_t complete_month(const uint8_t m)
{
        return to_bcd(m);
}

static uint8_t pure_year(const uint8_t y)
{
        return from_bcd(y & MASK_YEAR);
}

static uint8_t complete_year(const uint8_t y)
{
        return to_bcd(y);
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

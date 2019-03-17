/**
 * @file
 * @brief
 * @details
 *
 * @date создан 23.02.2019
 * @author Nick Egorrov
 */

#ifndef _RTC_INSIDE
#error Dont include this file
#endif

#include <string.h>

#include "../rtc.h"
#include "../iic.h"

static rtc_tm local_time;
static rtc_error_t error;

/*
 * Копия управляющих битов микросхемы.
 */
static union {
        uint8_t all;
        struct {
                uint8_t stop :1;
                uint8_t century :1;
                uint8_t centuryEnable :1;
                uint8_t test :1;
                uint8_t out :1;
        };
} bits;

/*************************************************************
 *      Chip-specific function prototypes.
 *************************************************************/

static void chip_init(void);

static void chip_reset(void);

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

static void parse_control(const uint8_t c);

static uint8_t complete_control(void);

static void apply_stop_bit(void);

static void apply_century_bits(void);

static void apply_control_bits(void);

static void corrector_update(rtc_tm const *_time);

/*************************************************************
 *      Private function prototypes.
 *************************************************************/

static void reset(void);

static uint8_t to_bcd(const uint8_t i);

static uint8_t from_bcd(const uint8_t b);

static void set_error(rtc_error_t e);

static void check(void);

static void begin_io(iic_mode_t mode, uint8_t addr);

static void end_io(void);

/*************************************************************
 *      Public function
 *************************************************************/

/*
 * \brief Инициализация микросхемы RTC.
 * \details Проверяется состояние микросхемы и если был сбой резервного питания,
 *  то устанавливается ошибка #RTC_POWER_ERROR.
 *  Функция не делает инициализации IIC.
 */
void rtc_init()
{
        rtc_sync();
        if (bits.stop == 1) {
                local_time.actual = 0;
                rtc_store_actuals(local_time.actual);
        }
        chip_init();
}

rtc_error_t rtc_error()
{
        return error;
}

void rtc_clear()
{
        rtc_error_t err = rtc_error();
        set_error(RTC_NO_ERROR);

        switch (err) {
        case RTC_CHIP_ERROR:
                iic_clear();
                break;
        case RTC_POWER_ERROR:
                reset();
                break;
        default:
                ;
        }
}

void rtc_start()
{
        bits.stop = 0;
        apply_stop_bit();
}

void rtc_stop()
{
        bits.stop = 1;
        apply_stop_bit();
}

uint8_t rtc_check()
{
        uint8_t curr;
        static uint8_t old = 0;

        begin_io(IIC_READ, ADDR_SECONDS);
        curr = pure_seconds(iic_ll_read(0));
        end_io();

        if (rtc_error() == RTC_CHIP_ERROR) {
                return 0;
        }

        if (curr != old) {
                old = curr;
                return 1;
        }

        return 0;
}

const rtc_tm * rtc_get_time()
{
        return &local_time;
}

void rtc_sync()
{
        begin_io(IIC_READ, ADDR_SECONDS);
        local_time.seconds = pure_seconds(iic_ll_read(1));
        local_time.minutes = pure_minutes(iic_ll_read(1));
        local_time.hours = pure_hours(iic_ll_read(1));
        local_time.week_day = pure_week_days(iic_ll_read(1) - 1);
        local_time.day = pure_day(iic_ll_read(1));
        local_time.month = pure_month(iic_ll_read(1));
        local_time.year = pure_year(iic_ll_read(1));
        parse_control(iic_ll_read(0));
        end_io();
}

void rtc_set_time(const rtc_tm * _time)
{
        begin_io(IIC_WRITE, ADDR_SECONDS);
        iic_ll_write(complete_seconds(_time->seconds));
        iic_ll_write(complete_minutes(_time->minutes));
        iic_ll_write(complete_hours(_time->hours));
        end_io();

        if (rtc_error() != RTC_NO_ERROR)
                return;

#if defined CHIP_HAS_CORRECTOR && !defined CFG_RTC_NOT_USE_CORRECTOR
        corrector_update(_time);
#endif

        local_time.seconds = _time->seconds;
        local_time.minutes = _time->minutes;
        local_time.hours = _time->hours;
        local_time.actual |= _time->actual & RTC_BITS_ACTUAL_TIME;

        rtc_store_actuals(local_time.actual);
}

void rtc_set_date(const rtc_tm * _time)
{
        begin_io(IIC_WRITE, ADDR_WEEK_DAY);
        iic_ll_write(complete_week_days(_time->week_day + 1));
        iic_ll_write(complete_day(_time->day));
        iic_ll_write(complete_month(_time->month));
        iic_ll_write(complete_year(_time->year));
        end_io();

        if (rtc_error() != RTC_NO_ERROR)
                return;

#if defined CHIP_HAS_CORRECTOR && !defined CFG_RTC_NOT_USE_CORRECTOR
        corrector_update(_time);
#endif

        local_time.week_day = _time->week_day;
        local_time.day = _time->day;
        local_time.month = _time->month;
        local_time.year = _time->year;
        local_time.actual |= _time->actual
                        & (RTC_BITS_ACTUAL_DATE | RTC_BIT_ACTUAL_WDAY);

        rtc_store_actuals(local_time.actual);
}

#ifndef RAM_SIZE
#define RAM_SIZE 0
#endif

#if RAM_SIZE > 0

static void clean_ram(void);

/*
 * Чтение массива данных из RTC.
 * @param dst Указатель на буфер для приёма данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для чтения.
 */
void rtc_mem_read(void *dst, const uint8_t adr, const uint8_t sz)
{
        uint8_t c = sz;
        uint8_t *ptr = dst;
        begin_io(IIC_READ, ADDR_RAM_BEGIN + adr);
        while (c != 0) {
                *ptr++ = iic_ll_read(--c);
        }
        end_io();
}

/*
 * Запись массива данных в RTC.
 * @param src Указатель на буфер для передачи данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для записи.
 */
void rtc_mem_write(const void *src, const uint8_t adr, const uint8_t sz)
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

void clean_ram()
{
        char i;

        begin_io(IIC_WRITE, ADDR_RAM_BEGIN);
        for (i = RAM_SIZE; i > 0; i--) {
                iic_ll_write(0);
        }
        end_io();
}

#else

void rtc_mem_read(void *dst, const uint8_t adr, const uint8_t sz)
{
        set_error(RTC_NOT_IMPLEMENTED);
}

void rtc_mem_write(const void *src, const uint8_t adr, const uint8_t sz)
{
        set_error(RTC_NOT_IMPLEMENTED);
}

#endif /* CHIP_HAS_RAM */

/*************************************************************
 *      Private function.
 *************************************************************/

void reset()
{
        memset(&local_time, 0, sizeof(rtc_tm));
        bits.all = 0;
        rtc_set_time(&local_time);
        rtc_set_date(&local_time);
        apply_control_bits();
#if RAM_SIZE > 0
        clean_ram();
#endif
        chip_reset();
}

uint8_t to_bcd(const uint8_t i)
{
        return ((i / 10) * 16) | (i % 10);
}

uint8_t from_bcd(const uint8_t b)
{
        return ((b / 16) * 10) + (b & 0x0f);
}

void set_error(rtc_error_t e)
{
        error = e;
}

/*
 * Проверка успешности обмена с микросхемой RTC. При сбое устанавливается
 * ошибка.
 */
void check()
{
        if (iic_error() != IIC_NO_ERROR) {
                set_error(RTC_CHIP_ERROR);
                local_time.actual = 0;
        } else if (_is_power_false()) {
                local_time.actual = 0;
                rtc_store_actuals(local_time.actual);
                set_error(RTC_POWER_ERROR);
        } else {
                local_time.actual = rtc_restore_actuals();
        }

}

/*
 * Подготовка к обмену с микросхемой RTC.
 * @param mode Тип обмена: запись (#IIC_WRITE) или чтение (#IIC_READ).
 * @param addr адрес регистра или ячейки памяти микросхемы.
 */
void begin_io(iic_mode_t mode, uint8_t addr)
{
        iic_set_address(RTC_ADDRESS);
        iic_ll_start(IIC_WRITE);
        iic_ll_write(addr);
        /* Вообще, можно сделать iic_ll_start(mode) без проверки. */
        if (mode != IIC_WRITE) {
                iic_ll_start(IIC_READ);
        }
}

/*
 * Завершение обмена с микросхемой RTC.
 */
void end_io()
{
        iic_ll_stop();
        check();
}

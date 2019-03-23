/***************************************************************************//**
 @file
 @date создан: 26 окт. 2015 г.
 @author Nick Egorrov

 @defgroup RTC <rtc.h> Модуль RTC
 @{
 ******************************************************************************/

#ifndef HAL_RTC_H_
#define HAL_RTC_H_

#include <stdint.h>
#include <time.h>

#include "../config.h"

/**
 * @brief Представление времени суток и календарной даты.
 * @details В отличие от структуры tm из <time.h>, поля этой структуры имеют тип
 * uint8_t.
 * @attention Поле месяца имеет диапазон от 1 до 12, поэтому перечисление
 * _MONTHS_ для этого поля не применимо.
 * @attention Год отсчитывается от 2000, а не от 1900.
 *
 * Кроме того, здесь нет полей летнего времени и количества дней с начала года.
 * Зато есть флаги rtc_tm::actual, которые показывают, достоверны ли данные этой
 * структуры. Это связано с тем, что микросхема RTC может содержать неправильное
 * время из-за сбоев питания или может быть недоступна из-за неисправностей.
 */
struct rtc_tm {
        /** Секунды, 0-59 */
        uint8_t seconds;
        /** Минуты, 0-59 */
        uint8_t minutes;
        /** Часы, 0-23 */
        uint8_t hours;
        /** День недели, от 0 до 6. 0 - воскресенье, 1 -понедельник. */
        uint8_t week_day;
        /** День месяца, 1-31 */
        uint8_t day;
        /** Месяц, 1-12 */
        uint8_t month;
        /** Год, 0-99 */
        uint8_t year;
        /** Флаги актуальности данных. */
        uint8_t actual;
};

#define RTC_BIT_ACTUAL_SECONDS  (1 << 0)
#define RTC_BIT_ACTUAL_MINUTES  (1 << 1)
#define RTC_BIT_ACTUAL_HOURS    (1 << 2)
#define RTC_BIT_ACTUAL_WDAY     (1 << 3)
#define RTC_BIT_ACTUAL_DAY      (1 << 4)
#define RTC_BIT_ACTUAL_MONTH    (1 << 5)
#define RTC_BIT_ACTUAL_YEAR     (1 << 6)
#define RTC_BITS_ACTUAL_TIME    (RTC_BIT_ACTUAL_SECONDS | RTC_BIT_ACTUAL_MINUTES | RTC_BIT_ACTUAL_HOURS)
#define RTC_BITS_ACTUAL_DATE    (RTC_BIT_ACTUAL_DAY | RTC_BIT_ACTUAL_MONTH | RTC_BIT_ACTUAL_YEAR)
#define _rtc_is_time_actual(fa) ((fa & RTC_BITS_ACTUAL_TIME) == RTC_BITS_ACTUAL_TIME)
#define _rtc_is_date_actual(fa) ((fa & RTC_BITS_ACTUAL_DATE) == RTC_BITS_ACTUAL_DATE)
#define _rtc_is_actual(fa)\
        ((fa & (RTC_BITS_ACTUAL_TIME | RTC_BITS_ACTUAL_DATE)) ==\
                        (RTC_BITS_ACTUAL_TIME | RTC_BITS_ACTUAL_DATE))

typedef uint32_t rtc_utime_t;
typedef int32_t rtc_time_t;

/**
 * Перечисление ошибок RTC.
 */
enum rtc_errors{
        /** Нет ошибок. */
        RTC_NO_ERROR,
        /** Неизвестная ошибка. */
        RTC_UNKNOWN_ERROR,
        /** Функция не реализована или неподдерживаемый режим. */
        RTC_NOT_IMPLEMENTED,
        /** Ошибка микросхемы RTC */
        RTC_CHIP_ERROR,
        /** Сбой питания микросхемы RTC */
        RTC_POWER_ERROR
};


#define _RTC_DC1307 1
#define _RTC_M41T56 2

#ifndef CFG_RTC_CHIP
#       error "rtc chip not defined"
#elif CFG_RTC_CHIP == _RTC_DC1307
#       include "rtc/chip_dc1307.h"
#elif CFG_RTC_CHIP == _RTC_M41T56
#       include "rtc/chip_m41t56.h"
#else
#       error "unknown rtc chip"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Инициализация RTC.
 */
extern void rtc_init(void);

/**
 * Получение ошибки RTC.
 * @return ошибка RTC.
 */
extern uint8_t rtc_error(void);

/**
 * Сброс ошибки RTC, если это возможно.
 */
extern void rtc_clear(void);

/**
 * Запуск счёта времени.
 */
extern void rtc_start(void);

/**
 * Останов счёта времени.
 */
extern void rtc_stop(void);

/**
 * @brief Проверка обновления RTC.
 * @details Функция проверяет, было ли обновлено состояние счётчиков RTC с
 * момента последнего вызова этой функции. После обнаружения обновления следует
 * вызвать rtc_sync() для синхронизации внутренней структуры rtc_tm.
 * @return Ноль если обновления не было.
 */
extern uint8_t rtc_check(void);

/**
 * @brief Синхронизация с микросхемой RTC.
 * @details Внутренняя структура rtc_tm синхронизируется с микросхемой. При
 * неудачном чтении или при обнаружении сбоя поле rtc_tm::actual очищается, что
 * сигнализирует о недостоверных данных этой структуры. Синхронизацию удобно
 * делать после обнаружения обновления времени RTC функцией rtc_check().
 */
extern void rtc_sync(void);

/**
 * @brief Получение указателя на структуру rtc_tm.
 * @details Модуль содержит служебную rtc_tm, которая используется для хранения
 * текущего времени. Это время обновляется функцией rtc_sync().
 * @return Указатель на внутреннюю структуру rtc_tm.
 */
extern const struct rtc_tm * rtc_get_time(void);

/**
 * @brief Установка времени.
 * @details Данные из _time записываются в микросхему RTC без проверки и вне
 * зависимости от флагов достоверности данных, однако эти флаги копируются в
 * флаги внутренней структуры.
 * @attention Это единственный способ изменить состояние флагов достоверности
 * внутренней структуры.
 * @param _time Указатель на структуру с новым временем.
 */
extern void rtc_set_time(const struct rtc_tm * _time);

/**
 * @brief Установка даты.
 * @details Данные из _time записываются в микросхему RTC без проверки и вне
 * зависимости от флагов достоверности данных, однако эти флаги копируются в
 * флаги внутренней структуры.
 * @attention Это единственный способ изменить состояние флагов достоверности
 * внутренней структуры.
 * @param _time Указатель на структуру с новой датой.
 */
extern void rtc_set_date(const struct rtc_tm * _time);

/**
 * Чтение массива данных из RTC.
 * @param dst Указатель на буфер для приёма данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для чтения.
 */
extern void rtc_mem_read(void *dst, const uint8_t adr, const uint8_t sz);

/**
 * Запись массива данных в RTC.
 * @param src Указатель на буфер для передачи данных.
 * @param adr Адрес данных в памяти RTC.
 * @param sz Количество байт для записи.
 */
extern void rtc_mem_write(const void *src, const uint8_t adr, const uint8_t sz);

/** @} */

/***************************************************************************//**
 @defgroup RTC_SERVICE Дополнительные функции
 @ingroup RTC
 @{
 ******************************************************************************/

/**
 * Копирование структуры rtc_tm.
 * @param dst Указатель на заполняемую структуру.
 * @param src Указатель на исходную структуру.
 */
extern void rtc_copy(struct rtc_tm *dst, struct rtc_tm const *src);

/**
 * Конвертирует структуру rtc_tm в стандартную tm.
 * @param dst Указатель на заполняемую структуру.
 * @param src Указатель на исходную структуру.
 */
extern void rtc_to_tm(struct tm *dst, struct rtc_tm const *src);

extern void rtc_date_adjust(struct rtc_tm *date);

extern uint_fast16_t rtc_day_in_year(const struct rtc_tm *date);

extern rtc_utime_t rtc_day_in_millenium(const struct rtc_tm *date);

extern rtc_utime_t rtc_seconds_from_midnight(const struct rtc_tm *time);

extern rtc_utime_t rtc_seconds_from_millenium(const struct rtc_tm *time);

/** @} */

/***************************************************************************//**
 @defgroup RTC_CORRECTOR Коррекция хода
 @ingroup RTC
 @{
 ******************************************************************************/

typedef struct {
        rtc_utime_t start;
        rtc_utime_t last;
        rtc_time_t diff;
} rtc_corrector;

extern const rtc_corrector *rtc_get_corrector(void);

extern void rtc_set_corrector(const rtc_corrector *corr);

/**
 * Получение значения коррекции хода.
 * @return Коррекция хода в ppm.
 */
extern int8_t rtc_get_caliber(void);

/**
 * Запись коррекции хода. Поскольку коррекция не равна в точности одному ppm,
 * реально записанное значение может отличаться от заданного. Если RTC спешит,
 * то загружаемое значение должно быть меньше нуля.
 * @param clb Коррекция хода в ppm.
 */
extern void rtc_set_caliber(int8_t clb);

/**
 * @brief Ограничение величины калибровки.
 * @details Если clb в допустимых границах, то оно возвращается без изменений.
 * Иначе возвращается значение нижнего или верхнего предела для отрицательных
 * или положительных значений clb соответственно.
 * @param clb Значение калибровки.
 * @return Величина калибровки, не выходящая за пределы допустимых значений.
 */
extern int8_t rtc_limit_caliber(const int8_t clb);

/**
 * Получение отклонения хода RTC, вычисленного на основе разницы между
 * временем RTC и временем, переданным в rtc_set_time(). Если RTC отстаёт, то
 * возвращаемое значение меньше нуля.
 * @return Вычисленное отклонение хода в ppm.
 */
extern int8_t rtc_get_deviation(void);

/**
 * Применение вычисленного отклонения хода RTC. Значение, полученное
 * rtc_diff() вычитается из текущего значения коррекции и
 * записывается в RTC.
 */
extern void rtc_apply_corrector(void);

/**
 * Сброс вычисленного отклонения хода RTC.
 */
extern void rtc_reset_corrector(void);

/** @} */

/***************************************************************************//**
 @defgroup RTC_IMPORT Импортируемые функции
 @ingroup RTC
 @anchor store
 @par Сохранение флагов актуальности данных

 @code
 #include <avr/eeprom.h>
 #include "rtc.h"

 EEMEM static uint8_t eeprom_actual = 0;

 uint8_t rtc_restore_actuals()
 {
          return eeprom_read_byte(&eeprom_actual);
 }

 void rtc_store_actuals(uint8_t ac)
 {
          eeprom_write_byte(&eeprom_actual, ac);
 }
 @endcode
 @{
 ******************************************************************************/

/**
 * @brief Сохранение флагов структуры rtc_tm.
 * @details Эта функция используется для сохранения флагов достоверности данных
 * и должна быть реализована в пользовательском коде. При изменении флагов
 * служебной структуры, модуль RTC при помощи этой функции сохраняет эти флаги в
 * энергонезависимой памяти (например, EEPROM). \ref store
 * @param ac Сохраняемое значение.
 */
extern void rtc_store_actuals(uint8_t ac);

/**
 * @brief Восстановление флагов структуры rtc_tm.
 * @details Эта функция используется для восстановления флагов достоверности
 * данных и должна быть реализована в пользовательском коде.
 * @return Восстановленное значение.
 * @see rtc_store_actuals()
 */
extern uint8_t rtc_restore_actuals(void);

extern void rtc_store_caliber(uint8_t cl);

extern uint8_t rtc_restore_caliber(void);

extern void rtc_store_corrector(rtc_corrector *corr);

extern void rtc_restore_corrector(rtc_corrector *corr);

/** @} */

#ifdef __cplusplus
}
#endif

/***************************************************************************//**
 @defgroup RTC_CHIPS Микросхемы RTC
 @ingroup RTC
 ******************************************************************************/

#endif /* HAL_RTC_H_ */

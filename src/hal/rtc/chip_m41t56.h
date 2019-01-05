/**
 * \file
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

/***************************************************************************//**
\defgroup RTC_M41T56 M41T56
\ingroup RTC

RTC M41T56 содержит восемь регистров и 56 ячеек ОЗУ. Для доступа используется
указатель, который увеличивается после записи или чтения в/из регистра или
ячейки. В этот указатель записывается первый байт передаваемых данных. Таким
образом, для записи в регистр или ячейку сначала нужно передать индекс, затем -
записываемые данные. Что бы получить данные с нужного места, необходимо сделать
запись в указатель, передать состояние повторный старт и считывать данные.

Регистры находятся в диапазоне адресов 0x00 -- 0x07. Ячейки памяти располагаются
с адреса 0x08 по 0x3f.

\code
+---------+-------+-----------------------------------------------+--------+
|         |       |                биты                           |Возмож- |
| функция | адрес +-----+-----+-----+-----+-----+-----+-----+-----+ные     |
|         |       |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |значения|
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| СЕКУНДЫ | 0x00  |  ST |  десятки секунд |     единицы секунд    | 00-59  |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| МИНУТЫ  | 0x01  |  X  |  десятки минут  |     единицы минут     | 00-59  |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| ЧАСЫ    | 0x02  | SEB | SB  |  десятки  |                       |        |
|         |       |     |     |   часов   |     единицы часов     | 00-23  |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| ДЕНЬ    | 0x03  |  X  |  X  |  X  |  X  |  X  |   день недели   |  1-7   |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| ЧИСЛО   | 0x04  |  X  |  X  |  десятки  |     единицы числа     | 01-31  |
|         |       |     |     |   числа   |                       |        |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| МЕСЯЦ   | 0x05  |  X  |  X  |  X  | дес.|     единицы месяца    | 01-12  |
|         |       |     |     |     |месяц|                       |        |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| ГОД     | 0x06  |      десятки лет      |      единицы лет      | 00-99  |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+
| УПРАВЛ. | 0x07  | OUT | FT  |  S  |     калибровка хода RTC     |        |
+---------+-------+-----+-----+-----+-----+-----+-----+-----+-----+--------+

+------+----------+--------------------------------------------------------+
| бит  | значение |                                                        |
|      | при      |                                                        |
|      | включении|                                                        |
+------+----------+--------------------------------------------------------+
| ST   |  X       | При записи 1 в этот бит генератор останавливается      |
+------+----------+--------------------------------------------------------+
| CEB  |  X       | Когда бит CEB (Century Enable Bit) установлен, то при  |
+------+----------+ достижении нового века бит CB (Century Bit) поменяет   |
| CB   |  X       | значение на противоположное.                           |
+------+----------+--------------------------------------------------------+
| OUT  |  1       | Когда биты OUT (OUTput level) и FT (Frequency Test)    |
+------+----------+ равны нулю, вывод FT/OUT прижимается к земле. При FT=1 |
| FT   |  0       | на выводе FT/OUT 512 Гц и бит OUT не имеет значения.   |
+------+----------+--------------------------------------------------------+
| S    |  X       | Знак калибровки хода RTC                               |
+------+----------+--------------------------------------------------------+

X = состояние может быть любым
\endcode
M41T56 управляется кварцевым генератором с номинальной частотой 32768 Гц.
Тестирование показывает, что погрешность частоты генератора может достигать
35 ppm (частей на миллион) при 25 °C, это примерно ±1,53 минуты в месяц. При
правильной установке калибровочных битов точность любого M41T56 улучшается до
±2 ppm при 25 °C.

Частота колебаний любого кристалла изменяется с температурой. Большинство
микросхем RTC компенсируют ошибку частоты кристаллов и их температурный дрейф с
помощью громоздких подстроечных конденсаторов. Однако дизайн M41T56 использует
периодическую коррекцию счетчика. Калибровочная схема добавляет или вычитает
импульсы в схеме делителя генератора при делении на 256, как показано ниже.
Количество пропусков импульсов (вычитание, отрицательная калибровка) или
добавления (добавление, положительная калибровка) зависит от значения,
загруженного в пять младших бит регистра управления. Добавление импульсов
ускоряет ход, а вычитание импульсов замедляет ход часов.

\code
   без        ┌───────┐       ┌───────┐       ┌───────┐       ┌───────┐
калибровки ───┘       └───────┘       └───────┘       └───────┘       └─

положительная ┌───────┐       ┌┐ ┌┐ ┌─┐       ┌───────┐       ┌───────┐
калибровка ───┘       └───────┘└─┘└─┘ └───────┘       └───────┘       └─

отрицательная ┌───────┐                        ┌───────┐       ┌───────┐
калибровка ───┘       └────────────────────────┘       └───────┘       └─
\endcode

Слово калибровки занимает пять младших бит (D4-D0) в регистре управления (адрес
7). Это слово может быть установлено для представления в двоичной форме любого
значения от 0 до 31. Бит S (D5) - бит знака; «1» указывает на положительную
калибровку, «0» означает отрицательную калибровку. Калибровка происходит в
течении 64-минутного цикла. Первые 62 минуты в цикле могут, один раз в минуту,
иметь одну секунду, либо сокращённую на 128, либо увеличенную на 256 импульсов
генератора. Если в слово калибровки загружается значение 1, будут изменены
только первые 2 минуты в течении 64-минутного цикла; если загружается 6, то
будут затронуты первые 12 минут из 64 и так далее.

Поэтому каждый шаг калибровки имеет эффект добавления 512 или вычитания 256
импульсов генератора для каждых 125829120 действительных циклов генератора, то
есть +4.068 или -2.034 ppm регулировки на шаг калибровки в регистре калибровки.
Предполагая, что генератор фактически выдаёт ровно 32768 Гц, каждый из 31
приращений в калибровочном слове будет составлять +10,7/-5,35 секунды в
месяц, что соответствует общему диапазону +5,5/-2,75 минуты в месяц.

Для определения величины калибровки для данного M41T56 доступны два метода.
Первый включает в себя просто установку часов, позволяя им работать в течении
месяца и сравнение показаний с известным точным временем (например, трансляции
WWV). Хотя такой подход может показаться грубым, он позволяет разработчику дать
конечному пользователю возможность откалибровать часы, как это может потребовать
его окружающая среда, даже если конечный продукт упакован в неразборный корпус.
Все, что разработчик должен сделать, это предоставить простой способ изменения
калибровочного слова.

Второй подход лучше подходит для производственной среды и предполагает
использование некоторого испытательного оборудования. Когда бит FT, самый
старший бит (D7) в регистре управления, установлен на «1», а генератор работает
на 32768 Гц, вывод FT/OUT устройства будет переключаться с частотой 512 Гц.
Любое отклонение от 512 Гц указывает степень и направление сдвига частоты
генератора при тестовой температуре.

К примеру,  измеренная частота 512.01024 Гц указывает на ошибку резонатора
+20 ppm и коррекции требует загрузки значения -10 (XX001010) в калибровочное
слово.

Калибровочное слово никак не влияет на частоту вывода FT/OUT.
*******************************************************************************/

#ifndef HAL_RTC_CHIP_M41T56_H_
#define HAL_RTC_CHIP_M41T56_H_

#define rtc_init        rtc_init_m41t56
#define rtc_error       rtc_error_m41t56
#define rtc_clear       rtc_clear_m41t56
#define rtc_check       rtc_check_m41t56
#define rtc_start       rtc_start_m41t56
#define rtc_stop        rtc_stop_m41t56
#define rtc_get_time    rtc_get_time_m41t56
#define rtc_set_time    rtc_set_time_m41t56
#define rtc_get_date    rtc_get_date_m41t56
#define rtc_set_date    rtc_set_date_m41t56
#define rtc_mem_read    rtc_mem_read_m41t56
#define rtc_mem_write   rtc_mem_write_m41t56
#define rtc_get_caliber rtc_get_caliber_m41t56
#define rtc_set_caliber rtc_set_caliber_m41t56

/** \ingroup RTC_M41T56 @{ */

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HAL_RTC_CHIP_M41T56_H_ */

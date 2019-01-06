/**
 * @file
 * @brief Конфигурация устройства.
 * @details
 *
 * @date создан 18.11.2018
 * @author Nick Egorrov
 */

#ifndef SRC_TMS_CONFIG_H_
#define SRC_TMS_CONFIG_H_

/*
 * Соответствие разрядов регистров индикации сегментам.
 * Левая цифра - номер регистра      правая - номер бита. Нумерация от 0.
 *                            reg, bit
 */

/* Нижний ряд ламп. */
#define CFG_REG_ALARM           0, 7
#define CFG_REG_MONDAY          1, 4
#define CFG_REG_TEUSDAY         2, 1
#define CFG_REG_WEDNESDAY       3, 6
#define CFG_REG_THURSDAY        4, 3
#define CFG_REG_FRIDAY          5, 0
#define CFG_REG_SATURDAY        6, 5
#define CFG_REG_SUNDAY          7, 2

/* Разделители. */
#define CFG_REG_RIGHT_BOTTOM    6, 6
#define CFG_REG_RIGHT_TOP       6, 7
#define CFG_REG_LEFT_BOTTOM     3, 7
#define CFG_REG_LEFT_TOP        2, 0

/* Единицы секунд */
#define CFG_REG_xxxxx0          8, 6
#define CFG_REG_xxxxx1          8, 5
#define CFG_REG_xxxxx2          8, 4
#define CFG_REG_xxxxx3          8, 2
#define CFG_REG_xxxxx4          7, 0
#define CFG_REG_xxxxx5          7, 1
#define CFG_REG_xxxxx6          8, 0
#define CFG_REG_xxxxx7          8, 1
#define CFG_REG_xxxxx8          8, 3
#define CFG_REG_xxxxx9          8, 7

/* Десятки секунд */
#define CFG_REG_xxxx0x          6, 1
#define CFG_REG_xxxx1x          6, 0
#define CFG_REG_xxxx2x          7, 7
#define CFG_REG_xxxx3x          7, 5
#define CFG_REG_xxxx4x          6, 3
#define CFG_REG_xxxx5x          6, 4
#define CFG_REG_xxxx6x          7, 3
#define CFG_REG_xxxx7x          7, 4
#define CFG_REG_xxxx8x          7, 6
#define CFG_REG_xxxx9x          6, 2

/* Единицы минут */
#define CFG_REG_xxx0xx          5, 7
#define CFG_REG_xxx1xx          5, 6
#define CFG_REG_xxx2xx          5, 5
#define CFG_REG_xxx3xx          5, 3
#define CFG_REG_xxx4xx          4, 1
#define CFG_REG_xxx5xx          4, 2
#define CFG_REG_xxx6xx          5, 1
#define CFG_REG_xxx7xx          5, 2
#define CFG_REG_xxx8xx          5, 4
#define CFG_REG_xxx9xx          4, 0

/* Десятки минут */
#define CFG_REG_xx0xxx          3, 2
#define CFG_REG_xx1xxx          3, 1
#define CFG_REG_xx2xxx          3, 0
#define CFG_REG_xx3xxx          4, 6
#define CFG_REG_xx4xxx          3, 4
#define CFG_REG_xx5xxx          3, 5
#define CFG_REG_xx6xxx          4, 4
#define CFG_REG_xx7xxx          4, 5
#define CFG_REG_xx8xxx          4, 7
#define CFG_REG_xx9xxx          3, 3

/* Единицы часов */
#define CFG_REG_x0xxxx          1, 0
#define CFG_REG_x1xxxx          2, 7
#define CFG_REG_x2xxxx          2, 6
#define CFG_REG_x3xxxx          2, 4
#define CFG_REG_x4xxxx          1, 2
#define CFG_REG_x5xxxx          1, 3
#define CFG_REG_x6xxxx          2, 2
#define CFG_REG_x7xxxx          2, 3
#define CFG_REG_x8xxxx          2, 5
#define CFG_REG_x9xxxx          1, 1

/* Десятки часов */
#define CFG_REG_0xxxxx          0, 3
#define CFG_REG_1xxxxx          0, 2
#define CFG_REG_2xxxxx          0, 1
#define CFG_REG_3xxxxx          1, 7
#define CFG_REG_4xxxxx          0, 5
#define CFG_REG_5xxxxx          0, 6
#define CFG_REG_6xxxxx          1, 5
#define CFG_REG_7xxxxx          1, 6
#define CFG_REG_8xxxxx          0, 0
#define CFG_REG_9xxxxx          0, 4

/*
 * Соответствие выводов для SPI.
 *          Последняя буква порта, номер бита
 */
#define CFG_SPI_LOAD            C, 0 /* Вывод для строба записи регистров. */
#define CFG_SPI_SS              B, 2
#define CFG_SPI_MOSI            B, 3
#define CFG_SPI_MISO            B, 4 /* Не используется. */
#define CFG_SPI_SCK             B, 5

/*
 * Если задан CFG_DISPLAY_LOGARITHMIC_BALANCE, то баланс яркостей старого и
 * нового значения индикатора имеет логарифмическую зависимость.
 */
/* #define CFG_DISPLAY_LOGARITHMIC_BALANCE */

/*
 *
 */
#define CFG_SHOW_DURATION_ERROR 3000U
#define CFG_SHOW_DURATION_DATE  7000U
#define CFG_SHOW_BLINK_DURATION 500U
#define CFG_SHOW_INTRO_PERIOD   333U

/*
 * Соответствие выводов кнопкам.
 *  Последняя буква порта, номер бита
 */
#define CFG_KEY_MENU    B, 4
#define CFG_KEY_SELECT  B, 0
#define CFG_KEY_CHANGE  B, 1

/* Длительность паузы для защиты от дребезга, в мс. */
#define CFG_USER_KEY_BOUNCE             80
/* Длительность паузы перед запуском автоповтора нажатий кнопок, в мс. */
#define CFG_USER_KEY_REPEAT_DELAY       600
/* Длительность периода автоповтора нажатий кнопок, в мс. */
#define CFG_USER_KEY_REPEAT_PERIOD      240
/* Количество автоповторов перед включением быстрого автоповтора. */
#define CFG_USER_KEY_REPEAT_NUMBER      4
/* Длительность периода быстрого автоповтора нажатий кнопок, в мс. */
#define CFG_USER_KEY_REPEAT_FAST        80

/*
 * Выводы для управления плеером. Вход CFG_PLAYER_IN зарезервирован на
 * всякий случай.
 *          Последняя буква порта, номер бита
 */
#define CFG_PLAYER_IN           D, 7
#define CFG_PLAYER_ADR1         D, 6
#define CFG_PLAYER_ADR2         D, 5
#define CFG_PLAYER_ADR4         D, 4
#define CFG_PLAYER_ADR8         D, 3
#define CFG_PLAYER_ADR16        D, 2
#define CFG_PLAYER_ADR32        D, 1

#endif /* SRC_TMS_CONFIG_H_ */

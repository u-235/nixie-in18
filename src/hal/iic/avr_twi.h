/**
 * \file
 * \brief Реализация IIC в AVR
 * \details
 *
 * @date created on:
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 *
 * \section twi Модуль TWI.
 * Модуль TWI предназначен для связи микроконтроллера с внешними устройствами
 * через шину I2C.
 * \n \ref twireg
 * \n \ref twistate
 *
 * \subsection twireg Регистры TWI.
 * Модуль содержит следующие регистры
 *
 * \par Регистр TWBR.
 * Совместно с \b "TWSR" управляет частотой сигнала \b SCL. Частота вычисляется
 * по формуле
 * \code
 *         Частота CPU
 * SCL=----------------------
 *                       TWPS
 *        16 + 2(TWBP)*4
 * \endcode
 * Регистр доступен для чтения и записи.
 *
 * \par Регистр управления TWCR.
 * \b |TWINT|TWEA|TWSTA|TWSTO|TWWC|TWEN|---|TWIE|
 * \n\n\ref TWINT. Флаг прерывания.
 * \n\ref TWEA. Бит разрешения подтверждения \b ACK.
 * \n\ref TWSTA. Бит состояния \b START.
 * \n\ref TWSTO. Бит состояния \b STOP.
 * \n\ref TWWC. Флаг коллизии записи.
 * \n\ref TWEN. Флаг разрешения TWI.
 * \n\ref TWIE. Бит разрешения прерываний TWI.
 *
 * \par  Регистр состояния TWSR.
 * \b |TWS7|TWS6|TWS5|TWS4|TWS3|---|TWPS1|TWPS0|
 * \n\n Биты TWS7 - TWS3 отражают состояние модуля и доступны только для чтения.
 * Что бы получить значение только этих битов, Вы можете использовать макрос
 * \ref _twi_status. Значения этих битов при различных условиях обсуждаются
 * на странице \ref twistate "состояния TWI".
 * \n\n Биты \ref TWPS1 и \ref TWPS0 управляют предделителем и доступны для чтения
 * и записи.
 *
 * \par  Регистр TWDR.
 * In Transmit mode, TWDR contains the next byte to be transmitted. In Receive mode, the
 TWDR contains the last byte received. It is writable while the TWI is not in the process of
 shifting a byte. This occurs when the TWI Interrupt Flag (TWINT) is set by hardware.
 Note that the Data Register cannot be initialized by the user before the first interrupt
 occurs. The data in TWDR remains stable as long as TWINT is set. While data is shifted
 out, data on the bus is simultaneously shifted in. TWDR always contains the last byte
 present on the bus, except after a wake up from a sleep mode by the TWI interrupt. In
 this case, the contents of TWDR is undefined. In the case of a lost bus arbitration, no
 data is lost in the transition from Master to Slave. Handling of the ACK bit is controlled
 automatically by the TWI logic, the CPU cannot access the ACK bit directly.
 *
 * \par  Регистр TWAR.
 *
 * \par  Регистр TWAMR.
 *
 * \section twistate Состояния TWI.
 * \subsection twistatemt Ведущий передатчик.
 * - \ref TWI_MT_START
 * - \ref TWI_MT_RESTART
 * - \ref TWI_MT_ADR_ACK
 * - \ref TWI_MT_ADR_NOACK
 * - \ref TWI_MT_DATA_ACK
 * - \ref TWI_MT_DATA_NOACK
 * - \ref TWI_MT_LOST_CONTROL
 *
 * \date created on: 21.01.2018
 * \author nick
 */

#ifndef HAL_IIC_AVR_TWI_H_
#define HAL_IIC_AVR_TWI_H_

/**
 * \def TWINT
 * \brief Флаг прерывания.
 * \details Этот бит устанавливается аппаратно при завершении текущей операции.
 * Сброс происходит только программно при записи в этот бит единицы. После
 * сброса этого флага запускается операция TWI, поэтому любые обращения к
 * регистрам "адреса",  "статуса" и "данных" должны быть завершены до сброса
 * этого флага.
 *
 * \def TWEA
 * \brief Бит разрешения подтверждения \b ACK.
 * \details Если этот бит установлен, то подтверждение \b ACK генерируется в
 * следующих случаях:
 * - Был получен адрес от ведущего устройства.
 * - Был получен общий вызов и биты \ref TWGCE и "TWAR" установлены.
 * - В режиме ведущего устройства был получен байт данных от ведомого устройства.
 *
 * \def TWSTA
 * \brief Бит состояния \b START.
 * \details Установка этого бита переводит модуль в режим ведущего и, если
 * шина I2C свободна, генерирует состояние \b START. Если шина занята, модуль
 * ожидает её освобождения, то есть состояния \b STOP. После генерации
 * \b START этот бит должен быть сброшен программно.
 *
 * \def TWSTO
 * \brief Бит состояния \b STOP.
 * \details В режиме ведущего установка этого бита приводит к генерации
 * состояния \b STOP. Когда \b STOP выполнен, этот бит сбрасывается
 * автоматически.
 * \n В режиме ведомого используется при ошибках. Генерации состояния \b STOP
 * не происходит. Модуль переводится в режим не выбранного устройства и линии
 * \b SCL и \b SDA переходят в высокоомное состояние.
 *
 * \def TWWC
 * \brief Флаг коллизии записи.
 * \details Этот флаг устанавливается при попытке записи в "регистр данных"
 * когда \ref TWINT сброшен. Этот флаг сбрасывается при записи в регистр данных
 * при высоком уровне TWINT.
 *
 * \def TWEN
 * \brief Бит разрешения TWI.
 * \details Этот бит разрешает операции TWI и включает интерфейс TWI. Когда
 *бит установлен в единицу, модуль TWI получает управление над выводами,
 *подключенными к \b SCL и \b SDA.
 *
 * \def TWIE
 * \brief Бит разрешения прерываний TWI.
 * \details Когда этот бит и бит \b I регистра \b SREG установлены в единицу
 * запрос прерывания TWI активен пока \ref TWINT находится в высоком состоянии.
 */
#ifdef __DOXYGEN__
/* TWI Bit Rate Register – TWBR */
#   define TWBR7 7
#   define TWBR6 6
#   define TWBR5 5
#   define TWBR4 4
#   define TWBR3 3
#   define TWBR2 2
#   define TWBR1 1
#   define TWBR0 0
/* TWI Control Register – TWCR */
#   define TWINT 7
#   define TWEA  6
#   define TWSTA 5
#   define TWSTO 4
#   define TWWC  3
#   define TWEN  2
#   define TWIE  0
/* TWI Status Register – TWSR */
#   define TWS7  7
#   define TWS6  6
#   define TWS5  5
#   define TWS4  4
#   define TWS3  3
#   define TWS2  2
#   define TWPS0 1
#   define TWPS1 0
/* TWI Data Register – TWDR */
#   define TWD7 7
#   define TWD6 6
#   define TWD5 5
#   define TWD4 4
#   define TWD3 3
#   define TWD2 2
#   define TWD1 1
#   define TWD0 0
/* TWI (Slave) Address Register – TWAR */
#   define TWA6  7
#   define TWA5  6
#   define TWA4  5
#   define TWA3  4
#   define TWA2  3
#   define TWA1  2
#   define TWA0  1
#   define TWGCE 0
#endif


#define _twi_status (TWSR & 0xF8)
#define _twsr_is(st) (_twi_status == st)
#define _twsr_not(st) (_twi_status != st)

/**
 * \def TWI_MT_START
 * \brief Было передано состояние START.
 * \details TWI работает в режиме "ведущий передатчик".
 *
 * \b Реакция
 * - Загрузка в "регистр данных" адреса устройства и бита
 *записи. Установка бита \ref TWINT. Ожидается передача адреса и состояние
 * \ref TWI_MT_ADR_ACK или \ref TWI_MT_ADR_NOACK.
 *"регистр состояния", \ref _twi_status
 */
#define TWI_MT_START           ((uint8_t)0x08)

/**
 * \def TWI_MT_RESTART
 * \brief Было передано повторное состояние START.
 * \details TWI работает в режиме "ведущий передатчик".
 *
 * \b Реакция
 * - Загрузка в "регистр данных" адреса устройства и бита
 *записи. Установка бита \ref TWINT. Ожидается передача адреса и состояние
 * \ref TWI_MT_ADR_ACK или \ref TWI_MT_ADR_NOACK.
 * - Загрузка в "регистр данных" адреса устройства и бита
 *чтения. Установка бита \ref TWINT. TWI переходит в режим ведущего получателя
 *данных. Ожидается состояние \ref TWI_MR_ADR_ACK или \ref TWI_MR_ADR_NOACK.
 *"регистр состояния", \ref _twi_status
 */
#define TWI_MT_RESTART         ((uint8_t)0x10)

/**
 * \def TWI_MT_LOST_CONTROL
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MT_LOST_CONTROL    ((uint8_t)0x38)

/**
 * \def TWI_MT_ADR_ACK
 * \brief Был передан адрес и получено состояние ACK.
 * \details TWI работает в режиме  "ведущий передатчик".
 *
 * \b Реакция
 * - Загрузка в "регистр данных" байта данных. Установка бита
 * \ref TWINT. Ожидается состояние \ref TWI_MT_DATA_ACK или
 * \ref TWI_MT_DATA_NOACK.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT и \ref TWSTA. Ожидается состояние \ref TWI_MT_RESTART.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT и \ref TWSTO. Будет передано состояние STOP и очищен бит
 * \ref TWSTO.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT, \ref TWSTA  и \ref TWSTO. Будет передано состояние START, затем
 *STOP и очищен бит \ref TWSTO.
 *"регистр состояния", \ref _twi_status
 */
#define TWI_MT_ADR_ACK         ((uint8_t)0x18)

/**
 * \def TWI_MT_ADR_NOACK
 * \brief Был передан адрес, но состояние ACK не получено.
 * \details TWI работает в режиме "ведущий передатчик".
 *
 * \b Реакция
 * - Загрузка в "регистр данных" байта данных. Установка бита
 * \ref TWINT. Ожидается состояние \ref TWI_MT_DATA_ACK или
 * \ref TWI_MT_DATA_NOACK.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT и \ref TWSTA. Ожидается состояние \ref TWI_MT_RESTART.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT и \ref TWSTO. Будет передано состояние STOP и очищен бит
 * \ref TWSTO.
 * - В "регистр данных" ничего не загружается. Устанавливаются биты
 * \ref TWINT, \ref TWSTA  и \ref TWSTO. Будет передано состояние START, затем
 *STOP и очищен бит \ref TWSTO.
 *"регистр состояния", \ref _twi_status
 */
#define TWI_MT_ADR_NOACK       ((uint8_t)0x20)

/**
 * \def TWI_MT_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MT_DATA_ACK        ((uint8_t)0x28)

/**
 * \def TWI_MT_DATA_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MT_DATA_NOACK      ((uint8_t)0x30)

/**
 * \def TWI_MR_START
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_START           ((uint8_t)0x08)

/**
 * \def TWI_MR_RESTART
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_RESTART         ((uint8_t)0x10)

/**
 * \def TWI_MR_LOST_CONTROL
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_LOST_CONTROL    ((uint8_t)0x38)

/**
 * \def TWI_MR_ADR_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_ADR_ACK         ((uint8_t)0x40)

/**
 * \def TWI_MR_ADR_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_ADR_NOACK       ((uint8_t)0x48)

/**
 * \def TWI_MR_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_DATA_ACK        ((uint8_t)0x50)

/**
 * \def TWI_MR_DATA_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_MR_DATA_NOACK      ((uint8_t)0x58)

/**
 * \def TWI_ST_ADR_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_ST_ADR_ACK         ((uint8_t)0xA8)

/**
 * \def TWI_ST_LOST_CONTROL
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_ST_LOST_CONTROL    ((uint8_t)0xB0)

/**
 * \def TWI_ST_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_ST_DATA_ACK        ((uint8_t)0xB8)

/**
 * \def TWI_ST_DATA_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_ST_DATA_NOACK      ((uint8_t)0xC0)

/**
 * \def TWI_ST_LAST_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_ST_LAST_DATA_ACK   ((uint8_t)0xC8)

/**
 * \def TWI_SR_ADR_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_ADR_ACK                 ((uint8_t)0x60)

/**
 * \def TWI_SR_LOST_CONTROL
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_LOST_CONTROL            ((uint8_t)0x68)

/**
 * \def TWI_SR_GENERAL_ADR_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_GENERAL_ADR_ACK         ((uint8_t)0x70)

/**
 * \def TWI_SR_GENERAL_LOST_CONTROL
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_GENERAL_LOST_CONTROL    ((uint8_t)0x78)

/**
 * \def TWI_SR_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_DATA_ACK                ((uint8_t)0x80)

/**
 * \def TWI_SR_DATA_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_DATA_NOACK              ((uint8_t)0x88)

/**
 * \def TWI_SR_GENERAL_DATA_ACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_GENERAL_DATA_ACK        ((uint8_t)0x90)

/**
 * \def TWI_SR_GENERAL_DATA_NOACK
 * \brief
 * \details
 *
 * \b Реакция
 * -
 */
#define TWI_SR_GENERAL_DATA_NOACK      ((uint8_t)0x98)

/**
 * \def TWI_UNKNOWN_STATE
 * \brief
 * \details Status indicates that no relevant information is available because
 * the #TWINT Flag is not set. This occurs between other states, and when the
 * TWI is not involved in a serial transfer.
 * \b Реакция
 * - No #TWDR action. Wait or proceed current transfer.
 */
#define TWI_UNKNOWN_STATE       ((uint8_t)0xF8)

/**
 * \def TWI_BUS_ERROR
 * \brief
 * \details Status indicates that a bus error has occurred during a Two-wire
 * Serial Bus transfer. A bus error occurs when a START or STOP condition occurs
 * at an illegal position in the format frame. Examples of such illegal
 * positions are during the serial transfer of an address byte, a data byte, or
 * an acknowledge bit. When a bus error occurs, #TWINT is set. To recover from
 * a bus error, the #TWSTO Flag must set and #TWINT must be cleared by writing
 * a logic one to it. This causes the TWI to enter the not addressed Slave mode
 * and to clear the #TWSTO Flag (no other bits in #TWCR are affected). The SDA
 * and SCL lines are released, and no STOP condition is transmitted.
 * \b Реакция
 * - No #TWDR action.
 *
 */
#define TWI_BUS_ERROR           ((uint8_t)0x00)

#define _twi_status (TWSR & 0xF8)
#define _twsr_is(st) (_twi_status == st)
#define _twsr_not(st) (_twi_status != st)

#endif /* HAL_IIC_AVR_TWI_H_ */

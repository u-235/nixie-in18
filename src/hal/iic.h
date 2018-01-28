/***************************************************************************//**
 \file iic.h
 \date created on: 21.01.2018
 \author Nick Egorrov
 \defgroup IIC <iic.h> : Шина IIC
 Этот модуль обеспечивает обмен с переферийными устройствами по шине I2C в
 режиме мастера.
 Работу с шиной  можно условно разделить на три группы:
 - \ref IIC_COMMON. Манипуляции с адресом устройств, получение кода ошибки и
 сброс ошибки.
 - \ref IIC_LOW
 - \ref IIC_HIGH
 ******************************************************************************/

#ifndef SRC_HAL_IIC_H_
#define SRC_HAL_IIC_H_

#include <stdint.h>

#ifdef _cplusplus
extern "C" {
#endif

/***************************************************************************//**
 \addtogroup IIC
 \section IIC_COMMON Общие функции
 Эти функции используются и с низкоуровневыми и с высокоуровневыми функциями и
 позволяют установить и получить адрес устройства, получить код ошибки и при

 @{
 ******************************************************************************/

/**
 * Время ожидания ответа от устройства в миллисекундах.
 */
#define IIC_TIMEOUT 50

/**
 * \brief hdf
 */
typedef enum {
        IIC_NO_ERROR = 0,       //!< IIC_STATUS_OK
        IIC_ERROR_START = 0x01, //!< IIC_ERROR_START
        IIC_ERROR_ADDR = 0x02,  //!< IIC_ERROR_ADDR
        IIC_ERROR_WRITE = 0x03, //!< IIC_ERROR_WRITE
        IIC_ERROR_READ = 0x04,  //!< IIC_ERROR_READ
        IIC_ERROR_STOP = 0x05,  //!< IIC_ERROR_STOP
        IIC_ERROR_WAIT = 0x80   //!< IIC_ERROR_WAIT
} iic_error_t;

/**
 * \brief Инициализация устройства.
 */
extern void iic_init();

/**
 * \brief
 * \details
 * \param adr
 */
extern void iic_set_address(uint8_t adr);

/**
 * \brief
 * \details
 * \return
 */
extern uint8_t iic_get_address();

/**
 * \brief
 * \details
 * \return
 */
extern iic_error_t iic_error();

/**
 * \brief
 * \details
 */
extern void iic_clear();

/** @} */

/***************************************************************************//**
 \addtogroup IIC
 \section IIC_LOW Низкоуровневые функции

 @{
 ******************************************************************************/

/**
 *
 * \param mode
 */
extern void iic_ll_start(uint8_t mode);

/**
 *
 */
extern void iic_ll_stop();

/**
 * Считывает с шины байт.
 * \param last ноль если требуется считать байт и завершить обмен по шине и не
 *      нулевое значение если требуется считать серию байт.
 * \return
 */
extern uint8_t iic_ll_read(uint8_t last);

/**
 *
 * @param d
 */
extern void iic_ll_write(uint8_t d);

/** @} */

/***************************************************************************//**
 \addtogroup IIC
 \section IIC_HIGH Высокоуровневые функции

 @{
 ******************************************************************************/

/**
 * \brief Запись байта в устройство.
 * \details Перед вызовом необходимо убедиться, что функция iic_error() возвращает
 * #IIC_NO_ERROR. В ином случае нужно вызвать iic_clear().
 * Адрес устройства можно изменить функцией iic_set_address(uint8_t adr).
 * \param data Байт для записи в устройство.
 */
extern void iic_write(uint8_t data);

/**
 * \brief Чтение байта из устройства.
 * \details Перед вызовом необходимо убедиться, что функция iic_error() возвращает
 * #IIC_NO_ERROR. В ином случае нужно вызвать iic_clear().
 * Адрес устройства можно изменить функцией iic_set_address(uint8_t adr).
 * \return Байт данных из устройства.
 */
extern uint8_t iic_read();

/**
 * \brief Запись и чтение массивов.
 * \details Запись и последующее чтение массивов из/в устройство. Поскольку
 * сначала происходит запись, то, при необходимости, считывание можно
 * производить в тот же буфер.
 * \param input Массив с данными для записи. Если равен нулю, то записи
 *      не происходит.
 * \param wSz Количество записываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param output Массив для считываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param rSz Количество считываемых байт. Если равен нулю, то записи
 *      не происходит.
 */
void iic_write_read(uint8_t input[], uint8_t wSz, uint8_t output[],
                uint8_t rSz);

/** @} */

#ifdef _cplusplus
extern "C" {
#endif

#endif /* SRC_HAL_IIC_H_ */

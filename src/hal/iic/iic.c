/**
 * \file iic.c
 * \brief
 * \details
 *
 * \date created on: 21.01.2018
 * \author Nick Egorrov
 */

#include "../iic.h"

#define INSIDE_SRC_HAL_IIC_C_

static iic_error_t status;  // Статус устройства.
static uint8_t address; // device address

/*
 * Сброс ошибки на уровне аппаратной части МК.
 */
static void clear();

extern void iic_init()
{
        iic_clear();
}

extern void iic_set_address(uint8_t adr)
{
        address = adr & 0xfe;
}

extern uint8_t iic_get_address()
{
        return address;
}

/*
 * Возвращает код ошибки.
 */
extern iic_error_t iic_error()
{
        return status;
}

/*
 *
 */
extern void iic_clear()
{
        clear();
        status = IIC_NO_ERROR;
}

/*
 * Запись байта в устройство. Адрес устройства можно изменить функцией
 * iic_set_address(uint8_t adr).
 *
 * Перед вызовом необходимо убедиться, что функция iic_error() возвращает
 * #IIC_NO_ERROR. В ином случае нужно вызвать iic_clear().
 * \param data Байт для записи в устройство.
 */
extern void iic_write(uint8_t data)
{
        iic_ll_start(0);
        iic_ll_write(data);
        iic_ll_stop();
}

/*
 * Чтение байта из устройства. Адрес устройства можно изменить функцией
 * iic_set_address(uint8_t adr).
 *
 * Перед вызовом необходимо убедиться, что функция iic_error() возвращает
 * #IIC_NO_ERROR. В ином случае нужно вызвать iic_clear().
 * \return Байт данных из устройства.
 */
extern uint8_t iic_read()
{
        uint8_t res;
        iic_ll_start(1);
        res = iic_ll_read(0);
        iic_ll_stop();
        return res;
}

/*
 * Запись и последующее чтение массивов из/в устройство. Поскольку сначала
 * происходит запись, то, при необходимости, считывание можно производить
 * в тот же буфер.
 * \param input Массив с данными для записи. Если равен нулю, то записи
 *      не происходит.
 * \param wSz Количество записываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param output Массив для считываемых данных. Если равен нулю, то записи
 *      не происходит.
 * \param rSz Количество считываемых байт. Если равен нулю, то записи
 *      не происходит.
 */
void iic_write_read(uint8_t input[], uint8_t wSz, uint8_t output[], uint8_t rSz)
{
        if (input != 0 && wSz != 0) {
                iic_ll_start(0);
                for (unsigned char i = 0; i < wSz; i++) {
                        iic_ll_write(*input++);
                }
        }

        if (output != 0 && rSz != 0) {
                iic_ll_start(1);
                for (unsigned char i = rSz; i > 0; i--) {
                        *output++ = iic_ll_read(i);
                }
        }
        iic_ll_stop();
}

#include "iic_avr_twi_impl.h"

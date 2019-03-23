/**
 * \file
 * \brief
 * \details
 *
 * \date created on: 26.01.2018
 * @author Nick Egorrov
 * @copyright Copyright 2019 Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef HAL_IIC_AVR_TWI_IMPL_H_
#define HAL_IIC_AVR_TWI_IMPL_H_

#include <avr/io.h>
#include <util/delay.h>
#include "avr_twi.h"

#ifndef INSIDE_SRC_HAL_IIC_C_
#       error This file most be included only in iic.h
#endif

/*
 * Для настройки TWI на нужную частоту линии Clock требуется указать эту
 * частоту в герцах в макросе F_IIC или задать настройки непосредственно в
 * макросах TWI_PRESCALER и TWI_BIT_RATE.
 */
#if !defined CFG_IIC_FREQUENSY && ( !defined TWI_PRESCALER || !defined TWI_BIT_RATE)
#       error Unknown IIC clock frequency
#endif

/*
 * Вычисление значения предделителя, если он не задан.
 */
#ifndef TWI_PRESCALER
#       ifndef TWI_BIT_RATE
#               if (F_CPU / CFG_IIC_FREQUENSY / 526) < 1
#                       define TWI_PRESCALER 0x00
#               elif (F_CPU / CFG_IIC_FREQUENSY / 526) <= 4
#                       define TWI_PRESCALER 0x01
#               elif (F_CPU / CFG_IIC_FREQUENSY / 526) <= 16
#                       define TWI_PRESCALER 0x02
#               elif (F_CPU / CFG_IIC_FREQUENSY / 526) <= 64
#                       define TWI_PRESCALER 0x03
#               else
#                       error too low CFG_IIC_FREQUENSY
#               endif
#       else
#               if TWI_BIT_RATE < 0 || TWI_BIT_RATE > 255
#                       error Invalid TWI_BIT_RATE
#               endif
#               if (F_CPU / CFG_IIC_FREQUENSY / (16 + 2 * TWI_BIT_RATE)) < 1
#                       define TWI_PRESCALER 0x00
#               elif (F_CPU / CFG_IIC_FREQUENSY / (16 + 2 * TWI_BIT_RATE)) <= 4
#                       define TWI_PRESCALER 0x01
#               elif (F_CPU / CFG_IIC_FREQUENSY / (16 + 2 * TWI_BIT_RATE)) <= 16
#                       define TWI_PRESCALER 0x02
#               elif (F_CPU / CFG_IIC_FREQUENSY / (16 + 2 * TWI_BIT_RATE)) <= 64
#                       define TWI_PRESCALER 0x03
#               else
#                       error too low CFG_IIC_FREQUENSY or too low TWI_BIT_RATE
#               endif
#       endif
#endif

#if TWI_PRESCALER < 0 || TWI_PRESCALER > 3
#       error Invalid TWI_PRESCALER
#endif

#ifndef TWI_BIT_RATE
#       if TWI_PRESCALER == 0x00
#               define TWI_BIT_RATE (F_CPU / CFG_IIC_FREQUENSY - 16) / 2
#       elif TWI_PRESCALER == 0x01
#               define TWI_BIT_RATE (F_CPU / CFG_IIC_FREQUENSY / 4 - 16) / 2
#       elif TWI_PRESCALER == 0x02
#               define TWI_BIT_RATE (F_CPU / CFG_IIC_FREQUENSY / 16 - 16) / 2
#       elif TWI_PRESCALER == 0x03
#               define TWI_BIT_RATE (F_CPU / CFG_IIC_FREQUENSY / 64 - 16) / 2
#       endif

#       if TWI_BIT_RATE > 255
#               error too big TWI_BIT_RATE, increase TWI_PRESCALER
#       endif
#endif

/*
 * \brief Ожидание завершения операции.
 * \details
 */
static void twi_wait(const uint8_t mask);

/*
 * Сброс ошибки на уровне аппаратной части МК.
 */
void reset()
{
        TWCR = 0; /* выключение модуля TWI */
        TWBR = TWI_BIT_RATE;
        TWSR = TWI_PRESCALER;

}

/*
 * Выставляет на шину состояние старт/рестарт и адрес устройства.
 * \param mode Режим обмена шины.
 */
void iic_ll_start(uint8_t mode)
{
        uint8_t addr;

        if (error != IIC_NO_ERROR) {
                return;
        }

        if (mode == IIC_WRITE) {
                addr = address & 0xfe;
        } else {
                addr = address | 0x01;
        }

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
        twi_wait(1 << TWINT);
        if (_twsr_not(TWI_MT_START) && _twsr_not(TWI_MT_RESTART)) {
                error |= IIC_ERROR_START;
                return;
        }

        TWDR = addr;
        TWCR = (1 << TWINT) | (1 << TWEN);
        twi_wait(1 << TWINT);
        if (_twsr_not(TWI_MT_ADR_ACK) && _twsr_not(TWI_MR_ADR_ACK)) {
                error |= IIC_ERROR_ADDR;
        }
        return;
}

void iic_ll_stop()
{
        if (error != IIC_NO_ERROR) {
                return;
        }

        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
        twi_wait(1 << TWSTO);
        if ((TWCR & (1 << TWSTO)) == 0) {
                error |= IIC_ERROR_STOP;
        }
}

/*
 * Считывает с шины байт.
 * \param last ноль если требуется считать байт и завершить обмен по шине и не
 *      нулевое значение если требуется считать серию байт.
 */
uint8_t iic_ll_read(uint8_t last)
{
        if (error != IIC_NO_ERROR) {
                return 0;
        }

        if (last == 0) {
                TWCR = (1 << TWINT) | (1 << TWEN);
                twi_wait(1 << TWINT);
                if (_twsr_not(TWI_MR_DATA_NOACK)) {
                        error |= IIC_ERROR_READ;
                }
        } else {
                TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
                twi_wait(1 << TWINT);
                if (_twsr_not(TWI_MR_DATA_ACK)) {
                        error |= IIC_ERROR_READ;
                }
        }
        return TWDR;
}

/*
 * Выставляет на шину байт данных.
 */
void iic_ll_write(uint8_t d)
{
        if (error != IIC_NO_ERROR) {
                return;
        }

        TWDR = d;
        TWCR = (1 << TWINT) | (1 << TWEN);
        twi_wait(1 << TWINT);
        if (_twsr_not(TWI_MT_DATA_ACK)) {
                error |= IIC_ERROR_WRITE;
        }
        return;
}

/*
 * \brief Ожидание завершения операции.
 * \details
 */
void twi_wait(const uint8_t mask)
{
        uint8_t c;
        for (c = 0; (TWCR & mask) == 0; c++) {
                if (c > IIC_TIMEOUT) {
                        error |= IIC_ERROR_WAIT;
                        break;
                }
                _delay_us(950);
        }
}

#endif /* HAL_IIC_AVR_TWI_IMPL_H_ */

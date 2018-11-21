/**
 * \file
 * \brief Манипуляции с битами.
 * \details Эти макросы призваны облегчить работу с битами вообще и управление
 * выводами МК в частности.
 *
 * \date создан 18.11.2018
 * \author Nick Egorrov
 */

#ifndef SRC_BIT_HELPER_H_
#define SRC_BIT_HELPER_H_

#define _mask_set(dst, mask) dst|=(mask)
#define _mask_clear(dst, mask) dst&=~(mask)
#define _bit_set(dst, bit) _mask_set(dst, (1<<bit))
#define _bit_clear(dst, bit) _mask_clear(dst, (1<<bit))

/**
 * @brief Настройка ножки МК на вывод.
 * @details Макрос принимает макрос, содержащий букву порта и номер ножки,
 * разделённые запятой.
 * @code
 * #define CFG_PIN_LED C, 7
 * #define CFG_PIN_KEY B, 2
 * ...
 * _dir_out(CFG_PIN_LED);
 * _dir_in(CFG_PIN_KEY);
 * @endcode
 */
#define _dir_out(x) __dir_out(x)
/**
 * @brief Настройка ножки МК на ввод.
 * @copydetails _dir_out */
#define _dir_in(x) __dir_in(x)

/**
 * @brief Установка вывода МК в 1.
 * @details Макрос принимает макрос, содержащий букву порта и номер ножки,
 * разделённые запятой.
 * @code
 * #define CFG_PIN_LED C, 7
 * ...
 * _pin_on(CFG_PIN_LED);
 * delay_ms(500);
 * _pin_off(CFG_PIN_LED);
 * @endcode
 */
#define _pin_on(x) __pin_on(x)
/**
 * @brief Сброс вывода МК в 0
 * @copydetails _pin_on */
#define _pin_off(x) __pin_off(x)

/**
 * @brief Проверка 0 на выводе МК.
 * @details Макрос принимает макрос, содержащий букву порта и номер ножки,
 * разделённые запятой.
 * @code
 * #define CFG_PIN_KEY B, 2
 * ...
 * _is_pin_clean(CFG_PIN_KEY){
 *      на выводе "0"
 * }
 * ...
 * _is_pin_set(CFG_PIN_KEY){
 *      на выводе "1"
 * }
 * @endcode
 */
#define _is_pin_clean(x) __is_pin_clean(x)
/**
 * @brief  Проверка 1 на выводе МК.
 * @copydetails _is_pin_clean */
#define _is_pin_set(x) __is_pin_set(x)

/* Вспомогательные макросы, зависящие от семейства МК. Не использовать
 * непосредственно. */
#define __dir_out(port, pin) _bit_set(DDR##port, pin)
#define __dir_in(port, pin) _bit_clear(DDR##port, pin)

#define __pin_on(port, pin) _bit_set(PORT##port, pin)
#define __pin_off(port, pin) _bit_clear(PORT##port, pin)

#define __is_pin_clean(port, pin) ((PIN##port & (1<<pin)) == 0)
#define __is_pin_set(port, pin) ((PIN##port & (1<<pin)) != 0)

#endif /* SRC_BIT_HELPER_H_ */

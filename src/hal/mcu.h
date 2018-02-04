/**
 * \file mcu.h
 * \brief Общее управление микроконтроллером.
 * \details Модуль реализует инициализацию МК, управление прерываниями и
 * механизм системного таймера.
 *
 * Системный таймер включает в себя обработчик прерывания от аппаратного
 * таймера, счётчик срабатываний и функцию mcu_get_timer_fire() для проверки
 * состояния счётчика. Это значит, что при срабатывании аппаратного таймера
 * происходит только увеличение счётчика срабатываний и никакой другой код
 * непосредственно не вызывается. Вместо этого следует проверять состояние
 * счётчика и, если он не равен нулю, выполнять действия по синхронизации.
 * Следующий пример показывает это.
 *
 * \code{.c}
 *#include "hal/mcu.h"
 *
 * int main(void)
 *{
 *        mcu_init();
 *        mcu_interrupt_unlock();
 *
 *        while (1) {
 *                if (mcu_get_timer_fire() != 0) {
 *                        // Действия при срабатывании системного таймера.
 *                        tms_tick();
 *                } else {
 *                        // Простой системы, можно подремать.
 *                }
 *        }
 *
 *        return 0;
 *}
 * \endcode
 *
 * Такой подход позволяет сократить время нахождения в обработчике прерывания
 * до минимума и в тоже время не терять синхронизацию даже если в какой-то
 * момент реакция на  срабатывание системного таймера будет долгой. То есть если
 * функция tms_tick() из примера выше однажды будет выполняться два "системных
 * тика", то затем она будет вызвана как минимум два раза подряд без пауз.
 *
 * Тем не менее, следует избегать долгой реакции, так как это точно приведёт
 * к неравномерному вызову кода обработки и может привести к переполнению
 * счетчика срабатываний.
 *
 * \date created on: 03.01.2018
 * \author Nick Egorrov
 */

#ifndef HAL_MCU_H_
#define HAL_MCU_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Инициализация микроконтроллера.
 * \details Инициализация всей аппаратной части МК. Инициализация
 * внешних устройств, таких как RTC или дисплей, должна производиться
 * отдельно.
 */
extern void mcu_init();

/**
 * \brief Блокировка всех прерываний.
 * \details Функция запрещает все прерывания и увеличивает счётчик
 * блокировки на единицу. Для разрешения прерываний каждый вызов этой
 * функции должен быть сбалансирован вызововм mcu_interrupt_unlock()
 */
extern void mcu_interrupt_lock();

/**
 * \brief Разблокировка всех прерываний.
 * \details Функция уменьшает счётчик блокировки на единицу и разрешает
 * все прерывания только если счётчик становится равен нулю. Таким
 * образом, для разрешения прерываний эта функция должна быть вызвана
 * столько же раз, что и mcu_interrupt_lock()
 */
extern void mcu_interrupt_unlock();

/**
 * \brief Показывает, были ли срабатывания системного таймера.
 * \details Модуль содержит обработчик прерывания от таймера, который
 * используется для синхронизации системы. Этот обработчик для
 * сокращения времени нахождения в прерывании просто увеличивает счётчик
 * срабатываний. Вызов данной функции возвращает значение счётчика и
 * уменьшает его на единицу.
 * \return Количество срабатываний системного таймера с момента
 * предыущего вызова функции.
 */
extern unsigned char mcu_get_timer_fire();

#ifdef __cplusplus
}
#endif

#endif /* HAL_MCU_H_ */

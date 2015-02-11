#ifndef HAL_PWMI_HL_H_
#define HAL_PWMI_HL_H_

/**
 * \file pwmi-hl.h
 * \brief Константы для модуля ШИМ.
 * \details Константы задают диаппазоны для регулировки
 *      яркости и скорости смены показаний.
 */

#define DISPLAY_BRIGHT_MAX 15
#define DISPLAY_BRIGHT_MIN 2 // 7 для логарифмического преобразования.
#define DISPLAY_RATE_MAX 7
#define DISPLAY_RATE_MIN 0

#endif // HAL_PWMI_HL_H_

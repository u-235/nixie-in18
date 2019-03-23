/**
 * @file
 * @brief
 * @details
 *
 * @date создан 13.11.2018
 * @author Nick Egorrov
 */

#ifndef USER_H_
#define USER_H_

enum virtual_keys{
        VK_NO,
        VK_MENU_DOWN,
        VK_MENU_UP,
        VK_SELECT_DOWN,
        VK_SELECT_UP,
        VK_CHANGE_DOWN,
        VK_CHANGE_UP
};

#ifdef __cplusplus
extern "C" {
#endif

extern void user_init(void);

extern uint8_t user_get_key(void);

#ifdef __cplusplus
}
#endif

#endif /* USER_H_ */

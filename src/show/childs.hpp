/**
 * @file
 * @brief
 * @details
 *
 * @date создан 05.01.2019
 * @author Nick Egorrov
 */

#ifndef SRC_SHOW_CHILDS_HPP_
#define SRC_SHOW_CHILDS_HPP_

#include "../show.hpp"

class ShowError: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
};

class ShowIntro: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
};

class ShowTime: Show {
public:
        static void on_start();
        static void on_update();
        static void on_hide();
        static void on_key(const key_t _key);
};

class ShowDate: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_key(const key_t _key);
};

class ShowAlarm: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_key(const key_t _key);
};

class ShowSetAlarm: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const key_t _key);
};

class ShowSetTime: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const key_t _key);
};

class ShowSetDate: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const key_t _key);
};

class ShowSetCaliber: Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const key_t _key);
};

#endif /* SRC_SHOW_CHILDS_HPP_ */

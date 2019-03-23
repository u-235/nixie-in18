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
#include "../config.h"

class ShowViewer: public Show {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
};

class ShowError: public ShowViewer {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
};

class ShowIntro: public ShowViewer {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
};

class ShowTime: public ShowViewer {
public:
        static void on_start();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
protected:
        static void fill();
};

class ShowDate: public ShowViewer {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
protected:
        static void fill();
};

class ShowAlarm: public ShowViewer {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_key(const uint8_t _key);
};

class ShowSetter: public ShowViewer {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
protected:
        static void refresh();
        static uint8_t flag_;
        static uint8_t state_;
};

class ShowSetAlarm: public ShowSetter {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
};

class ShowSetTime: public ShowSetter {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_sync();
        static void on_key(const uint8_t _key);
protected:
        static struct rtc_tm time_;
};

class ShowSetDate: public ShowSetter {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_hide();
        static void on_key(const uint8_t _key);
protected:
        static struct rtc_tm time_;
};

class ShowSetCaliber: public ShowSetter {
public:
        static void on_start();
        static void on_stop();
        static void on_update();
        static void on_key(const uint8_t _key);
};

#endif /* SRC_SHOW_CHILDS_HPP_ */

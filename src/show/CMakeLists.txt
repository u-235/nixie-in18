
#show

add_library(show STATIC alarm.cpp date.cpp error.cpp intro.cpp
    set.cpp set_alarm.cpp set_caliber.cpp set_date.cpp set_time.cpp
    show.cpp time.cpp)
target_link_libraries(show MAIN_MCU)

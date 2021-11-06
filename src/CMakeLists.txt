
cmake_minimum_required(VERSION 3.9)

project(nixie LANGUAGES C CXX)

set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 98)

avr_configure_mcu(atmega168
        NAME    MAIN_MCU
        DEFINES F_CPU=16000000UL
        OPTIONS -gdwarf-2 -mcall-prologues -flto -Os
            -fpack-struct -fshort-enums -ffunction-sections -fdata-sections
        LINK_OPTIONS -Wl,--gc-sections -Wl,-Map,clock.map -mrelax)

string(APPEND CMAKE_C_FLAGS   " -Wstrict-prototypes -Wmissing-prototypes")
string(APPEND CMAKE_CXX_FLAGS " -fno-exceptions -Wold-style-cast -Wnon-virtual-dtor")

add_subdirectory(hal)
add_subdirectory(show)
add_subdirectory(tms)

add_executable(nixie-in18 alarm.c sensor.c user.c main.cpp)
target_link_libraries(nixie-in18 hal show tms MAIN_MCU)

avr_get_firmware(nixie-in18)
avr_print_size(nixie-in18)


project(player LANGUAGES ASM C)

find_package(AVR REQUIRED binutils libc)

set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 90)

avr_configure_mcu(attiny861
        NAME         PLAYER_MCU
        DEFINES      F_CPU=16000000UL MODE=0
        OPTIONS      -Os -gdwarf-2 -mcall-prologues)

add_executable(player asmfunc.S main.c mmc.c pff.c)
target_link_libraries(player PLAYER_MCU)

avr_get_firmware(player)
avr_print_size(player)


cmake_minimum_required(VERSION 3.9)

project(nixie LANGUAGES ASM C CXX)

find_package(AVR REQUIRED binutils libc)

add_subdirectory(src)
add_subdirectory(player)

#avr_print_status()

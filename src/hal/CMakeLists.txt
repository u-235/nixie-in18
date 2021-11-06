
# hal

add_library(hal OBJECT display.c iic.h
    avr/adc.c avr/mcu.c avr/pwmi.c avr/spi.c
    iic/iic.c rtc/chip_m41t56.c rtc/service.c)
target_link_libraries(hal MAIN_MCU)

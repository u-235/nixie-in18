[русский](./README_RU.md)

## nixie-in18

A program for clock using IN-18 indicators.

Unlike many schemes, this uses a static display. The Atmega microcontroller
controls the indicators via the [74hc595 IC](http://www.nxp.com/documents/data_sheet/74HC_HCT595.pdf)
connected to the SPI. In addition to the IN-18, the clock has eight lights
to show the day of the week and an alarm clock, and four lights to separate
digits.

The microcontroller clocks from quartz at a frequency of 16 MHz. This is caused
by the presence of software PWM to adjust the brightness of the indicators.
The PWM frequency is selected at about 100 Hz to reduce the visibility
of flicker. The same mechanism of the program PWM allows you to make the effect
of a smooth change of indication indications.

A slightly modified [Chan player](http://elm-chan.org/works/sd20p/report.html)
is used as an alarm signal.

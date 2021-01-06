# NES Mini Controller USB Adapter for attiny85

This code will work with an attiny85 (or probably whatever similar AVR microcontroller once the Makefile are changed and the pins constants
adapted - __SDA__, __SCL__ and the two __Data__ for the __V-USB__ part) hooked to a NES Mini Controller via I2C and with the proper hardware
required for a USB connection.

* The first part acts as a __generic USB gamepad__ using the [V-USB library](https://www.obdev.at/products/vusb/index.html) (it's a basic example modified to use
a gamepad descriptor and send the proper report. Not an expert on this library, so maybe it can be improved, fixed or optimized; but as far as I'm concerned
it does the work pretty well!).

* The second part uses a custom handmade bunch of functions to communicate with the __NES Mini Controller__ via __I2C__.

    * __nesminicontrollerdrv.c__ implements the Controller poll. It reads from the proper register and fetch the bytes related to the pressed buttons.
There are a few constants for each button that can be used to check which one was pressed (it allows multiple buttons pressed at the same time).

    * __i2cattiny85/i2cattiny85.c__ contains some low-level functions to send and read bytes using the SDA and SCL lines. In this case the file
is set for the attiny85 pins, but it can be changed if another microcontroller is used (didn't try that, but in theory it should work).

## Some extra considerations

Since I'm using an __attiny85__ I needed to change a few things in order to make the __V-USB library__ work with it.
[This post helps me a lot](https://www.ruinelli.ch/how-to-use-v-usb-on-an-attiny85).

Some changes need to be done to modify the CPU speed (also fuses!) among with the use of an Oscillator Calibration function.

The input pins are also different than the ones in the post, since one of those pins is also used as the SDA signal. To avoid conflicts between the
"USB pins" and the "I2C pins" I changed one of the USB Data pins to another one. Check the _usbconfig.h_ file for more info about how to re-arrange
the 4 pins.

## Useful links
[https://www.obdev.at/products/vusb/index.html](https://www.obdev.at/products/vusb/index.html) The V-USB library, software implementation for low-speed
USB device on AVR microcontrollers

[https://www.ruinelli.ch/how-to-use-v-usb-on-an-attiny85](https://www.ruinelli.ch/how-to-use-v-usb-on-an-attiny85) How to make the original V-USB library
examples work in an attiny85

[https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/](https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/)
Pin change Interrupts on attiny85

[https://en.wikipedia.org/wiki/I%C2%B2C](https://en.wikipedia.org/wiki/I%C2%B2C) The I2C page on Wikipedia

[http://wiibrew.org/wiki/Wiimote/Extension_Controllers](http://wiibrew.org/wiki/Wiimote/Extension_Controllers) Info about the Nintendo Extension Controllers (like
the Mini consoles or the Wii Nunchuk)

[http://albertgonzalez.coffee/projects/snes_mini_arduino/](http://albertgonzalez.coffee/projects/snes_mini_arduino/) When I started tinkering with this Mini Controllers
I did some stuff using Arduinos instead of "raw" AVR's. Here are some of the notes I wrote about connecting (S)NES Mini Controllers to an Arduino board.
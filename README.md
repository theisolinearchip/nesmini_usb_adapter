# NES Mini Controller USB Adapter for attiny85

This code will work with an attiny85 (or probably whatever similar AVR microcontroller once the Makefile are changed and the pins constants adapted - __SDA__, __SCL__ and the two __Data__ for the __V-USB__ part) hooked to a NES Mini Controller via I2C (or an SNES Mini Controller too!) and with the proper hardware required for a USB connection.

* The first part acts as a __generic USB gamepad__ using the [V-USB library](https://www.obdev.at/products/vusb/index.html) (it's a basic example modified to use
a gamepad descriptor and send the proper report. Not an expert on this library, so maybe it can be improved, fixed or optimized; but as far as I'm concerned
it does the work pretty well!).

* The second part uses a custom handmade bunch of functions to communicate with the __NES Mini Controller__ via __I2C__.

    * __nesminicontrollerdrv.c__ implements the Controller poll. It reads from the proper register and fetch the bytes related to the pressed buttons.
There are a few constants for each button that can be used to check which one was pressed (it allows multiple buttons pressed at the same time).

    * __i2cattiny85/i2cattiny85.c__ contains some low-level functions to send and read bytes using the SDA and SCL lines. In this case the file
is set for the attiny85 pins, but it can be changed if another microcontroller is used (didn't try that, but in theory it should work).

## SNES Mini Controller Support

Now a SNES Mini Controller can be attached too! It works in a similar way, but need some extra init operations (fully compatible with the NES Mini one, but not required) and a more accurate timming operations (the read process on the NES Mini Controllers seems to be more "tolearant" when chainning multiple i2c operations, but the SNES Mini device requires a small delay between them).

By adding default SNES Mini support the USB device now sends TWO bytes instead of only ONE to be able to map all the SNES gamepad buttons (even if you're using a NES Mini Controller).

## Some extra considerations

Since I'm using an __attiny85__ I needed to change a few things in order to make the __V-USB library__ work with it.
[This post helps me a lot](https://www.ruinelli.ch/how-to-use-v-usb-on-an-attiny85).

Some changes need to be done to modify the CPU speed (also fuses!) among with the use of an Oscillator Calibration function.

The input pins are also different than the ones in the post, since one of those pins is also used as the SDA signal. To avoid conflicts between the
"USB pins" and the "I2C pins" I changed one of the USB Data pins to another one. Check the _usbconfig.h_ file for more info about how to re-arrange
the 4 pins.

## TODO

* Distinction between SNES and NES Mini Controllers in order to avoid doing unnecessary stuff (like the SNES init for the NES Mini Controller or the 2 bytes report instead of a single byte). Maybe with a "manual switch" in the board?
* Auto-detect when the controller has been plugged/unplugged once the device is already powered on (now you need to attach the controller **first** and then plug the usb adapter, otherwise won't work because of the snes init operation)
* Check the board design, probably some pull-up resistors for the i2c bus are required (there's some kind of "deadlock" when turning the device on without any controller attached)
* Add more controllers? Probably out of the scope of this particular project...
* Naming? It seems confusing to have a "NES project" that also supports SNES stuff and have a function called "snes_init"...

## Useful links
[https://hackaday.io/project/176939-nes-mini-controller-usb-adapter-with-attiny85](https://hackaday.io/project/176939-nes-mini-controller-usb-adapter-with-attiny85) This project on my hackaday.io page (with schematics and more info)

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

[http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85](http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85) Fuse calculator

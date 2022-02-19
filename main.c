/*
	Based on one of the multiple examples for V-USB implementation.

	This file uses a generic gamepad USB descriptor to create a standard
	device using the V-USB library. It also uses the nesminicontrollerdrv.c
	functions to enable communication with a NES Mini Controller attached
	to the proper SDA and SCL pins on the attiny85.

	(I'm pretty sure it can	work in other microcontrollers with some changes
	like pin numbers and model name on the Makefile, but I only tested it
	in a bunch of at85)

	Check the usbconfig.h file, since one of the pins for the V-USB Data is
	different (it's the same pin used by the SDA signal on the I2C part, so
	I changed the interrupts to use that pin instead of the original one)

	The nesminicontrollerdrv.c uses a small I2C library that uses the two-wire mode
	(more on this here: https://github.com/theisolinearchip/i2c_attiny85_twi).
	Those files are located on the i2cattiny85/ folder

	----

	The V-USB library is under a GPL 2: https://www.obdev.at/products/vusb/license.html
*/

#define LED_PIN	4

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"

#include "nesminicontrollerdrv.c"

// also change USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH on usbconfig.h
PROGMEM const char usbHidReportDescriptor[27] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Mouse)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM
    0x29, 0x0C,                    //   USAGE_MAXIMUM
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x0C,                    //   REPORT_COUNT (12)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x04,                    //   REPORT_COUNT (4), padding to reach 2 bytes
	0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xC0,                          // END_COLLECTION
};

static snes_report_t report_buffer;
static snes_controller_state controller_state = { 0, 0 };

// implementation required, but not used
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	return 0;
}

int __attribute__((noreturn)) main(void) {

	DDRB |= (1 << LED_PIN);

	uchar i;

	wdt_enable(WDTO_1S);
	
	usbInit();
	usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
	i = 0;
	while(--i) {             /* fake USB disconnect for > 250 ms */
		wdt_reset(); 
		_delay_ms(1);
	}

	usbDeviceConnect();
	sei();

	// i2c_init, basically
	snes_init();

	// snes first connect attempt (will set the connected flag to 1/0)
	snes_connect(&controller_state);

	while(1) {
		wdt_reset();
		usbPoll();
		if (usbInterruptIsReady()) {
			// called after every poll of the interrupt endpoint

			if (controller_state.connected) {
				// fetch (or try to) only if connected (in the snes the connection
				// doesn't mind the proper initialization, so if we try to fetch always
				// then an effective 0x00 will be read without the init, so force
				// snes_connect everytime the connection is lost)
				snes_get_state(&controller_state);
			} else {
				PORTB |= (1 << LED_PIN);
				snes_connect(&controller_state);
				if (controller_state.connected) PORTB &= ~(1 << LED_PIN);
			}

			snes_set_report_buttons(&controller_state, &report_buffer);

			usbSetInterrupt((void *)&report_buffer, sizeof(report_buffer));
		}

		// set led if some key was preset (outside the USB interrupt block)
		if (controller_state.connected) {
			if (controller_state.buttons) {
				PORTB |= (1 << LED_PIN);
			} else {
				PORTB &= ~(1 << LED_PIN);
			}
		}
	}
}
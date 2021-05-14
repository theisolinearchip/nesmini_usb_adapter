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

	The nesminicontrollerdrv.c uses the functions from the i2cattiny85/i2cattiny85.c
	file. It's a custom homemade implementation for basic read/write on SDA/SCL.

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

typedef struct{
	uchar   commonButtonMask;	// 8 buttons, D-pad, A, B, SELECT, START
	uchar   snesButtonMask;		// X, Y, L, R (SNES only), the last bits are not used
}report_t;

static report_t reportBuffer;

static uchar    idleRate;   /* repeat rate for keyboards, never used for mice */

static uint16_t nes_controller_state;

// implementation required, but not used
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	return 0;
}

void process_buttons() {
	reportBuffer.commonButtonMask = reportBuffer.snesButtonMask = 0x00;

	// UP 0
	// RIGHT 1
	// DOWN 2 
	// LEFT 3 
	// SELECT 4
	// START 5
	// B 6
	// A 7
	// X 8 (SNES only)
	// Y 9 (SNES only)
	// L 10 (SNES only)
	// R 11 (SNES only)
	//
	// wanna read an EXACT MATCH without any other buttons?
	// use (!(nes_controller_state ^ NES_BUTTON_SELECT)) instead

	if (nes_controller_state & NES_BUTTON_UP) reportBuffer.commonButtonMask = 0x01;
	if (nes_controller_state & NES_BUTTON_RIGHT) reportBuffer.commonButtonMask |= (0x01 << 1);
	if (nes_controller_state & NES_BUTTON_DOWN) reportBuffer.commonButtonMask |= (0x01 << 2);
	if (nes_controller_state & NES_BUTTON_LEFT) reportBuffer.commonButtonMask |= (0x01 << 3);

	if (nes_controller_state & NES_BUTTON_SELECT) reportBuffer.commonButtonMask |= (0x01 << 4);
	if (nes_controller_state & NES_BUTTON_START) reportBuffer.commonButtonMask |= (0x01 << 5);

	if (nes_controller_state & NES_BUTTON_B) reportBuffer.commonButtonMask |= (0x01 << 6);
	if (nes_controller_state & NES_BUTTON_A) reportBuffer.commonButtonMask |= (0x01 << 7);

	if (nes_controller_state & NES_BUTTON_X) reportBuffer.snesButtonMask = 0x01;
	if (nes_controller_state & NES_BUTTON_Y) reportBuffer.snesButtonMask |= (0x01 << 1);

	if (nes_controller_state & NES_BUTTON_L) reportBuffer.snesButtonMask |= (0x01 << 2);
	if (nes_controller_state & NES_BUTTON_R) reportBuffer.snesButtonMask |= (0x01 << 3);

}

int __attribute__((noreturn)) main(void) {

	DDRB |= (1 << LED_PIN);

	uchar i;
	nes_controller_state = 0;

	// snes pre-read init, nes controller works fine with it
	snes_init();

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
	while(1) {
		wdt_reset();
		usbPoll();
		if(usbInterruptIsReady()) {
			/* called after every poll of the interrupt endpoint */

			nes_controller_state = nes_get_state();
			process_buttons();

			usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
		}

		// set led if some key was preset (outside the USB interrupt block)
		if (nes_controller_state) {
			PORTB |= (1 << LED_PIN);
		} else {
			PORTB &= ~(1 << LED_PIN);
		}
	}
}
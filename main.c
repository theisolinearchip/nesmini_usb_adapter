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
PROGMEM const char usbHidReportDescriptor[23] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Mouse)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM
    0x29, 0x08,                    //   USAGE_MAXIMUM
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xC0,                          // END_COLLECTION
};

typedef struct{
	uchar   buttonMask; // 8 buttons, 1 bit per button
}report_t;

static report_t reportBuffer;

static uchar    idleRate;   /* repeat rate for keyboards, never used for mice */

static uint16_t nes_controller_state;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

	/* The following requests are never used. But since they are required by
	 * the specification, we implement them in this example.
	 */
	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
		if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			/* we only have one report type, so don't look at wValue */
			usbMsgPtr = (void *)&reportBuffer;
			return sizeof(reportBuffer);
		}else if(rq->bRequest == USBRQ_HID_GET_IDLE){
			usbMsgPtr = &idleRate;
			return 1;
		}else if(rq->bRequest == USBRQ_HID_SET_IDLE){
			idleRate = rq->wValue.bytes[1];
		}
	}else{
		/* no vendor specific requests implemented */
	}
	return 0;   /* default for not implemented requests: return no data back to host */
}

void process_buttons() {
	reportBuffer.buttonMask = 0x00;

	// UP 0
	// RIGHT 1
	// DOWN 2 
	// LEFT 3 
	// SELECT 4
	// START 5
	// B 6
	// A 7
	//
	// wanna read an EXACT MATCH without any other buttons?
	// use (!(nes_controller_state ^ NES_BUTTON_SELECT)) instead

	if (nes_controller_state & NES_BUTTON_UP) reportBuffer.buttonMask = 0x01;
	if (nes_controller_state & NES_BUTTON_RIGHT) reportBuffer.buttonMask |= (0x01 << 1);
	if (nes_controller_state & NES_BUTTON_DOWN) reportBuffer.buttonMask |= (0x01 << 2);
	if (nes_controller_state & NES_BUTTON_LEFT) reportBuffer.buttonMask |= (0x01 << 3);

	if (nes_controller_state & NES_BUTTON_SELECT) reportBuffer.buttonMask |= (0x01 << 4);
	if (nes_controller_state & NES_BUTTON_START) reportBuffer.buttonMask |= (0x01 << 5);

	if (nes_controller_state & NES_BUTTON_B) reportBuffer.buttonMask |= (0x01 << 6);
	if (nes_controller_state & NES_BUTTON_A) reportBuffer.buttonMask |= (0x01 << 7);
}

int __attribute__((noreturn)) main(void) {

	DDRB |= (1 << LED_PIN);

	uchar i;
	nes_controller_state = 0;

	wdt_enable(WDTO_1S);
	/* Even if you don't use the watchdog, turn it off here. On newer devices,
	 * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
	 */
	/* RESET status: all port bits are inputs without pull-up.
	 * That's the way we need D+ and D-. Therefore we don't need any
	 * additional hardware initialization.
	 */
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
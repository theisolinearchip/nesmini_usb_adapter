/*
	Button handling for the	NES Mini Controllers (SNES Mini Controller also supported)
	File written by Albert Gonzalez (albertgonzalez.coffee)

	-------

	Tested only on an attiny85,
	using PB0 as SDA and PB2 as SCL

	It's mostly based on the "Example of bit-banging the
	I2C master protocol" section from the Wikipedia I2C page:
	https://en.wikipedia.org/wiki/I%C2%B2C
*/

#ifndef NESMiniControllerDriver_c
#define NESMiniControllerDriver_c

#include "i2cattiny85.c"

// when we see 0x52 as the address (usually on Arduino environments with I2C scanners,
// the Wire library and other stuff) we're talking about the first 7 bits, BUT we need
// to send an 8 bit in order to perform a WRITE operation (0) or a READ operation (1)
// 0x52 << 1 + (1 or 0) gets 0xA4 for writing and 0xA5 for reading
#define NES_I2C_ADDRESS_WRITE 0xA4
#define NES_I2C_ADDRESS_READ 0xA5

#define NES_BUTTON_UP 0x0001
#define NES_BUTTON_RIGHT 0x8000
#define NES_BUTTON_DOWN 0x4000
#define NES_BUTTON_LEFT 0x0002
#define NES_BUTTON_A 0x0010
#define NES_BUTTON_B 0x0040
#define NES_BUTTON_X 0x0008 // SNES Mini Only
#define NES_BUTTON_Y 0x0020 // SNES Mini Only
#define NES_BUTTON_L 0x2000 // SNES Mini Only
#define NES_BUTTON_R 0x0200 // SNES Mini Only
#define NES_BUTTON_START 0x0400
#define NES_BUTTON_SELECT 0x1000

void snes_init() {

	// According to http://wiibrew.org/wiki/Wiimote/Extension_Controllers the way to initialize the
	// SNES Mini Controller is by writting 0x55 to 0xF0 and 0x00 to 0xFB BUT it seems it works only
	// with the first write. The NES Mini does not require the init, but works anyway with it

	i2c_start();
	i2c_write_byte(NES_I2C_ADDRESS_WRITE); // 0x52
	i2c_write_byte(0xF0); // "address"
	i2c_write_byte(0x55); // info to write
	i2c_stop();
}

uint16_t nes_get_state() {
	i2c_start();

	i2c_write_byte(NES_I2C_ADDRESS_WRITE); // write
	i2c_write_byte(0x00); // we're gonna read from 0x00
	i2c_stop();

	_delay_ms(10); // the nes mini controller seems to work fine without this delay

	i2c_start();

	i2c_write_byte(NES_I2C_ADDRESS_READ); // read

	// read 6 bytes, use only the last two ones
	// (need to "read" the first 4 bytes in order
	// to "advance" to the last two ones)
	uint16_t state = 0;
	uint8_t read = 0;
	for (char x = 0; x < 6; x++) {
		read = i2c_read_byte(x >= 5); // nack ("not gonna ask for more" / "stop" / "omgexplosions" when fetching the last one)

		if (x >= 4) {
			read ^= 0xFF; // "255 - read"
			state |= read;
			if (x == 4) state <<= 8;
		}
	}

	i2c_stop();

	return state;
}

#endif
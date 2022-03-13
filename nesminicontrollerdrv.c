/*
	Button handling for the	NES Mini Controllers (SNES Mini Controller also supported)
	File written by Albert Gonzalez (albertgonzalez.coffee)
*/

#ifndef NESMiniControllerDriver_c
#define NESMiniControllerDriver_c

#include "i2c_primary.c"

// old I2C library deprecated
// #include "i2cattiny85.c"

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

// current controller status (buttons pressed, is_connected? etc.)
typedef struct{
	uint16_t	buttons;
	uchar		connected;
}snes_controller_state;

// report struct for the gamepad
typedef struct{
	uchar   commonButtonMask;	// 8 buttons, D-pad, A, B, SELECT, START
	uchar   snesButtonMask;		// X, Y, L, R (SNES only), the last bits are not used
}snes_report_t;

static void snes_init() {
	i2c_init();
}

static void snes_connect(snes_controller_state *state) {

	// According to http://wiibrew.org/wiki/Wiimote/Extension_Controllers the way to initialize the
	// SNES Mini Controller is by writting 0x55 to 0xF0 and 0x00 to 0xFB BUT it seems it works only
	// with the first write. The NES Mini does not require the init, but works anyway with it

	i2c_start();
	
	if (i2c_write_byte(NES_I2C_ADDRESS_WRITE) & 0x01) (*state).connected = 0;
	else (*state).connected = 1;

	i2c_write_byte(0xF0); // "address"
	i2c_write_byte(0x55); // info to write
	i2c_stop();
}

static void snes_get_state(snes_controller_state *state) {
	i2c_start();

	if (i2c_write_byte(NES_I2C_ADDRESS_WRITE) & 0x01) (*state).connected = 0;
	else (*state).connected = 1;

	i2c_write_byte(0x00); // we're gonna read from 0x00
	i2c_stop();

	if (!(*state).connected) {
		(*state).buttons = 0;
		return;
	}

	_delay_ms(5); // the nes mini controller seems to work fine without this delay

	i2c_start();

	i2c_write_byte(NES_I2C_ADDRESS_READ); // read

	// read 6 bytes, use only the last two ones
	// (need to "read" the first 4 bytes in order
	// to "advance" to the last two ones)
	uint16_t buttons = 0;
	uint8_t read = 0;
	for (char x = 0; x < 6; x++) {
		read = i2c_read_byte((x >= 5) ? 0xFF : 0x00); // nack ("not gonna ask for more" / "stop" / "omgexplosions" when fetching the last one)

		if (x >= 4) {
			read ^= 0xFF; // "255 - read"
			buttons |= read;
			if (x == 4) buttons <<= 8;
		}
	}

	i2c_stop();

	(*state).buttons = buttons;
}

static void snes_set_report_buttons(snes_controller_state *state, snes_report_t *report) {
	(*report).commonButtonMask = (*report).snesButtonMask = 0x00;

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
	// use (!(controller_state.buttons ^ NES_BUTTON_SELECT)) instead

	if ((*state).buttons & NES_BUTTON_UP) (*report).commonButtonMask = 0x01;
	if ((*state).buttons & NES_BUTTON_RIGHT) (*report).commonButtonMask |= (0x01 << 1);
	if ((*state).buttons & NES_BUTTON_DOWN) (*report).commonButtonMask |= (0x01 << 2);
	if ((*state).buttons & NES_BUTTON_LEFT) (*report).commonButtonMask |= (0x01 << 3);

	if ((*state).buttons & NES_BUTTON_SELECT) (*report).commonButtonMask |= (0x01 << 4);
	if ((*state).buttons & NES_BUTTON_START) (*report).commonButtonMask |= (0x01 << 5);

	if ((*state).buttons & NES_BUTTON_B) (*report).commonButtonMask |= (0x01 << 6);
	if ((*state).buttons & NES_BUTTON_A) (*report).commonButtonMask |= (0x01 << 7);

	if ((*state).buttons & NES_BUTTON_X) (*report).snesButtonMask = 0x01;
	if ((*state).buttons & NES_BUTTON_Y) (*report).snesButtonMask |= (0x01 << 1);

	if ((*state).buttons & NES_BUTTON_L) (*report).snesButtonMask |= (0x01 << 2);
	if ((*state).buttons & NES_BUTTON_R) (*report).snesButtonMask |= (0x01 << 3);
}

#endif
/*
	A file with I2C basic raw operations
	(read/write sda/scl) for master devices written
	by Albert Gonzalez (albertgonzalez.coffee)

	-------

	Tested only on an attiny85, using
		PB0 as SDA
		PB2 as SCL

	It's mostly based on the "Example of bit-banging the
	I2C master protocol" section from the Wikipedia I2C page:
	https://en.wikipedia.org/wiki/I%C2%B2C
*/

#ifndef I2Cattiny85_c
#define I2Cattiny85_c

#include <avr/io.h>
#include <util/delay.h>

#define I2C_ATTINY85_PORT_SDA	0 // on io.h for attiny85 it's the PB0
#define I2C_ATTINY85_PORT_SCL	2

#define I2C_ATTINY85_PIN_SDA	0 // it's the same!
#define I2C_ATTINY85_PIN_SCL	2

uint8_t i2c_read_sda() {
	return (PINB & (1 << I2C_ATTINY85_PIN_SDA)) != 0 ? 1 : 0;
}

uint8_t i2c_read_scl() {
	return (PINB & (1 << I2C_ATTINY85_PIN_SCL)) != 0 ? 1 : 0;
}

void i2c_set_sda() {
	// logic 1 'cause open drain
	DDRB &= ~(1 << I2C_ATTINY85_PORT_SDA); // port as input
	PORTB &= ~(1 << I2C_ATTINY85_PORT_SDA); // disable internal pullup register
}

void i2c_set_scl() {
	DDRB &= ~(1 << I2C_ATTINY85_PORT_SCL);
	PORTB &= ~(1 << I2C_ATTINY85_PORT_SCL);
}

void i2c_clear_scl() {
	// logic 0 (we're active pulling down the line by setting the 1)
	DDRB |= (1 << I2C_ATTINY85_PORT_SDA); // port as output
	PORTB &= ~(1 << I2C_ATTINY85_PORT_SDA); // set port to 1 (read as logic 0)
}

void clear_scl() {
	DDRB |= (1 << I2C_ATTINY85_PORT_SCL);
	PORTB &= ~(1 << I2C_ATTINY85_PORT_SCL);
}

void i2c_delay() {
	_delay_ms(1);
}

// ---------------------

void i2c_start() {
	i2c_set_sda();
	i2c_set_scl();
	i2c_delay();
	i2c_clear_scl();
	clear_scl();
}

void i2c_stop() {
	i2c_clear_scl();
	i2c_delay();

	i2c_set_scl();
	while (i2c_read_scl() == 0) { }

	i2c_delay();

	//now clock is high, set sda high too
	i2c_set_sda();
}

void i2c_write_single_bit(uint8_t bit) {
	if (bit) i2c_set_sda();
	else i2c_clear_scl();

	i2c_set_scl(); // clock high to indicate a new valid sda value
	while (i2c_read_scl() == 0) { }

	clear_scl(); // pull clock low once it's finished
}

uint8_t i2c_read_single_bit() {
	i2c_set_sda(); // send 0, so the slave device handles whatever they wanna handle

	i2c_set_scl(); // clock high to indicate
	while (i2c_read_scl() == 0) { }

	uint8_t bit = i2c_read_sda(); // read the pin value (set by slave)

	clear_scl(); // pull clock low once it's finished

	return bit;
}

// return 0 if slave sends ACK
uint8_t i2c_write_byte(uint8_t byte) {
	for (uint8_t bit = 0; bit < 8; ++bit) {
		i2c_write_single_bit((byte & 0x80) != 0);
		byte <<= 1;
	}

	uint8_t nack = i2c_read_single_bit(); 
	return nack;
}

uint8_t i2c_read_byte(uint8_t nack) {
	uint8_t byte = 0;
	for (uint8_t bit = 0; bit < 8; ++bit) {
		byte = (byte << 1) | i2c_read_single_bit();
	}
	i2c_write_single_bit(nack); // send 0 if we wanna read more
	return byte;
}

#endif
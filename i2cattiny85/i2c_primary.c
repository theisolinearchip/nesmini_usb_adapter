#include "i2c_primary.h"

void i2c_init() {

	USIDR = 0xFF;

	USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK);

	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) |
			(1 << USIDC) |    // Clear flags,
			(0x0 << USICNT0); // and reset counter.

	DDRB |= (1 << PIN_SDA); // output (after enabling the two-wire mode)
	DDRB |= (1 << PIN_SCL); // output

}

void i2c_start() {

	// generate start condition
	PORTB |= (1 << PIN_SDA); // sda released
	PORTB |= (1<<PIN_SCL); // scl release until high
	while (!(PORTB & (1<<PIN_SCL)));

	PORTB &= ~(1<<PIN_SDA); // sda low (start condition)

	_delay_us(WAIT_LONG);

	PORTB &= ~(1<<PIN_SCL); // scl low	

	PORTB |= (1<<PIN_SDA); // sda high (release to start transmitting)

}

void i2c_stop() {

	// SDA goes low
	PORTB &= ~(1<<PIN_SDA);

	// release SCL
	PORTB |= (1<<PIN_SCL);
	while (! (PINB & (1<<PIN_SCL)));

	_delay_us(WAIT_LONG);

	// release SDA
	PORTB |= (1<<PIN_SDA);
	
	_delay_us(WAIT_SHORT);

}

unsigned char i2c_transfer(unsigned char usisr_mask) {

	// force SDL low (it's already low probably, since we're
	// toggling it up and down in pairs, but just in case...)
	PORTB &= ~(1<<PIN_SCL);

	USISR = usisr_mask;

	// transfer until counter overflow
	do {
		_delay_us(WAIT_LONG);
		USICR |= (1 << USITC);
		while (! (PINB & (1<<PIN_SCL))); //Waiting for SCL to go high
		_delay_us(WAIT_SHORT);
		USICR |= (1 << USITC);
	} while (!(USISR & (1 << USIOIF)));
	_delay_us(WAIT_LONG);

	// release SDA
	// "The output pin (DO or SDA, depending on the wire mode)
	// is connected via the output latch to the most significant
	// bit (bit 7) of the USI Data Register."
	// (so write 1 to set SDA high before the next operation,
	// otherwise it'll be pulled down and read may not work)
	unsigned char temp = USIDR;
	USIDR = 0xFF; // 0x80 will work too (bit 7)

	return temp; // previous USIDR copy

}

// controller sends a byte to the bus
// returns 0 if there's a valid nack, otherwise 1
unsigned char i2c_write_byte(unsigned char data) {
	USIDR = data;
	i2c_transfer(USISR_CLOCK_8_BITS);

	// wait for ack
	DDRB &= ~(1<< PIN_SDA); // to input
	unsigned char nack = i2c_transfer(USISR_CLOCK_1_BIT);
	DDRB |= (1 << PIN_SDA);

	return nack;
}

// controller reads 1 byte from the bus
// and sends a nack if wanna read another one
// (1 = will read another one, 0 = stop sending)
// returns the read byte
unsigned char i2c_read_byte(unsigned char nack) {

	DDRB &= ~(1<< PIN_SDA); // reciving, so change to input
	unsigned char data = i2c_transfer(USISR_CLOCK_8_BITS);
	DDRB |= (1 << PIN_SDA);

	// send nack
	USIDR = nack;
	i2c_transfer(USISR_CLOCK_1_BIT);

	return data;
}
/*
	A raw and not accurate way of sending some debug lines using serial communication.
	This snippet uses a TX pin to send raw bytes in a fixed speed. There's no error checking
	nor any clock / speed / transfer / whatever control, so it may fail in some complex
	transactions timing or complexity is more critical than the few times I've used it.

	Wanna try a version that uses interrupts? This post talks about it! https://marcelmg.github.io/software_uart/
*/

#ifndef UARTRaw_c
#define UARTRaw_c

#include <stdlib.h>

#define UART_TX_PIN 4 // ol' led pin

#define DELAY_US 104 // 1/9600 = 104 (other close values may work better)

void uart_raw_init() {
	DDRB |= (1 << UART_TX_PIN); // port as output
}

void uart_raw_tx_1() {
	PORTB |= (1 << UART_TX_PIN); // set port to 1
}
void uart_raw_tx_0() {
	PORTB &= ~(1 << UART_TX_PIN); // set port to 0
}

void uart_raw_send_byte(unsigned char byte) {
	// bit 1 to start transaction
	uart_raw_tx_0();
	_delay_us(DELAY_US);

	for (unsigned char i = 0; i < 8; i++) {
		if (byte & 0x01) uart_raw_tx_1();
		else uart_raw_tx_0();
		_delay_us(DELAY_US);
		byte >>= 1;
	}

	// stop
	uart_raw_tx_1();
	_delay_us(DELAY_US);
}

void uart_raw_send_string(char *string, unsigned char length) {
	for (unsigned char i = 0; i < length; i++) {
		uart_raw_send_byte(string[i]);
	}
}

// append a \n\r at the end of the string
void uart_raw_send_line(char *string, unsigned char length) {
	char *string_2;
	char length_2 = length + 2;

	string_2 = malloc(length_2);
	strcat(string_2, string);
	strcat(string_2, "\n\r");
	uart_raw_send_string(string_2, length_2);
	free(string_2);
}

// borrowed from the log functions from the v-usb library
char hex_ascii(char h) {
    h &= 0xf;
    if(h >= 10)
        h += 'a' - (char)10 - '0';
    h += '0';
    return h;
}

void uart_raw_send_hex_line(char *string, unsigned char length) {
	for (unsigned char i = 0; i < length; i++) {
		uart_raw_send_byte(hex_ascii(string[i] >> 4));
		uart_raw_send_byte(hex_ascii(string[i]));
	}
	uart_raw_send_byte('\n');
	uart_raw_send_byte('\r');
}

#endif
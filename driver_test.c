#include <avr/io.h>
#include <util/delay.h> 

#include "nesminicontrollerdrv.c"

// http://eleccelerator.com/fusecalc/fusecalc.php?chip=attiny85
// with low fuse at E1 (CKSEL 0001) we're working at 16mhz (according to datasheet - High Frequency PLL Clock)

int main(void) {
	DDRB |= (1 << PB4);
	PORTB &= ~(1 << PB4);

	while(1) {

		int16_t read = nes_get_state();
		if (read == (NES_BUTTON_UP | NES_BUTTON_A) ) {
			PORTB |= (1 << PB4);
		} else {
			PORTB &= ~(1 << PB4);
		}

	}

}
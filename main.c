/*
* Light_WS2812 library example - RGB_blinky
*
* cycles one LED through red, green, blue
*
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Light_WS2812/light_ws2812.h>
#include <i2c/i2c_slave_defs.h>
#include <i2c/i2c_machine.h>

volatile uint8_t i2c_reg[I2C_N_REG];
const uint8_t i2c_w_mask[I2C_N_REG];

struct cRGB led[1];

int main(void)
{

	#ifdef __AVR_ATtiny10__
	CCP=0xD8;		// configuration change protection, write signature
	CLKPSR=0;		// set cpu clock prescaler =1 (8Mhz) (attiny 4/5/9/10)
	#else
	CLKPR=_BV(CLKPCE);
	CLKPR=0;			// set clock prescaler to 1 (attiny 25/45/85/24/44/84/13/13A)
	#endif

	i2c_init();
	sei();


	while(1)
	{
		led[0].r=255;led[0].g=00;led[0].b=0;    // Write red to array
		ws2812_setleds(led,1);
		_delay_ms(500);                         // wait for 500ms.

		led[0].r=0;led[0].g=255;led[0].b=0;			// green
		ws2812_setleds(led,1);
		_delay_ms(500);

		led[0].r=0;led[0].g=00;led[0].b=255;		// blue
		ws2812_setleds(led,1);
		_delay_ms(500);
	}
}

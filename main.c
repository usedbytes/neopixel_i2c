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

volatile uint8_t i2c_reg[I2C_N_REG] = { 0, 128, 0, 128 };

struct cRGB led[1];

int main(void)
{
	char flag = 1;

	#ifdef __AVR_ATtiny10__
	CCP=0xD8;		// configuration change protection, write signature
	CLKPSR=0;		// set cpu clock prescaler =1 (8Mhz) (attiny 4/5/9/10)
	#else
	CLKPR=_BV(CLKPCE);
	CLKPR=0;			// set clock prescaler to 1 (attiny 25/45/85/24/44/84/13/13A)
	#endif

	DDRB = 0x02;
	PORTB = 0x00;
	DDRD = 0x01;
	PORTD = 0;

	i2c_init();
	sei();

	while(1)
	{
		if (i2c_check_stop()) {
			if (REG_CTRL & CTRL_GLB) {
				led[0].r = REG_GLB_R;
				led[0].g = REG_GLB_G;
				led[0].b = REG_GLB_B;
				ws2812_setleds(led, 1);
				flag = 1;
			} else if (flag) {
				led[0].r = 0;
				led[0].g = 0;
				led[0].b = 0;
				ws2812_setleds(led, 1);
				flag = 0;
			}
		}
	}
}

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

#define N_LEDS 16

inline void set_leds_global(void)
{
	ws2812_setleds_constant((uint8_t *)&REG_GLB_G, 16);
}

inline void update_leds(void)
{

}

int main(void)
{
	DDRD = 0x1;

	i2c_init();
	sei();

	set_leds_global();

	while(1)
	{
		if (i2c_check_stop()) {
			if (REG_CTRL & CTRL_GLB)
				set_leds_global();
			else
				update_leds();
		}
	}
}

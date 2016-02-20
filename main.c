/*
* Neopixel I2C Slave application
*
* At boot, scrolls a bright spot (init_color) along the array
* Will stop as soon as an i2c transaction is received
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <Light_WS2812/light_ws2812.h>
#include <i2c/i2c_slave_defs.h>
#include <i2c/i2c_machine.h>

volatile uint8_t i2c_reg[I2C_N_REG];
const uint8_t init_color[N_CMP] PROGMEM = { 0x80, 0x80, 0x80 };

inline void set_leds_global(void)
{
#ifdef WS2812_RGBW
	ws2812_setleds_constant_rgbw((struct cRGBW *)&REG_GLB_G, N_LEDS);
#else
	ws2812_setleds_constant((struct cRGB *)&REG_GLB_G, N_LEDS);
#endif
}

inline void update_leds(void)
{
	ws2812_sendarray(i2c_reg + I2C_N_GLB_REG, N_LEDS * N_CMP);
}

void do_reset(void)
{
	char i = N_LEDS * N_CMP;
	volatile uint8_t *p = i2c_reg + I2C_N_GLB_REG;

	cli();
	REG_GLB_G = 0;
	REG_GLB_R = 0;
	REG_GLB_B = 0;
#ifdef WS2812_RGBW
	ws2812_setleds_constant_rgbw((struct cRGBW *)&REG_GLB_G, N_LEDS);
#else
	ws2812_setleds_constant((struct cRGB *)&REG_GLB_G, N_LEDS);
#endif
	REG_GLB_G = pgm_read_byte(init_color);
	REG_GLB_R = pgm_read_byte(init_color + 1);
	REG_GLB_B = pgm_read_byte(init_color + 2);
	REG_CTRL = 0;

	/* Reset the registers or we'll just go back to the old values! */
	while (i--) {
		*p = 0;
		p++;
	}

	sei();
}

void swirly(void)
{
	uint8_t led = N_LEDS;
	volatile uint8_t *p = i2c_reg + I2C_N_GLB_REG + (N_LEDS * N_CMP);
	uint8_t g = pgm_read_byte(init_color);
	uint8_t r = pgm_read_byte(init_color + 1);
	uint8_t b = pgm_read_byte(init_color + 2);
#ifdef WS2812_RGBW
	uint8_t w = pgm_read_byte(init_color + 3);
#endif
	uint8_t tmp;

	/* Initialise a bright spot with a tail:
	 * { 255, 127, 63, 31, 15, 15, 15, 15 ... }
	 */
	while (led--) {
#ifdef WS2812_RGBW
		*(--p) = w;
		if (w & 0xf0)
			w >>= 1;
#endif

		*(--p) = b;
		if (b & 0xf0)
			b >>= 1;

		*(--p) = r;
		if (r & 0xf0)
			r >>= 1;

		*(--p) = g;
		if (g & 0xf0)
			g >>= 1;
	}

	/* Shuffle the bright spot along */
	g = pgm_read_byte(init_color);
	r = pgm_read_byte(init_color + 1);
	b = pgm_read_byte(init_color + 2);
#ifdef WS2812_RGBW
	w = pgm_read_byte(init_color + 3);
#endif
	while (1)
	{
		update_leds();

		led = N_LEDS;
		p = &i2c_reg[I2C_N_GLB_REG];
		while (led--) {
			tmp = *p;
			*(p++) = g;
			if (led)
				g = tmp;

			tmp = *p;
			*(p++) = r;
			if (led)
				r = tmp;

			tmp = *p;
			*(p++) = b;
			if (led)
				b = tmp;

#ifdef WS2812_RGBW
			tmp = *p;
			*(p++) = w;
			if (led)
				w = tmp;
#endif
		}

		/* As soon as there's a transaction to handle, bail out */
		tmp = 100;
		while (tmp--) {
			if (i2c_check_stop())
				return;
			_delay_ms(1);
		}
	}
}

int main(void)
{
	DDRB = (1 << 3);

	i2c_init();
	sei();

	swirly();
	goto inner;

	while(1)
	{
		if (i2c_check_stop()) {
inner:
			if (REG_CTRL & CTRL_RST)
				do_reset();
			else if (REG_CTRL & CTRL_GLB)
				set_leds_global();
			else
				update_leds();
		}
	}
}

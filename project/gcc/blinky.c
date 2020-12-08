#include <stdlib.h>
#include <libopencm3/gd32/rcc.h>
#include <libopencm3/gd32/gpio.h>

#include "systick.h"

void setup_clock(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
}

void setup_gpio(void)
{
	/* LED */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
		        GPIO_PUPD_NONE,
		        GPIO4);
	gpio_clear(GPIOA, GPIO4);
}

int main(void)
{
	setup_clock();
	setup_gpio();
	setup_systick();

	while(true) {
		gpio_clear(GPIOA, GPIO4);
		mdelay(1000);
		gpio_set(GPIOA, GPIO4);
		mdelay(1000);
	}
}

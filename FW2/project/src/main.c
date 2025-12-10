#include "at32f425_wk_config.h"
#include "wk_gpio.h"
#include "wk_system.h"

void LED_Init(void);

int main(void)
{
    wk_system_clock_config();
    wk_periph_clock_config();
    wk_nvic_config();
    wk_timebase_init();
    wk_gpio_config();
    LED_Init();
    
    while(1)
    {
      gpio_bits_write(GPIOC, GPIO_PINS_5, 0);
      //wk_delay_ms(1000);
      //gpio_bits_write(GPIOC, GPIO_PINS_5, 1);
    }
}

void LED_Init(void) {
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_5;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);
}
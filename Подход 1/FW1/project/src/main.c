#include "at32f425_wk_config.h"
#include "wk_gpio.h"
#include "wk_system.h"
#include "at32f425.h"

#define BOOT_FLAG_ADDRESS   (0x08000000 + (64 * 1024) - 2048)

void LED_Init(void);
void write_boot_flag(uint32_t value);

void main(void)
{
    wk_system_clock_config();
    wk_periph_clock_config();
    wk_nvic_config();
    wk_timebase_init();
    wk_gpio_config();
    LED_Init();
    
    write_boot_flag(1);

    while(1)
    {
      gpio_bits_write(GPIOC, GPIO_PINS_3, 0);
      wk_delay_ms(500);
      gpio_bits_write(GPIOC, GPIO_PINS_3, 1);
      wk_delay_ms(500);
    }
}

void LED_Init(void) {
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_3;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);
}

void write_boot_flag(uint32_t value) {
    flash_unlock();
    flash_sector_erase(BOOT_FLAG_ADDRESS);
    while(flash_flag_get(FLASH_ODF_FLAG) == RESET);
    flash_flag_clear(FLASH_ODF_FLAG);
    flash_word_program(BOOT_FLAG_ADDRESS, value);
    flash_lock();
}
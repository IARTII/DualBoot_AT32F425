#include "at32f425_wk_config.h"
#include "wk_gpio.h"
#include "wk_system.h"
#include "at32f425.h"

#define BOOT_FLAG_ADDRESS   (0x08000000 + (64 * 1024) - 2048)
#define FW1_START_ADDRESS    0x08003000
#define FW2_START_ADDRESS    0x08005000

void LED_Init(void);
uint32_t read_boot_flag(void);
void write_boot_flag(uint32_t value);

void main(void)
{
    wk_system_clock_config();
    wk_periph_clock_config();
    wk_nvic_config();
    wk_timebase_init();
    wk_gpio_config();
    LED_Init();

    gpio_bits_write(GPIOC, GPIO_PINS_2, 0);
    
    wk_delay_ms(1000);  
    
    if(read_boot_flag() == 0)
    {
      wk_delay_ms(100);
      gpio_bits_write(GPIOC, GPIO_PINS_2, 1); 
      
      uint32_t *fw2_vector = (uint32_t*)FW1_START_ADDRESS;
      
      if(fw2_vector[0] == 0xFFFFFFFF || fw2_vector[1] == 0xFFFFFFFF) {
        return; 
      }
        
      __disable_irq();
      SysTick->CTRL = 0;
      NVIC_DisableIRQ(SysTick_IRQn);

      __set_MSP(*(uint32_t*)FW1_START_ADDRESS);
      SCB->VTOR = FW1_START_ADDRESS;

      __DSB();
      __ISB();

      uint32_t jump_address = *(uint32_t*)(FW1_START_ADDRESS + 4);
      void (*jump_to_app)(void) = (void (*)(void))jump_address;
      jump_to_app();
    }
        
    if(read_boot_flag() == 1)
    {
      wk_delay_ms(100);
      gpio_bits_write(GPIOC, GPIO_PINS_2, 1); 
      
      uint32_t *fw2_vector = (uint32_t*)FW2_START_ADDRESS;
      
      if(fw2_vector[0] == 0xFFFFFFFF || fw2_vector[1] == 0xFFFFFFFF) {
        return; 
      }
        
      __disable_irq();
      SysTick->CTRL = 0;
      NVIC_DisableIRQ(SysTick_IRQn);

      __set_MSP(*(uint32_t*)FW2_START_ADDRESS);
      SCB->VTOR = FW2_START_ADDRESS;

      __DSB();
      __ISB();

      uint32_t jump_address = *(uint32_t*)(FW2_START_ADDRESS + 4);
      void (*jump_to_app)(void) = (void (*)(void))jump_address;
      jump_to_app();
    }
    
    if(flash_fap_high_level_status_get() == RESET)
    {
      flash_unlock();
      
      write_boot_flag(0);
      
      flash_lock();
      NVIC_SystemReset();
    }
}

void LED_Init(void) {
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_2;
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

uint32_t read_boot_flag(void) {
    return *(uint32_t*)BOOT_FLAG_ADDRESS;
}
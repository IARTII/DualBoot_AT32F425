# DualBoot_AT32F425

1. В начале надо 2 проекта, пишим логику и т.п. <br>

В моем проекте: <br>
Первая прошивка с 0x08000000 <br>
Вторая прошивка с 0x08004000 <br>

Переменная-флаг с 0x800F800 (в моем случаи в последнюю страницу) <br>

# 2. Переменная-флаг

Делаем переменную по которой будем смотреть какая прошивка нужна, если вторая - пишем ее в MainFlash (либо в спец область если есть), писать надо постранично.
```C
   if(flash_fap_high_level_status_get() == RESET){
		flash_unlock();
		write_boot_flag(1);
		flash_lock();
		NVIC_SystemReset();
	}
	void write_boot_flag(uint32_t value) {
			flash_unlock();
    		flash_sector_erase(BOOT_FLAG_ADDRESS);
    		while(flash_flag_get(FLASH_ODF_FLAG) == RESET);
    		flash_flag_clear(FLASH_ODF_FLAG);
    		flash_word_program(BOOT_FLAG_ADDRESS, value);
    		flash_lock();
	}
```	

# 3. Считываем переменную, и:
```C
__disable_irq();												// 1. Отключаем все прерывания

SysTick->CTRL = 0;												// 2. Отключаем SysTick если использовали
NVIC_DisableIRQ(SysTick_IRQn);

__set_MSP(*(uint32_t*)FW2_START_ADDRESS);						// 3. Устанавливаем новый стек

SCB->VTOR = FW2_START_ADDRESS;									// 4. Устанавливаем новую таблицу векторов

__DSB();														// 5. Делаем барьеры
__ISB();

uint32_t jump_address = *(uint32_t*)(FW2_START_ADDRESS + 4);	// 6. Берем адрес Reset_Handler и переходим на него
void (*jump_to_app)(void) = (void (*)(void))jump_address;

jump_to_app();													// 7. Переход
```
# 4. Линкер
Идем менять линкер у второй прошивки (в IAR: Project->Option->Linker), линкер это файл с расширением .icf. В нем меняем:

```
define symbol __ICFEDIT_intvec_start__ = 0x08004000;          //<- Начало нашей 2 прошивки
/*-Memory Regions-*/
define symbol __ICFEDIT_region_ROM_start__ = 0x08004000;      //<- Начало нашей 2 прошивки
define symbol __ICFEDIT_region_ROM_end__   = 0x0800FFFF;      // Без изменений
define symbol __ICFEDIT_region_RAM_start__ = 0x20000000;      // Без изменений
define symbol __ICFEDIT_region_RAM_end__   = 0x20004FFF;      // Без изменений
```

Потом компилим в .bin файл
# 5. Прошиваем:

Я шил через ArteryICPProgrammer, там все легко: <br>
1)Подключаемся <br>
2)Добавляем файл первой прошивки и проставлям стартовый адрес (по дефолту и у меня 0x08000000) <br>
3)Добавляем файл второй прошивки и проставлям стартовый адрес (у меня 0x08004000) <br>
4)Загружаем на чип <br>

# Тестим :)

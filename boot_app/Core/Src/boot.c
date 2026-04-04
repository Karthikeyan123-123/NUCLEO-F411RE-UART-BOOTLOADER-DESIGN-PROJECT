 
 #include <string.h>
 #include <stdint.h>
 #include "main.h"
 #include "boot.h"
 #include "uart.h"

 
#define APP1_ADDRESS 0x08004000
#define APP2_ADDRESS 0x08008000
#define RAM_END_ADDR 0x20020000
#define RAM_START_ADDR 0x20000000
extern UART_HandleTypeDef huart2; 
 
 
 
 void deinit_and_jump(uint32_t app_base)
{
    typedef void (*app_entry_t)(void);

    // Read vector table entries
    uint32_t app_sp    = *(volatile uint32_t *)(app_base + 0U);
    uint32_t app_reset = *(volatile uint32_t *)(app_base + 4U);

    // Sanity check: SP must point to valid RAM
    if (app_sp < RAM_START_ADDR || app_sp > RAM_END_ADDR)
    {
        uart_write("Invalid app SP. Abort.\r\n");
        return;
    }

    // Stop SysTick and NVIC interrupts
    __disable_irq();
    SysTick->CTRL = 0;
    for (uint32_t i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }

    // Deinit HAL/peripherals
    HAL_UART_DeInit(&huart2);
    HAL_DeInit();

    // Relocate vector table to application
    SCB->VTOR = app_base;

    // Set MSP to application's stack pointer and jump to reset handler
    __set_MSP(app_sp);
    app_entry_t app_reset_handler = (app_entry_t)app_reset;
    app_reset_handler();
}


void Bootloader_Run(void)
{
    uart_write("\r\n=== NUCLEO--STM32F411RE Bootloader ===\r\n");
    uart_write("1) Run Application 1 @ 0x08004000\r\n");
    uart_write("2) Run Application 2 @ 0x08008000\r\n");
    uart_write("Select [1/2]: ");

    for (;;)
    {
        uint8_t sel = uart_read_char();
        //uint8_t sel = '1';
        uart_write("\r\n");
        if (sel == '1')
        {   uart_write("APP1_BOOTED");
            deinit_and_jump(APP1_ADDRESS);
        }
        else if (sel == '2')
        {   uart_write("APP2_BOOTED");
            deinit_and_jump(APP2_ADDRESS);
        }
        else
        {
            uart_write("Invalid selection.\r\n");
        }
        uart_write("Select [1/2]: ");
    }
}
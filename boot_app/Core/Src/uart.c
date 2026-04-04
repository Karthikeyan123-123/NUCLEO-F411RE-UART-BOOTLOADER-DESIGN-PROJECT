#include "main.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>




extern UART_HandleTypeDef huart2; 


void uart_write(const char *s)
{
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)s, (uint16_t)strlen(s), HAL_MAX_DELAY);
}

uint8_t uart_read_char(void)
{
    uint8_t ch = 0;
    (void)HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);
    return ch;
}

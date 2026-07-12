/**
 ******************************************************************************
 * @file      main.c
 * @brief     Minimal LED Blink Test + PA8 Chip Select Config
 * @hardware  STM32F103xx (with 16MHz External Crystal)
 ******************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"

// Function Prototypes
void SystemClock_Config(void);
void GPIO_Init(void);

int main(void){
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();          

    /* Infinite loop */
    while (1)
    {
        // Toggle LED (PC15)
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);
        HAL_Delay(500);
    }
}

// Configures the clock system using the physical 16MHz HSE crystal on your board
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON; 
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1);
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while(1);
    }
}

// Unified GPIO Initialization for Peripheral & Status Pins
void GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* --- 1. Enable Clocks for both Port A and Port C --- */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* --- 2. Configure PC15 for the D3 LED --- */
    // Set default state to High (LED Off initially because it's active-low)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* --- 3. Configure PA8 as Chip Select Pin --- */
    // Set default state to High (External SPI Device Deselected initially)
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
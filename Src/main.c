/**
 ******************************************************************************
 * @file      main.c
 * @brief     Cell Balancing Algorithm - STM32F103 + ADBMS6830B
 *
 * @hardware  STM32F103xx
 * @device    ADBMS6830B Battery Monitor
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"

void GPIO_Init(void);
int main(void){
    HAL_Init();
    GPIO_Init();

    while(1){
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);
        HAL_Delay(500);
    }
}

void GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
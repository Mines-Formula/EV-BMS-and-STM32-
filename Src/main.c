/**
 ******************************************************************************
 * @file      main.c
 * @brief     Cell Balancing Algorithm - STM32F103 + ADBMS6830B (isoSPI)
 * @hardware  STM32F103xx (with 16MHz External Crystal)
 ******************************************************************************
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_spi.h"   

// Global peripheral handles
SPI_HandleTypeDef hspi2;

// Function Prototypes
void SystemClock_Config(void);
void GPIO_Init(void);
void MX_SPI2_Init(void); 

int main(void){
    HAL_Init();
    SystemClock_Config(); 
    GPIO_Init();
    MX_SPI2_Init(); 

    /* Infinite loop */
    while (1)
    {
        uint8_t spi_tx_data[4] = {0xAA, 0xBB, 0xCC, 0xDD};
        uint8_t spi_rx_data[4] = {0};

        // Turn LED ON at start of packet transmission cycle (Active-LOW: RESET = ON)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

        // 1. Pull Chip Select (CS) LOW to alert the isoSPI transceiver/ADBMS6830B
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

        // 2. Transmit and Receive SPI data over the bus
        HAL_SPI_TransmitReceive(&hspi2, spi_tx_data, spi_rx_data, 4, 100);

        // 3. Pull Chip Select (CS) HIGH to end the frame transaction
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
        
        // Turn LED OFF after transmission complete (Active-LOW: SET = OFF)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

        // Leave the light off long enough for your eyes to visually register the blink!
        HAL_Delay(250);
    }
}

// Configures the clock system using the physical 16MHz HSE crystal on your board
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Initialize the External High Speed (HSE) oscillator circuit matching your 16MHz crystal
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON; // Turns on physical Y1 path
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        // Fault protection hook
        while(1);
    }

    // Direct system tracks to look at your HSE crystal frequency
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    // Setting appropriate flash latency so the CPU doesn't outrun reading memory instructions
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while(1);
    }
}

// Configures the SPI logical peripheral settings
void MX_SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT; 
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;

    HAL_SPI_Init(&hspi2);
}

// Hardware-level initialization called automatically inside HAL_SPI_Init
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hspi->Instance == SPI2)
    {
        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /** SPI2 GPIO Configuration
        PB13    ------> SPI2_SCK
        PB14    ------> SPI2_MISO
        PB15    ------> SPI2_MOSI
        */
        
        // Configure SCK and MOSI as Alternate Function Push-Pull
        GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        // Configure MISO as Input Floating
        GPIO_InitStruct.Pin = GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

// Configures General Outputs (LED and Chip Select)
void GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 1. Configure PC13 for the Debug/Status LED
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // 2. Configure PB12 as standard Output for isoSPI Chip Select (CS)
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Set defaults: CS High (idle), LED High (off)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
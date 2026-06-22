/***************************************************************************//**
 *   @file   adbms_common.h
 *   @brief  Header file of ADBMS Common Driver.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *   @author Jeric Oliver Vargas (jeric.vargas@analog.com)
********************************************************************************
 * MODIFICATION HISTORY:
 * Ver   Who Date     Changes
 * ----- --- -------- -----------------------------------------------------------
 * 1.0   Jeric Oliver Vargas 05/15/24 Initial release
 *******************************************************************************
 *
 * Copyright(c) 2024 Analog Devices, Inc. All Rights Reserved. This software is
 * proprietary & confidential to Analog Devices, Inc. and its licensors. By using
 * this software you agree to the terms of the associated Analog Devices License
 * Agreement.
 *
 ******************************************************************************/
#ifndef __ADBMS_COMMON_H__
#define __ADBMS_COMMON_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "adbms_utilities.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_delay.h"

/******************************************************************************/
/************************** ADBMS Common Definitions **************************/
/******************************************************************************/
#define ADBMS_WR_CFG_A					0x0001
#define ADBMS_WR_CFG_B					0x0024
#define ADBMS_RD_CFG_A					0x0002
#define ADBMS_RD_CFG_B					0x0026
#define ADBMS_RD_ID                     0x002C
#define ADBMS_SNAPSHOT					0x002D
#define ADBMS_RELEASE_SNAPSHOT			0x002F
#define ADBMS_RESET_CMD_CNT				0x002E
#define ADBMS_SOFT_RESET				0x0027

#define ADBMS_PEC_LEN					2
#define ADBMS_CMD_PEC_LEN				4
#define ADBMS_DATA_PEC_LEN				8

/* maximum number of devices in the chain per iso-spi channel */
#define ADBMS_MAX_NUM_DEV 				20

/* Time for isoSPI Wakeup */
#define ADBMS_TRDY						10

/* Time for ADBMS Wakeup (200uS <= t <= 500uS) */
#define ADBMS_TWAKE						300

/* Max SPI CLK */
#define ADBMS_MAX_SPI_CLK               2000000

/* Common Constants */
#define ADBMS_16_BIT_COMP_THRES			0x8000
#define ADBMS_16_BIT_MAX                0xFFFF
#define ADBMS_24_BIT_COMP_THRES			0x800000
#define ADBMS_24_BIT_MAX                0xFFFFFF
#define ADBMS_SDO_POLL_DONE             0xFF


struct adbms_dev {
    /* SPI */
    struct no_os_spi_desc *main_spi_desc;
    /* SPI */
    struct no_os_spi_desc *aux_spi_desc;    
    /* GPIO	 */
    struct no_os_gpio_desc *main_gpio_desc;
    /* GPIO	 */
    struct no_os_gpio_desc *aux_gpio_desc;    
    /* Number of devices */
    uint8_t num_devs;
    /* Describes devices in the chain */
    uint8_t *chain_devs;
    /* Write Data Buffer */
    uint8_t *write_buf;
    /* Read Data Buffer */
    uint8_t *read_buf;
};

struct adbms_init_param {
    /* SPI */
    struct no_os_spi_init_param *main_spi_init;
    /* SPI */
    struct no_os_spi_init_param *aux_spi_init;
    /* GPIO */
    struct no_os_gpio_init_param *main_gpio_init;
    /* GPIO */
    struct no_os_gpio_init_param *aux_gpio_init;
    /* Number devices */
    uint8_t *num_dev;
    /* Describes devices in the chain */
    uint8_t *chain_devs;
};

/* Currently Supported */
enum adbms_device_id {
    DEV_ADES1830 = (uint16_t)0x03,
    DEV_ADBMS6830 = (uint16_t)0x03,
    DEV_ADBMS2950 = (uint16_t)0x06,
};

/* ISO SPI Channels */
enum adbms_isospi_ch {
    MAIN_ISO_SPI_CHAN = (bool)true,
	AUX_ISO_SPI_CHAN = (bool)false,
};

extern const uint8_t adbms_dummy_packet[];
extern volatile bool IS_ADC_REFUP;
/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initializes the communication peripheral and checks if the ADBMS part is present. */
int adbms_init(struct adbms_dev **device, struct adbms_init_param *init_param);

/* Free the resources allocated by adbms_init(). */
int adbms_remove(struct adbms_dev *dev);

/* Performs a read command */
int adbms_read_command(struct adbms_dev *device, uint16_t bms_cmd, uint8_t *read_data,
                       uint8_t num_dev, bool is_main);

/* Performs a write command */
int adbms_write_command(struct adbms_dev *device, uint16_t bms_cmd, uint8_t *write_data,
                        uint8_t num_dev, bool is_main);

/* Performs general polling routine */
int adbms_poll(struct adbms_dev *device, uint8_t *buf, bool is_main);

/* Wake up the ADBMS device */
int adbms_wakeup(struct adbms_dev *device, uint8_t num, bool is_main);

/* ADBMS Command / Data Stream Builder */
int adbms_command_build(struct adbms_dev *device, uint8_t ic_pos,
                        uint8_t *dat, uint8_t *buf, uint8_t bytes);

/* Generates the stream for a data for write (w/ PEC) */
int adbms_data_prep(uint8_t *data, uint8_t *data_prep);

/* Generates the stream for a command (w/ PEC) */
int adbms_command_prep(uint16_t command, uint8_t *command_prep);

#endif	/* __ADBMS_COMMON_H__ */

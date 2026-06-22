/***************************************************************************//**
 *   @file   adbms6830.c
 *   @brief  Implementation of adbms6830 driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adbms6830.h"
#include "adbms_common.h"
#include "adbms_utilities.h"

#include <stdint.h>
#include <stdlib.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Generates data for writing to cfg a register
 *
 * @param [in] cfg_a_data - Structure containing CFG A parameters / configuration
 * @param [out] wr_buf - Array containing data stream to write to
 * 			CFG A register group
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Translates readable software settings for Configuration Register Group A into raw binary bytes.
How it works: It acts as a safety validator and bit-packer. 
It checks if your threshold values fall within valid boundaries, 
maps parameters like internal reference settings (ref_on), comparator thresholds (cth), 
and General Purpose Input/Output settings (gpo) into a transient 12-byte hardware buffer (for_write), 
calculates a security checksum (adbms_data_prep), and loads it into your SPI write buffer.
*/

int adbms6830_cfg_a_gen(struct adbms6830_cfg_a_data cfg_a_data, uint8_t *wr_buf)
{
    int i, ret;
    uint8_t for_write[12] = {0};

    if (cfg_a_data.cth > ADBMS6830_40_P_05_MV || cfg_a_data.cth < ADBMS6830_5_P_1_MV)
        return -EINVAL;

    if (cfg_a_data.owa > 0x07 || cfg_a_data.fc > 0x07 || cfg_a_data.gpo > 0x3ff)
        return -EINVAL;

    for_write[4] = no_os_field_prep(ADBMS6830_REFON_MASK, (uint8_t)cfg_a_data.ref_on)
                   | no_os_field_prep(ADBMS6830_CTH_MASK, cfg_a_data.cth);
    for_write[5] = cfg_a_data.flag;
    for_write[6] = no_os_field_prep(ADBMS6830_SOAKON_MASK, (uint8_t)cfg_a_data.soak_on)
                   | no_os_field_prep(ADBMS6830_OWRNG_MASK, (uint8_t)cfg_a_data.owrng)
                   | no_os_field_prep(ADBMS6830_OWA_MASK, cfg_a_data.owa);
    for_write[7] = (uint8_t)cfg_a_data.gpo;
    for_write[8] = no_os_field_get(ADBMS6830_GPO_MASK, cfg_a_data.gpo);
    for_write[9] = no_os_field_prep(ADBMS6830_SNAP_ST_MASK, (uint8_t)cfg_a_data.snap_st)
                   | no_os_field_prep(ADBMS6830_MUTE_ST_MASK, (uint8_t)cfg_a_data.mute_st)
                   | no_os_field_prep(ADBMS6830_COMM_BK_MASK, (uint8_t)cfg_a_data.comm_bk)
                   | no_os_field_prep(ADBMS6830_FC_MASK, cfg_a_data.fc);

    ret = adbms_data_prep(for_write + 4, for_write + 4);
    if (ret)
        return ret;

    for (i = 0; i < 8; i++)
        wr_buf[i] = for_write[i + 4];

    return 0;
}

/***************************************************************************//**
 * @brief Parses register data to CFG A structure
 *
 * @param [out] cfg_a_data - Structure to stores CFG A parsed data
 * @param [in] reg_data - Array containing the register data read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Performs the exact inverse of the generator function.
How it works: When you read back Configuration Register Group A 
from the chip, it arrives as a stream of raw bytes (reg_data). 
This function extracts specific bit-fields using masks and bit-shifts, 
assigning them back into a clean, human-readable structure (cfg_a_data).
*/

int adbms6830_cfg_a_parse(struct adbms6830_cfg_a_data *cfg_a_data, uint8_t *reg_data)
{
    cfg_a_data->ref_on = ((reg_data[4] & 0x80) >> 7);
    cfg_a_data->cth =(reg_data[4]) & 0x07;
    cfg_a_data->flag = reg_data[5] & 0xFF;
    cfg_a_data->soak_on = ( (reg_data[6]) & 0x80) >> 7;
    cfg_a_data->owrng = ( (reg_data[6])  & 0x40) >> 6;
    cfg_a_data->owa = ((reg_data[6]) & 0x38) >> 3;
    cfg_a_data->gpo = (reg_data[7] & 0xFF) | ((reg_data[8] & 0x03) << 8);
    cfg_a_data->snap_st = ((reg_data[9]) & 0x20) >> 5;
    cfg_a_data->mute_st = ((reg_data[9]) & 0x10) >> 4;
    cfg_a_data->comm_bk = ((reg_data[9]) & 0x08) >> 3;
    cfg_a_data->fc = ((reg_data[9]) & 0x07) >> 0;

    return 0;
}

/***************************************************************************//**
 * @brief Generates data for writing to cfg b register
 *
 * @param [in] cfg_b_data - Structure containing the data to write to CFG B register
 * @param [out] wr_buf - Array containing data stream to write to CFG B register group
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Translates configuration parameters for 
Configuration Register Group B into raw binary bytes.
How it works: This handles cell protection boundaries. 
It validates and packs your Under-Voltage limit (vuv), Over-Voltage limit (vov), 
discharge timeout options (dcto), and passive cell-balancing switches (dcc) into the SPI output buffer.
*/

int adbms6830_cfg_b_gen(struct adbms6830_cfg_b_data cfg_b_data, uint8_t *wr_buf)
{
    int i, ret;
    uint8_t for_write[12] = {0};

    if (cfg_b_data.vuv > 0xFFF || cfg_b_data.vov > 0xFFF)
        return -EINVAL;

    if (cfg_b_data.dcto > 0x3F)
        return -EINVAL;

    for_write[4] = (uint8_t) cfg_b_data.vuv;
    for_write[5] = no_os_field_get(ADBMS6830_VUV_XTR_MASK, cfg_b_data.vuv)
                   | ((cfg_b_data.vov & 0xf) << 4);
    for_write[6] = no_os_field_get(ADBMS6830_VOV_XTR2_MASK, cfg_b_data.vov);
    for_write[7] = no_os_field_prep(ADBMS6830_DTMEN_MASK, cfg_b_data.dtmen)
                   | no_os_field_prep(ADBMS6830_DTRNG_MASK, cfg_b_data.dtrng)
                   | no_os_field_prep(ADBMS6830_DCTO_MASK, cfg_b_data.dcto);

    for_write[8] = no_os_field_get(ADBMS6830_LOW_BYTE, cfg_b_data.dcc);
    for_write[9] = no_os_field_get(ADBMS6830_HI_BYTE, cfg_b_data.dcc);

    ret = adbms_data_prep(for_write + 4, for_write + 4);
    if (ret)
        return ret;

    for (i = 0; i < 8; i++)
        wr_buf[i] = for_write[i + 4];

    return 0;
}

/***************************************************************************//**
 * @brief Parses register data to CFG B parameters
 *
 * @param [out] cfg_b_data - Structure to stores CFG B parsed data
 * @param [in] reg_data - Array containing the register data read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Extracts readable cell protection boundaries from raw read data.
How it works: Decodes raw binary stream arrays back into specific variables, 
revealing what the current under-voltage, over-voltage, and passive 
cell-balancing configurations are on the chip.
*/

int adbms6830_cfg_b_parse(struct adbms6830_cfg_b_data *cfg_b_data, uint8_t *reg_data)
{
    cfg_b_data->vuv = ((reg_data[4])  | ((reg_data[5] & 0x0F) << 8));
    cfg_b_data->vov = (reg_data[6]<<4)+((reg_data[5] & 0xF0) >> 4);
    cfg_b_data->dtmen = (bool)(((reg_data[7] & 0x80) >> 7));
    cfg_b_data->dtrng = (bool)((reg_data[7] & 0x40) >> 6);
    cfg_b_data->dcto = ((reg_data[7] & 0x3F));
    cfg_b_data->dcc = ((reg_data[8]) | ((reg_data[9] & 0xFF) << 8));

    return 0;
}

/***************************************************************************//**
 * @brief Polls selected ADBMS6830 ADC
 *
 * @param [in] device - The device structure.
 * @param [in] poll - option for selecting which ADC/s to poll
 * @param [in] is_main - Set true to use MAIN ISO-SPI Channel. False for AUX ISO-SPI Channel.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Asks the chip, "Are you done measuring yet?"
How it works: Depending on what you are tracking (Cell voltages, Auxiliary channels, or Status bits), 
it picks the corresponding hardware polling opcode, loads it into the write buffer, 
and initiates a low-level check (adbms_poll) to see if the chip's internal conversion is complete.
*/

int adbms6830_poll_adc(struct adbms_dev *device, enum adbms6830_poll_adc poll,
        bool is_main)
{
    int ret;
    
    switch (poll) {
    case ADBMS6830_POLL_CELL:
        ret = adbms_command_prep(ADBMS6830_POLL_CADC, device->write_buf);
        break;
    case ADBMS6830_POLL_S:
        ret = adbms_command_prep(ADBMS6830_POLL_SADC, device->write_buf);
        break;
    case ADBMS6830_POLL_AUX:
        ret = adbms_command_prep(ADBMS6830_POLL_AUX_ADC, device->write_buf);
        break;
    case ADBMS6830_POLL_AUX_2:
        ret = adbms_command_prep(ADBMS6830_POLL_AUX_ADC_2, device->write_buf);
        break;
    case ADBMS6830_POLL_ANY:
        ret = adbms_command_prep(ADBMS6830_POLL_ADC_STAT, device->write_buf);
        break;
    default:
        return -EINVAL;                
    }

    if (ret)
        return ret;

    return adbms_poll(device, device->write_buf, is_main);    
}



/**
*******************************************************************************
* Function: adbms6830_adcv
* @brief ADCV Command.
*
* @details Send adcv command to start cell voltage conversion.
*
* Parameters:
* @param [in] rd       - Redundant bit
* @param [in] cont     - Continuous measurement bit
* @param [in] dcp      - Discharge bit
* @param [in] rstf     - Reset filter
* @param [in] owcs     - Open wire c/s
* @param [in] is_main  - Set true to use MAIN ISO-SPI Channel. False for AUX ISO-SPI Channel.
*
* @return 0 in case of success, negative error code otherwise.
*
*******************************************************************************
*/

/*
What it does: Commands the chip to physically measure your battery cell voltages.
How it works: It pieces together the unique multi-bit ADCV opcode using 
operational configuration inputs like discharge control (dcp) and continuous measuring (cont). 
It calculates a mandatory Packet Error Code (adbms_command_pec), 
drives the Chip Select line low to signal the converter board, 
sends the 4-byte command over SPI, and drives Chip Select high again.
*/
int adbms6830_adcv
(
struct adbms_dev *device,
uint8_t rd,
uint8_t cont,
uint8_t dcp,
uint8_t rstf,
uint8_t owcs,
bool is_main
)
{
  int ret = 0;
  uint8_t cmd[4];
  uint16_t cmd_pec;
  struct no_os_spi_desc *spi_path;
  struct no_os_gpio_desc *cs_used;

  cmd[0] = 0x02 | rd;
  cmd[1] = (cont<<7) | (dcp<<4) | (rstf<<2) | (owcs & 0x03) | 0x60;
  uint16_t cmd_temp = ((cmd[0]) << 8) | cmd[1];
  ret = adbms_command_pec(cmd_temp, &cmd_pec);
  if (ret)
	  return ret;
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  /* Use setting depending on path selected */
  if (is_main) {
	  spi_path = device->main_spi_desc;
	  cs_used = device->main_gpio_desc;
  } else {
	  spi_path = device->aux_spi_desc;
	  cs_used = device->aux_gpio_desc;
  }

  /* set cs low */
  ret = no_os_gpio_set_value(cs_used, NO_OS_GPIO_LOW);
  if (ret)
	  return ret;

  /* perform spi transaction */
  ret = no_os_spi_write_and_read(spi_path, cmd, 4);
  if (ret)
	  return ret;

  /* set cs high */
  return no_os_gpio_set_value(cs_used, NO_OS_GPIO_HIGH);

}

/**
*******************************************************************************
* Function: adbms6830_adsv
* @brief ADSV Command.
*
* @details Send adsv command to start cell voltage conversion.
*
* Parameters:
* @param [in] cont     - Continuous measurement bit
* @param [in] dcp      - Discharge bit
* @param [in] owcs     - Open wire c/s
* @param [in] is_main  - Set true to use MAIN ISO-SPI Channel. False for AUX ISO-SPI Channel.
*
* @return 0 in case of success, negative error code otherwise.
*
*******************************************************************************
*/

/*
What it does: Commands the chip to measure secondary 
diagnostic pins (S-pins) for diagnostic safety redundancy.
How it works: This functions identically to adbms6830_adcv, 
but targets the chip's redundant internal multiplexer circuitry 
to double-check that the primary readings remain accurate and haven't drifted.
*/

int adbms6830_adsv
(
struct adbms_dev *device,
uint8_t cont,
uint8_t dcp,
uint8_t owcs,
bool is_main
)
{
	int ret = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;
	struct no_os_spi_desc *spi_path;
	struct no_os_gpio_desc *cs_used;

	cmd[0] = 0x01;
	cmd[1] = (cont<<7) | (dcp<<4) | (owcs &0x03) | 0x68;
	uint16_t cmd_temp = ((cmd[0]) << 8) | cmd[1];
	ret = adbms_command_pec(cmd_temp, &cmd_pec);
	if (ret)
		return ret;
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	/* Use setting depending on path selected */
	if (is_main) {
		spi_path = device->main_spi_desc;
		cs_used = device->main_gpio_desc;
	} else {
		spi_path = device->aux_spi_desc;
		cs_used = device->aux_gpio_desc;
	}


	/* set cs low */
	ret = no_os_gpio_set_value(cs_used, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* perform spi transaction */
	ret = no_os_spi_write_and_read(spi_path, cmd, 4);
	if (ret)
		return ret;

	/* set cs high */
	return no_os_gpio_set_value(cs_used, NO_OS_GPIO_HIGH);
}

/**
*******************************************************************************
* Function: adbms6830_adax
* @brief ADAX Command.
*
* @details Send aux command to starts auxiliary conversion.
*
* Parameters:
* @param [in]  owcs    - Open wire c/s
* @param [in]  pup     - Pull Down current during aux conversion
* @param [in]  ch      - GPIO Channel selection
* @param [in] is_main  - Set true to use MAIN ISO-SPI Channel. False for AUX ISO-SPI Channel.
*
* @return 0 in case of success, negative error code otherwise.
*
*******************************************************************************
*/

/*
What it does: Commands the chip to measure its auxiliary inputs 
(typically connected to temperature NTC thermistors).
How it works: Packs a hardware instruction specifying which auxiliary 
GPIO channel (ch) to measure and whether to engage pull-down testing currents (pup), 
then sends that 4-byte frame across the chosen SPI path.
*/
int adbms6830_adax
(
struct adbms_dev *device,
uint8_t owaux,
uint8_t pup,
uint8_t ch,
bool is_main
)
{
	int ret = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;
	struct no_os_spi_desc *spi_path;
	struct no_os_gpio_desc *cs_used;


	cmd[0] = 0x04 | owaux;
	cmd[1] = ( ((pup << 7) | (((ch >>4)&0x01)<<6)) | (ch & 0x0F) ) | 0x10;
	uint16_t cmd_temp = ((cmd[0]) << 8) | cmd[1];
	ret = adbms_command_pec(cmd_temp, &cmd_pec);
	if (ret)
		return ret;
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	/* Use setting depending on path selected */
	if (is_main) {
		spi_path = device->main_spi_desc;
		cs_used = device->main_gpio_desc;
	} else {
		spi_path = device->aux_spi_desc;
		cs_used = device->aux_gpio_desc;
	}


	/* set cs low */
	ret = no_os_gpio_set_value(cs_used, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	/* perform spi transaction */
	ret = no_os_spi_write_and_read(spi_path, cmd, 4);
	if (ret)
		return ret;

	/* set cs high */
	return no_os_gpio_set_value(cs_used, NO_OS_GPIO_HIGH);

}

/***************************************************************************//**
 * @brief Converts the raw data into corresponding voltage measurement
 *
 * @param [in] cell_data - Stores raw 16 bit cell voltage readings.
 * @param [in] len - Length of data to convert from raw data.
 * @param [out] converted - Stores the converted value/s
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/

/*
What it does: Converts raw, cryptic 16-bit integers into real, 
readable floating-point voltage numbers (e.g., converting 0x3A98 to 3.65V).
How it works: It checks if a value is negative by comparing it against 
a two's complement threshold limit (ADBMS_16_BIT_COMP_THRES). 
It then loops through the array data, applying fixed conversion scaling math (ADBMS6830_VCONV_SCALE) 
and any necessary calibration offsets to populate your float *converted array 
with practical voltage measurements.
*/

int adbms6830_cells_convert(uint16_t *cell_data, uint8_t len, float *converted)
{
    int i, temp;

    if (!cell_data || !converted)
        return -EINVAL;

    if (len == 1) {
        temp = *cell_data;

        if (temp >= ADBMS_16_BIT_COMP_THRES)
            temp = temp - ADBMS_16_BIT_MAX + 1;

        *converted = (float)temp * ADBMS6830_VCONV_SCALE + ADBMS6830_VCONV_OFFSET;
    }
    else {
        for (i = 0; i < len; i++) {
            temp = cell_data[i];
            if (temp >= ADBMS_16_BIT_COMP_THRES)
                temp = temp - ADBMS_16_BIT_MAX + 1;

            *converted = (float)temp * ADBMS6830_VCONV_SCALE + ADBMS6830_VCONV_OFFSET;
            converted++;
        }
    }

    return 0;
}

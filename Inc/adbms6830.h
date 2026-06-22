/***************************************************************************//**
 *   @file   adbms6830.h
 *   @brief  Header file of ADBMS6830 Driver.
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
#ifndef __ADBMS6830_H__
#define __ADBMS6830_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>

#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_delay.h"

#include "adbms_common.h"
#include "adbms_utilities.h"
/******************************************************************************/
/************************** ADBMS6830 Definitions *******************************/
/******************************************************************************/

/* Command Codes */

/* READ CELL */
#define ADBMS6830_RD_CV_A				0x0004
#define ADBMS6830_RD_CV_B				0x0006
#define ADBMS6830_RD_CV_C				0x0008
#define ADBMS6830_RD_CV_D				0x000A
#define ADBMS6830_RD_CV_E				0x0009
#define ADBMS6830_RD_CV_F				0x000B
#define ADBMS6830_RD_CV_ALL				0x000C

/* READ AVERAGED CELL */
#define ADBMS6830_RD_ACV_A				0x0044
#define ADBMS6830_RD_ACV_B				0x0046
#define ADBMS6830_RD_ACV_C				0x0048
#define ADBMS6830_RD_ACV_D				0x004A
#define ADBMS6830_RD_ACV_E				0x0049
#define ADBMS6830_RD_ACV_F				0x004B
#define ADBMS6830_RD_ACV_ALL				0x004C

/* READ S VOLTAGE */
#define ADBMS6830_RD_SV_A				0x0003
#define ADBMS6830_RD_SV_B				0x0005
#define ADBMS6830_RD_SV_C				0x0007
#define ADBMS6830_RD_SV_D				0x000D
#define ADBMS6830_RD_SV_E				0x000E
#define ADBMS6830_RD_SV_F				0x000F
#define ADBMS6830_RD_SV_ALL				0x0010

/* READ ALL CELLS */
#define ADBMS6830_RD_CS_ALL				0x0011
#define ADBMS6830_RD_ACS_ALL				0x0051

/* READ FILTERED CELL */
#define ADBMS6830_RD_FCV_A				0x0012
#define ADBMS6830_RD_FCV_B				0x0013
#define ADBMS6830_RD_FCV_C				0x0014
#define ADBMS6830_RD_FCV_D				0x0015
#define ADBMS6830_RD_FCV_E				0x0016
#define ADBMS6830_RD_FCV_F				0x0017
#define ADBMS6830_RD_FCV_ALL				0x0018

/* READ AUX */
#define ADBMS6830_RD_AUX_A				0x0019
#define ADBMS6830_RD_AUX_B				0x001A
#define ADBMS6830_RD_AUX_C				0x001B
#define ADBMS6830_RD_AUX_D				0x001F

/* READ REDUNDANT AUX */
#define ADBMS6830_RD_RAUX_A				0x001C
#define ADBMS6830_RD_RAUX_B				0x001D
#define ADBMS6830_RD_RAUX_C				0x001E
#define ADBMS6830_RD_RAUX_D				0x0025

/* STATUS REG */
#define ADBMS6830_RD_STAT(x)			0x0030 + (x)

/* PWM READ/WRITE */
#define ADBMS6830_WR_PWM_A				0x0020
#define ADBMS6830_RD_PWM_A				0x0022
#define ADBMS6830_WR_PWM_B				0x0021
#define ADBMS6830_RD_PWM_B				0x0023

/* START ADC COMMANDS */
#define ADBMS6830_START_ADC_CONV		0x0260
#define ADBMS6830_START_SADC_CONV		0x0168
#define ADBMS6830_START_AUX_ADC_CONV		0x0410
#define ADBMS6830_START_AUX2_ADC_CONV		0x0400

/* CLEAR ADCS */
#define ADBMS6830_CLR_CELL_GRPS			0x0711
#define ADBMS6830_CLR_FILT_CELL_GRPS		0x0714
#define ADBMS6830_CLR_AUX_GRP			0x0712
#define ADBMS6830_CLR_SVOLTAGE_GRPS		0x0716

/* CLEAR FLAGS */
#define ADBMS6830_CLR_FLAGS			0x0717
#define ADBMS6830_CLR_OVUV			0x0715

/* POLLING */
#define ADBMS6830_POLL_ADC_STAT			0x0718
#define ADBMS6830_POLL_CADC			0x071C
#define ADBMS6830_POLL_SADC			0x071D
#define ADBMS6830_POLL_AUX_ADC			0x071E
#define ADBMS6830_POLL_AUX_ADC_2		0x071F

/* ADBMS AS MASTER (SPI/I2C) */
#define ADBMS6830_WR_COMM_REG_GRP		0x0721
#define ADBMS6830_RD_COMM_REG_GRP		0x0722
#define ADBMS6830_START_COMM			0x0723

/* DISCHARGE */
#define ADBMS6830_MUTE_DISCHARGE		0x0028
#define ADBMS6830_UNMUTE_DISCHARGE		0x0029

/* RETENTION REGISTERS */
#define ADBMS6830_UNLOCK_RET_REG		0x0038
#define ADBMS6830_WRITE_RET_REG			0x0039
#define ADBMS6830_READ_RET_REG			0x003A

/* Others */
#define ADBMS6830_LOW_BYTE			NO_OS_GENMASK(7, 0)
#define ADBMS6830_HI_BYTE			NO_OS_GENMASK(15, 8)

#define ADBMS6830_LOW_NIBBLE			NO_OS_GENMASK(3, 0)
#define ADBMS6830_HI_NIBBLE			NO_OS_GENMASK(7, 4)

#define ADBMS6830_ICOM_X_MASK			NO_OS_GENMASK(7, 4)
#define ADBMS6830_FCOM_X_MASK			NO_OS_GENMASK(3, 0)

#define ADBMS6830_PWM_RES_PERCENT		6.6

/* ADCV */
#define ADBMS6830_RD_ADCV_MASK			NO_OS_BIT(8)
#define ADBMS6830_OW_AUX_MASK			NO_OS_BIT(8)
#define ADBMS6830_CONT_MEAS_MASK		NO_OS_BIT(7)
#define ADBMS6830_PULL_UP_DOWN			NO_OS_BIT(7)
#define ADBMS6830_DIS_PERMIT_MASK		NO_OS_BIT(4)
#define ADBMS6830_RST_FILT_MASK			NO_OS_BIT(2)

/* ADSV */
#define ADBMS6830_OW_C_S_ADC			NO_OS_GENMASK(1, 0)
#define ADBMS6830_AUX_IN_MASK			NO_OS_GENMASK(3, 0)
#define ADBMS6830_AUX_IN_CH4			NO_OS_BIT(6)
#define ADBMS6830_AUX_IN_CH4_MASK		ADBMS6830_AUX_IN_CH4 | ADBMS6830_AUX_IN_MASK

/* CFG A */
#define ADBMS6830_REFON_MASK			NO_OS_BIT(7)
#define ADBMS6830_CTH_MASK			NO_OS_GENMASK(2, 0)
#define ADBMS6830_SOAKON_MASK			NO_OS_BIT(7)
#define ADBMS6830_OWRNG_MASK			NO_OS_BIT(6)
#define ADBMS6830_OWA_MASK			NO_OS_GENMASK(5, 3)
#define ADBMS6830_GPO_MASK			NO_OS_GENMASK(9, 8)
#define ADBMS6830_SNAP_ST_MASK			NO_OS_BIT(6)
#define ADBMS6830_MUTE_ST_MASK			NO_OS_BIT(5)
#define ADBMS6830_COMM_BK_MASK			NO_OS_BIT(4)
#define ADBMS6830_FC_MASK			NO_OS_GENMASK(2, 0)

/* CFG B */
#define ADBMS6830_VUV_XTR_MASK			NO_OS_GENMASK(11, 8)
#define ADBMS6830_VOV_XTR1_MASK			NO_OS_GENMASK(7, 4)
#define ADBMS6830_VOV_XTR2_MASK			NO_OS_GENMASK(11, 4)
#define ADBMS6830_DTMEN_MASK			NO_OS_BIT(7)
#define ADBMS6830_DTRNG_MASK			NO_OS_BIT(6)
#define ADBMS6830_DCTO_MASK			NO_OS_GENMASK(5, 0)

/* STAT C */
#define ADBMS6830_CT_XTR_MASK			NO_OS_GENMASK(4, 0)

/* CLEAR FLAGS */
#define ADBMS6830_CLR_VA_OV			NO_OS_BIT(7)
#define ADBMS6830_CLR_VA_UV			NO_OS_BIT(6)
#define ADBMS6830_CLR_VD_OV			NO_OS_BIT(5)
#define ADBMS6830_CLR_VD_UV			NO_OS_BIT(4)
#define ADBMS6830_CLR_CED			NO_OS_BIT(3)
#define ADBMS6830_CLR_CMED			NO_OS_BIT(2)
#define ADBMS6830_CLR_SED			NO_OS_BIT(1)
#define ADBMS6830_CLR_SMED			NO_OS_BIT(0)

#define ADBMS6830_CLR_VDEL			NO_OS_BIT(7)
#define ADBMS6830_CLR_VDE			NO_OS_BIT(6)
#define ADBMS6830_CLR_SPI_FILT			NO_OS_BIT(4)
#define ADBMS6830_CLR_SLEEP			NO_OS_BIT(3)
#define ADBMS6830_CLR_THSD			NO_OS_BIT(2)
#define ADBMS6830_CLR_TMODE			NO_OS_BIT(1)
#define ADBMS6830_CLR_OSC_CHK			NO_OS_BIT(0)

/* PWM CHANNEL MASKS */
#define ADBMS6830_PWM_ODD_CHANNEL		NO_OS_GENMASK(3, 0)
#define ADBMS6830_PWM_EVEN_CHANNEL		NO_OS_GENMASK(7, 4)

/* Constants etc. */
#define ADBMS6830_VCONV_SCALE			1.5e-4
#define ADBMS6830_VCONV_OFFSET			1.5024
#define ADBMS6830_TEMP_SCALE			0.0075
#define ADBMS6830_TEMP_OFFSET			273

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adbms6830_cell_meas_type {
    ADBMS6830_CELL,
    ADBMS6830_CELL_FILTERED,
    ADBMS6830_CELL_AVERAGED,
    ADBMS6830_CELL_S_VOLTS
};

enum adbms6830_adc_memory {
    ADBMS6830_CLEAR_CELLS,
    ADBMS6830_CLEAR_FILTERED_CELLS,
    ADBMS6830_CLEAR_AUXILLIARY,
    ADBMS6830_CLEAR_S_PINS,
};

enum adbms6830_poll_adc {
    ADBMS6830_POLL_ANY,
    ADBMS6830_POLL_CELL,
    ADBMS6830_POLL_S,
    ADBMS6830_POLL_AUX,
    ADBMS6830_POLL_AUX_2,
};

enum adbms6830_c_vs_s_threshold {
    ADBMS6830_5_P_1_MV,
    ADBMS6830_8_P_1_MV,
    ADBMS6830_9_P_0_MV,
    ADBMS6830_10_P_05_MV,
    ADBMS6830_15_P_0_MV,
    ADBMS6830_19_P_95_MV,
    ADBMS6830_25_P_05_MV,
    ADBMS6830_40_P_05_MV,
};

enum adbms6830_aux_in {
    ADBMS6830_ALL,
    ADBMS6830_GPIO1,
    ADBMS6830_GPIO2,
    ADBMS6830_GPIO3,
    ADBMS6830_GPIO4,
    ADBMS6830_GPIO5,
    ADBMS6830_GPIO6,
    ADBMS6830_GPIO7,
    ADBMS6830_GPIO8,
    ADBMS6830_GPIO9,
    ADBMS6830_GPIO10,
    ADBMS6830_VREF2 = 0x10,
    ADBMS6830_VD,
    ADBMS6830_VA,
    ADBMS6830_ITEMP,
    ADBMS6830_VPV,
    ADBMS6830_VMV,
    ADBMS6830_RES,
    ADBMS6830_RESERVED,
};

enum adbms6830_ow_conf {
    ADBMS6830_OFF_ALL_CH,
    ADBMS6830_OFF_ODD_CH,
    ADBMS6830_OFF_EVEN_CH,
    ADBMS6830_ON_ALL_CH,
};

enum adbms6830_init_comm_ctrl {
    ADBMS6830_I2C_BLANK,
    ADBMS6830_I2C_STOP,
    ADBMS6830_I2C_START = 0x6,
    ADBMS6830_I2C_NO_TRANSMIT,
    ADBMS6830_SPI_CSB_LOW,
    ADBMS6830_SPI_INIT_CSB_HIGH,
    ADBMS6830_SPI_CSB_FALLING_EDGE,
    ADBMS6830_SPI_NO_TRANSMIT = 0xF,
};

enum adbms6830_final_i2c_write {
    ADBMS6830_I2C_MASTER_ACK,
    ADBMS6830_I2C_MASTER_NACK = 0x8,
    ADBMS6830_I2C_MASTER_NACK_STOP,
};

enum adbms6830_final_i2c_read {
    ADBMS6830_I2C_ACK_FROM_MASTER,
    ADBMS6830_I2C_ACK_SLAVE_STOP_MASTER,
    ADBMS6830_I2C_ACK_SLAVE = 0x7,
    ADBMS6830_I2C_NACK_SLAVE_STOP_MASTER = 0x9,
    ADBMS6830_I2C_NACK_SLAVE = 0xF,
};

enum adbms6830_final_spi_comm {
    ADBMS6830_SPI_CSB_LOW_1,
    ADBMS6830_SPI_CSB_LOW_2 = 0x8,
    ADBMS6830_SPI_FINAL_CSB_HIGH,
};

/***************************************************************************//**
 * @brief CFG A data structure
 *
 * @param ref_on - Reference power up config (0: shut down after conversion
 * 			1: remain powered until watchdog timeout)
 * @param cth - Cell voltage comparison threshold (C-ADC vs. S-ADC)
 * 			(Refer to datasheet Table 102)
 * @param flag - Flag assetion (STATC) for latent fault detection
 * 			(Refer to datasheet Table 102)
 * @param soak_on - AUX ADC Soak conig (0: disable, 1: enable)
 * @param owrng - Soak time range (0: short soak time, 1: long soak time)
 * @param owa - Open wire soak times (Refer to datasheet Table 102)
 * @param gpo - GPOx control (for GPIO 1 to 10) -> 10 bit combination
 * 			of GPO levels (MSB -> LSB : GPO10 -> GPO1)
 * @param snap_st - Snapshot status (0: deactivated, 1: result registers
 * 			are frozen / activated)
 * @param mute_st - Mute Status (0: deactivated, 1: discharging is
 * 			disabled / activated)
 * @param comm_bk - Communication break (0: normal operation,
 * 			1: prevents further propagation through the chain)
 * @param fc - IIR filter parameter (Refer to datasheet Table 102)
*******************************************************************************/
struct adbms6830_cfg_a_data {
    bool ref_on;
    enum adbms6830_c_vs_s_threshold cth;
    uint8_t flag;
    bool soak_on;
    bool owrng;
    uint8_t owa;
    uint16_t gpo;
    bool snap_st;
    bool mute_st;
    bool comm_bk;
    uint8_t fc;
};

/***************************************************************************//**
 * @brief CFG B data structure
 *
 * @param vuv - Undervoltage Comparison Threshold
 * 			(Refer to datasheet Table 103)
 * @param vov - Overvoltage Comparison Threshold
 * 			(Refer to datasheet Table 103)
 * @param dtmen - Discharge Monitor Enable (0: disable, 1: enable if device
 * 			transition to extended balancing state)
 * @param dtrng - Discharge Timer Range:
 * 			0: 0 to 63 mins., 1 min. increments
 * 			1: 0 to 16.8hrs., 16 min. increments
 * @param dcto - Discharge Timeout Value
 *			0: timeout occurred / DCTO not set.
 * 			1: <= 1 increment remaining.
 * @param dcc - Discharge Cell -> 10 bit combination of GPO levels
 * 			(MSB -> LSB : GPO10 -> GPO1)
*******************************************************************************/
struct adbms6830_cfg_b_data {
    uint16_t vuv;
    uint16_t vov;
    bool dtmen;
    bool dtrng;
    uint8_t dcto;
    uint16_t dcc;
};

/***************************************************************************//**
 * @brief ADBMS6830 Aux ADC configuration structure
 *
 * @param ow - Open wire on AUX ADC (0: OFF, 1: ON)
 * @param pup - Pull up/down for open wire conversions
 * 			0: Pull down current during AUX conversion
 * 			1: Pull up current durign AUX conversion
 * @param ch - Selection of AUX ADC inputs (Refer to datasheet Table 24)
*******************************************************************************/
struct adbms6830_aux_adc_cfg {
    bool ow;
    bool pup;
    enum adbms6830_aux_in ch;
};

/***************************************************************************//**
 * @brief ADBMS6830 Cell ADC configuration structure
 *
 * @param rd - Redundant Measurement enable (0: disable, 1: enable)
 * @param cont - Measurement config (0: single, 1: continuous)
 * @param dcp - Discharge Permission
 * 			0: Discharge not permitted during S-ADC conversion
 * 			1: Discharge permitted during S-ADC conversion
 * 		Note: Refer to "Discharge During Measurements" section in
 * 			datasheet.
 * @param rstf - Reset Filter (0: Don't reset filter , 1: Reset filter)
 * @param ow - Open wire configuration (refer to table 54 on datasheet)
*******************************************************************************/
struct adbms6830_cell_adc_cfg {
    bool rd;
    bool cont;
    bool dcp;
    bool rstf;
    enum adbms6830_ow_conf ow;
};

/***************************************************************************//**
 * @brief ADBMS6830 Aux Data structure (GPIO, vmv, vpv)
 *
 * @param gpio - array to contain raw and parsed gpio data
 * @param vpv - Voltage measured from V+ with respect to V-
 * @param vmv - Voltage measured from V- with respect to V-
*******************************************************************************/
struct adbms6830_aux_data {
    uint16_t gpio[10];
    uint16_t vpv;
    uint16_t vmv;
};

/***************************************************************************//**
 * @brief STATx (0 to 4) data structure
 *
 * @param vref - 2nd reference voltage. Refer to table 105 of datasheet
 * @param itmp - Internal die temperature. Refer to table 105 of datasheet
 * @param vd - Digital power supply voltage (2.7 - 3.6V). Refer to table 106
 *      of datasheet
 * @param va - Analog power supply voltage (4.5 - 5.5V). Refer to table 106
 *     of datasheet
 * @param vres - Voltage across resistor for open wire check. Refer to table 106
 *    of datasheet
 * @param cs_flt - 16-bit combination of C vs. S ADC faults for channels 1 to 16.
 *   Refer to table 108 of datasheet for more details
 * @param ct - Conversion Counter value
 * @param cts - Conversions subcounter value
 * @param va_ov - Analog supply rail overvoltage flag.
 * @param va_uv - Analog supply rail undervoltage flag.
 * @param vd_ov - Digital supply rail overvoltage flag.
 * @param vd_uv - Digital supply rail undervoltage flag.
 * @param other_flags - Other flags (Refer to table 108 of datasheet)
 * @param cell_ovuv - 32-bit combination of over and undervoltage flags for channels
 *  1 to 16. Refer to table 109 of datasheet for more details
 * @param oc_cntr - Oscillator check counter. Refer to table 109 of datasheet
 * @param gpi - 10-bit combination of GPIO levels (MSB -> LSB : GPO10 -> GPO1)
 * @param rev - Device Revision Code
*******************************************************************************/
struct adbms6830_status_data {
    uint16_t vref;
    uint16_t itmp;
    uint16_t vd;
    uint16_t va;
    uint16_t vres;
    uint16_t cs_flt;
    uint16_t ct;
    uint8_t cts;
    bool va_ov;
    bool va_uv;
    bool vd_ov;
    bool vd_uv;
    bool other_flags;
    uint32_t cell_ovuv;
    uint8_t oc_cntr;
    uint16_t gpi;
    uint8_t rev;
};

/***************************************************************************//**
 * @brief Cell (Normal, Averaged, Filtered, S-Voltage) data structure
 *
 * @param cell - array to contain raw and parsed cell data
*******************************************************************************/
struct adbms6830_cell_data {
    uint16_t cell[16];
};

extern const uint16_t adbms6830_cell_commands[];
extern const uint16_t adbms6830_averaged_cell_commands[];
extern const uint16_t adbms6830_filtered_cell_commands[];
extern const uint16_t adbms6830_s_volts_cell_commands[];
extern const uint16_t adbms6830_rd_aux_commands[];
extern const uint16_t adbms6830_rd_aux_redundant_commands[];

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Generate stream to write to CFG A register group */
int adbms6830_cfg_a_gen(struct adbms6830_cfg_a_data cfg_a_data, uint8_t *wr_buf);

/* Parses CFGA data to individual settings */
int adbms6830_cfg_a_parse(struct adbms6830_cfg_a_data *cfg_a_data, uint8_t *reg_data);

/* Generate stream to write to CFG B register group */
int adbms6830_cfg_b_gen(struct adbms6830_cfg_b_data cfg_b_data, uint8_t *wr_buf);

/* Parses CFGB data to individual settings */
int adbms6830_cfg_b_parse(struct adbms6830_cfg_b_data *cfg_b_data, uint8_t *reg_data);

/* Performs ADBMS6830 ADC Polling */
int adbms6830_poll_adc(struct adbms_dev *device, enum adbms6830_poll_adc poll,
        bool is_main);


/* ADC commands */
int adbms6830_adcv(struct adbms_dev *device, uint8_t rd, uint8_t cont, uint8_t dcp, uint8_t rstf,
						uint8_t owcs, bool is_main);

int adbms6830_adax(struct adbms_dev *device, uint8_t owaux, uint8_t pup, uint8_t ch, bool is_main);

int adbms6830_adsv(struct adbms_dev *device, uint8_t cont, uint8_t dcp, uint8_t owcs, bool is_main);


/* Convert Raw Cell Data to Voltage data */
int adbms6830_cells_convert(uint16_t *cell_data, uint8_t len, float *converted);

#endif	/* __ADBMS6830_H__ */

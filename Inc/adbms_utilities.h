/***************************************************************************//**
 *   @file   adbms_utilities.h
 *   @brief  Header file of ADBMS Common Driver.
 *   @author Jeric Oliver Vargas(jeric.vargas@analog.com)
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
#ifndef __ADBMS_UTILITIES_H__
#define __ADBMS_UTILITIES_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_timer.h"
/******************************************************************************/
/************************** ADBMS Common Definitions **************************/

extern struct no_os_timer_desc *timer_desc;
extern uint64_t t1, t2;
extern float time_exec;
extern uint64_t count;
/******************************************************************************/

/* Generates the PEC for a given command */
int adbms_command_pec(uint16_t comm, uint16_t *pec_comm);

/* Generates PEC for data to send or receive */
int adbms_data_pec(uint8_t *data, uint16_t *pec_dat, uint16_t len, bool is_read);


// Function pointer type
typedef int (*timer_func_ptr)(void *);

// Wrapper functions to match the function pointer type
int adbms_timer_counter_get_wrapper(void *counter);
int adbms_timer_get_elapsed_time_nsec_wrapper(void *counter);

int adbms_get_time_count(uint64_t *time_count);
int adbms_get_time_ms(uint64_t start_time_cnt, uint64_t end_time_cnt, float *time_ms);

#endif /* __ADBMS_UTILITIES_H__ */

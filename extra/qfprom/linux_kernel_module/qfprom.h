/**
*  Copyright (C) Gassan Idriss <ghassani@gmail.com>
*
* This program is free software ; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation ; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY ; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with the program ; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* @file qfprom.h
* @package OpenPST Extras - QFPROM Kernel Module
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef __QFPROM_TCP_QFPROM_H
#define __QFPROM_TCP_QFPROM_H

#include <linux/kernel.h>
#include <linux/msm_ion.h>
#include <mach/scm.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/regulator/consumer.h>
#include "debug.h"
#include <linux/qfp_fuse.h>
#include <mach/msm_iomap.h>

#define TZ_SVC_FUSE 8

#define QFP_FUSE_IOC_MAGIC                  0x92
#define QFP_FUSE_IOC_WRITE                  _IO(QFP_FUSE_IOC_MAGIC, 1)
#define QFP_FUSE_IOC_READ                   _IO(QFP_FUSE_IOC_MAGIC, 2)
/*
* 8x26
*/
#define QFPROM_BLOW_TIMER_ADDR          0xFC4BA038
#define QFPROM_BLOW_STATUS_ADDR         0xFC4BA048
#define QFPROM_BLOW_TIMER_CLK_FREQ_MHZ  2.4
#define QFPROM_FUSE_BLOW_TIME_IN_US     4
#define QFPROM_ACCEL_VALUE              0x100
#define QFPROM_ACCEL_RESET_VALUE        0x104
#define QFPROM_INVALID_ENTRY            0xFFFFFFFF
#define QFPROM_BLOW_STATUS_BUSY             0x01
#define QFPROM_BLOW_STATUS_ERROR            0x02

#define QFPROM_BLOW_TIMER_VALUE             (QFPROM_FUSE_BLOW_TIME_IN_US * 83)

enum TZ_SVC_FUSE_COMMANDS {
    TZ_BLOW_SW_FUSE_ID                  = 0x00000001,
    TZ_IS_SW_FUSE_BLOWN_ID              = 0x00000002,
    TZ_QFPROM_WRITE_ROW_ID              = 0x00000003,
    TZ_QFPROM_WRITE_MULTIPLE_ROWS_ID    = 0x00000004,
    TZ_QFPROM_READ_ROW_ID               = 0x00000005,
    TZ_QFPROM_ROLLBACK_WRITE_ROW_ID     = 0x00000006,
};

/**
* SCM structures
*/

// sent as the request structure to scm_call
typedef struct scm_qfprom_read_row_req_s {
    uint32_t           address;     // Physical address of the QFPROM row to read
    int32_t            type;        // Raw or corrected read. 0 = raw, 1 = corrected
    uint32_t           row_data;    // Physical address to location for row results 
    uint32_t           error;       // Physical address to location for operation error
} __attribute__ ((packed)) scm_qfprom_read_row_req_t;

typedef struct scm_qfprom_read_row_resp_s {
    uint32_t    len;
    uint32_t    buf_offset;
    uint32_t    is_complete;
} __attribute__ ((packed)) scm_qfprom_read_row_resp_t;

// request to blow a fuse and write to the row
typedef struct scm_qfprom_write_row_req_s {
  uint32_t  address;        // Physical address of the QFPROM fuse to blow
  uint32_t  row_data;       // Physical address to location for row write values (lsb and msb) 
  uint32_t  bus_clk_khz;    // Bus clock speed in Khz
  uint32_t  error;          // Physical address to location for operation error
} __attribute__ ((packed)) scm_qfprom_write_row_req_t;

typedef struct scm_qfprom_write_row_resp_s {
    uint32_t    len;
    uint32_t    buf_offset;
    uint32_t    is_complete;
} __attribute__ ((packed)) scm_qfprom_write_row_resp_t;

// this stucture represents the response by the qfprom driver with the read result
// error encountered if error not 0
typedef struct scm_qfprom_read_row_data_s {
    uint32_t lsb; 
    uint32_t msb;
    uint32_t error;
} __attribute__ ((packed)) scm_qfprom_read_row_data_t;

// this stucture represents the data written to the fuse if successful
// error encountered if error not 0
typedef struct scm_qfprom_write_row_data_s {
    uint32_t lsb; 
    uint32_t msb;
    uint32_t error;
} __attribute__ ((packed)) scm_qfprom_write_row_data_t;


int tz_qfprom_read_row(uint32_t address, uint32_t type, scm_qfprom_read_row_data_t* row_data);
int tz_qfprom_write_row(uint32_t address, uint32_t lsb, uint32_t msb, uint32_t bus_clk_khz, scm_qfprom_write_row_data_t* row_data);
int qfprom_read_direct(uint32_t address, scm_qfprom_read_row_data_t* row_data);
int tz_qfprom_write_direct(uint32_t address, uint32_t lsb, uint32_t msb, uint32_t bus_clk_khz, scm_qfprom_write_row_data_t* row_data);


#endif // __QFPROM_TCP_QFPROM_H

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
* @file server.h
* @package OpenPST Extras - QFPROM Kernel Module
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef __QFPROM_TCP_SERVER_H
#define __QFPROM_TCP_SERVER_H

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <net/sock.h>
#include <net/tcp.h>
#include "qfprom.h"


enum QFPROM_TCP_REQUEST {
    QFPROM_TCP_DISCONNECT_REQUEST   = 0,
    QFPROM_TCP_READ_REQUEST         = 1,
    QFPROM_TCP_WRITE_REQUEST        = 2,
    QFPROM_TCP_READ_DIRECT_REQUEST  = 3
};

/**
* QFPROM request and response structures
*/
typedef struct qfprom_tcp_read_row_req_s {
    uint32_t address;
    uint32_t type;
} __attribute__ ((packed)) qfprom_tcp_read_row_req_t;

typedef struct qfprom_tcp_read_row_resp_s {
    uint32_t address;
    uint32_t type;
    uint32_t lsb;
    uint32_t msb;
    uint32_t error;
} __attribute__ ((packed)) qfprom_tcp_read_row_resp_t;


typedef struct qfprom_tcp_write_row_req_s {
    uint32_t address;
    uint32_t lsb;
    uint32_t msb;
    uint32_t bus_clk_khz;
} __attribute__ ((packed)) qfprom_tcp_write_row_req_t;

typedef struct qfprom_tcp_write_row_resp_s {
    uint32_t address;
    uint32_t lsb;
    uint32_t msb;
    uint32_t bus_clk_khz;
    uint32_t error;
} __attribute__ ((packed)) qfprom_tcp_write_row_resp_t;


typedef struct qfprom_tcp_req_s {
    int8_t command;
    union {
        qfprom_tcp_read_row_req_t read;
        qfprom_tcp_write_row_req_t write;
    };
} __attribute__ ((packed)) qfprom_tcp_req_t;

typedef struct qfprom_tcp_resp_s {
    int8_t command;
    union {
        qfprom_tcp_read_row_resp_t read;
        qfprom_tcp_write_row_resp_t write;
    };
} __attribute__ ((packed)) qfprom_tcp_resp_t;

int tcp_server_start(int port);
void tcp_server_shutdown(void);
int tcp_server_thread(void);
int tcp_server_receive(struct socket* sock, uint8_t* buffer, int length);
int tcp_server_send(struct socket* sock, uint8_t* data, int length);

#endif // __QFPROM_TCP_SERVER_H





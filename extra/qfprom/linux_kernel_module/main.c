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
* @file main.c
* @package OpenPST Extras - QFPROM Kernel Module
* @brief This linux kernel module allows you to read/write QFPROM rows
*			where permission is granted. Optionally can run as a tcp
*			server so you can call to it from outside of kernel space.
*			@see client.py for connecting to the server
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include "qfprom.h"
#include "server.h"
#include "debug.h"

typedef struct qfprom_read_operation_entry_s {
	char name[75];
	uint32_t address;
	bool corrected;
} qfprom_read_operation_entry_t;

typedef struct qfprom_write_operation_entry_s {
	char name[75];
	uint32_t address;
	uint32_t lsb;
	uint32_t msb;
	uint32_t bus_clk_khz;
} qfprom_write_operation_entry_t;

static short int start_tcp = 0;


static qfprom_read_operation_entry_t read_rows_table[] = {
	// Enter the rows you wish to add to the read list
	//   name   address     corrected
	//{ "Name", 0x80000000, 0x00000000 }
};

static qfprom_write_operation_entry_t write_rows_table[] = {
	// Enter the rows you wish to add to the write list
	//   name   address     lsb         msb         bus_clk_khz
	//{ "Name", 0x80000000, 0x00000000, 0x00000000, 0x00000000 },
	//{ "Test", 0xFC4B80B8, 0x00000000, 0x00000000, 0x00000000}
};

static int __init tz_qfprom_init(void)
{
	int i;
	int read_count  = sizeof(read_rows_table)/sizeof(qfprom_read_operation_entry_t);
	int write_count = sizeof(write_rows_table)/sizeof(qfprom_write_operation_entry_t);
	scm_qfprom_read_row_data_t read_row_data = {};
	scm_qfprom_write_row_data_t write_row_data = {};

	if (start_tcp > 0) {
		tcp_server_start(start_tcp);
	}

	if (sizeof(read_rows_table) > 0) {
		for (i=0; i < read_count; i++) {
			log("%s - 0x%08X - %d\n", read_rows_table[i].name, read_rows_table[i].address, read_rows_table[i].corrected);
			tz_qfprom_read_row(read_rows_table[i].address, (read_rows_table[i].corrected ? 0x01 : 0x00), &read_row_data);
		}
	}

	if (sizeof(write_rows_table) > 0) {
		for (i=0; i < write_count; i++) {
			tz_qfprom_write_row(write_rows_table[i].address, write_rows_table[i].lsb, write_rows_table[i].msb, write_rows_table[i].bus_clk_khz, &write_row_data);
		}
	}
	
    return 0;
}

static void __exit tz_qfprom_exit(void)
{
	if (start_tcp > 0) {
		tcp_server_shutdown();
	}
}

module_init(tz_qfprom_init);
module_exit(tz_qfprom_exit);

MODULE_DESCRIPTION("QFPROM");
MODULE_AUTHOR("Gassan Idriss <ghassani@gmail.com>");
MODULE_LICENSE("GPL");

module_param(start_tcp, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(start_tcp, "Set to port you wish to enable the tcp server on. By default this is not enabled.");

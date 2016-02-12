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
*/

#ifndef _LAF_H
#define _LAF_H

#include "include/definitions.h"

#define LAF_EXEC_MAGIC 0xBCBAA7BA
#define LAF_CTRL_MAGIC 0xB3ADABBC

#define LAF_MAX_DATA_SIZE 1048578

#define LAF_ENTER 0x3AA16E7E

#define LAF_CMD_WRITE		0x45545257 // WRTE
#define LAF_CMD_READ		0x44414552 // READ
#define LAF_CMD_IOCT		0x54434F49 // IOCT
#define LAF_CMD_OPEN		0x4E45504F // OPEN
#define LAF_CMD_CLSE		0x45534C43 // CLSE
#define LAF_CMD_UNLINK		0x4B4C4E55 // UNLK
#define LAF_CMD_ERASE		0x45535245 // ERSE
#define LAF_CMD_CONTROL		0x4C525443 // CTRL
#define LAF_CMD_HELO		0x4F4C4548 // HELO
#define LAF_CMD_EXECUTE		0x43455845 // EXEC
#define LAF_CMD_RSVD		0x44565352 // RSVD
#define LAF_CMD_FAIL		0x4C494146 // FAIL
#define LAF_CMD_INFO		0x4F464E49 // INFO
#define LAF_CMD_MISC		0x4353494D // MISC
#define LAF_CMD_KILO		0x4F4C494B // KILO

#define LAF_CMD_INFO_SPRO  0x4F525053 // ORPS
#define LAF_CMD_INFO_GPRO  0x4F525047 // GPRO

#define LAF_CMD_CONTROL_POWER_OFF	0x46464F50 // POFF
#define LAF_CMD_CONTROL_RESET		0x54455352 // REST
#define LAF_CMD_CONTROL_ONRS		0x53524E4F // ONRS
#define LAF_CMD_CONTROL_RSON		0x4E4F5352 // RSON

#define LAF_CMD_KILO_METR		0x5254454D // METR
#define LAF_CMD_KILO_CENT		0x544E4543 // CENT

#define LAF_MISC_PARITION_OFFSET(number) \ number << 9
/*
This functionality is part of the laf kernel image in the / sbin / lafd binary file

[27.355346 / 03 - 31 05:04 : 58.419][LAF] laf_message.command = 0x43455845(EXEC)
[27.355398 / 03 - 31 05:04 : 58.419][LAF] laf_message.arg0 = 0x0
[27.355438 / 03 - 31 05:04 : 58.419][LAF] laf_message.arg1 = 0x0
[27.355477 / 03 - 31 05:04 : 58.419][LAF] laf_message.arg_opt0 = 0x0
[27.355517 / 03 - 31 05:04 : 58.419][LAF] laf_message.arg_opt1 = 0x0
[27.355557 / 03 - 31 05:04 : 58.419][LAF] laf_message.data_length = 0x5
[27.355597 / 03 - 31 05:04 : 58.419][LAF] laf_message.data_check = 0xdd13
[27.355636 / 03 - 31 05:04 : 58.419][LAF] laf_message.magic = 0xbcbaa7ba
*/

PACKED(typedef struct LafCmdHeader{
	uint32_t command;
	uint32_t arg0;
	uint32_t arg1;
	uint32_t arg_opt0;
	uint32_t arg_opt1;
	uint32_t size;
	uint32_t crc;
	uint32_t magic;
}) LafCmdHeader;

PACKED(typedef struct LafCmd{
	LafCmdHeader header;
	uint8_t data[2]; // must trail with 2 null bytes and requires at least 2 null bytes if no data
}) LafCmd;
#endif // _LAF_H
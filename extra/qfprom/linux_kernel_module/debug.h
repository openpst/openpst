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
* @file debug.h
* @package OpenPST Extras - QFPROM Kernel Module
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _QFPROM_TCP_DEBUG_H
#define _QFPROM_TCP_DEBUG_H

#define log(fmt, args...)	printk("tz_qfprom : %s "fmt, __FUNCTION__, ##args)

#endif // _QFPROM_TCP_DEBUG_H
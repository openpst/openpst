/**
* LICENSE PLACEHOLDER
*
* @file endian.h
* @package OpenPST
* @brief endian helper functions
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author https://github.com/posixninja/DLOADTool
*/

#ifndef _UTIL_ENDIAN_H
#define _UTIL_ENDIAN_H

#include "include/definitions.h"

uint16_t  flip_endian16(uint16_t  value);

uint32_t  flip_endian32(uint32_t value);


#endif

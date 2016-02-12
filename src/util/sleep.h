/**
* LICENSE PLACEHOLDER
*
* @file sleep.h
* @package OpenPST
* @brief multi platform sleep functions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _UTIL_SLEEP_H
#define _UTIL_SLEEP_H
#ifdef _WIN32

#include <windows.h>
#else
#include <unistd.h>
#endif

void sleep(int milliseconds);

#endif // _UTIL_SLEEP_H

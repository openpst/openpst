/**
* LICENSE PLACEHOLDER
*
* @file definitions.h
* @package OpenPST
* @brief standard definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _INCLUDE_DEFINITIONS_H
#define _INCLUDE_DEFINITIONS_H


#if defined(_WIN32) && !defined(__MINGW32__)
	#include "win_stdint.h"
	#include "win_inttypes.h"

    #define PACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )

#else
	#include <stdint.h>
    #define PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef DEBUG
#define LOGD(...) fprintf(stderr, __VA_ARGS__); 
#else
#define LOGD(...)
#endif

#define LOGE(...) fprintf(stderr, __VA_ARGS__); 
#define LOGI(...) fprintf(stdout, __VA_ARGS__); 


#endif //_INCLUDE_DEFINITIONS_H

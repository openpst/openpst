/**
* LICENSE PLACEHOLDER
*
* @file sleep.h
* @package OpenPST
* @brief multi platform sleep functions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "sleep.h"

void sleep(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

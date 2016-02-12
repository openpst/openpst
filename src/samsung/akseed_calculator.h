/**
* LICENSE PLACEHOLDER
*
* @file akseed.h
* @package OpenPST
* @brief Samsung AKSEED Calculations
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _SAMSUNG_AKSEED_CALCULATOR_H
#define _SAMSUNG_AKSEED_CALCULATOR_H

#include <string>
#include <math.h>
#include <sstream>
#include "include/definitions.h"
#include <tuple>

namespace OpenPST {
	namespace Samsung {

		enum AkseedModelOffset {
			kAkseedModelMarvell		= 0,
			kAkseedModelSMN900S		= 51,
			kAkseedModelKorea		= 51,
			kAkseedModelSMN900L		= 71,
			kAkseedModelSMG900L		= 47,
			kAkseedModelSMG900K		= 54,
			kAkseedModelSMG900S		= 73,
			kAkseedModelSHVE470S	= 73,
			kAkseedModelSMN900K		= 61,
			kAkseedModelSHVE250K	= 88,
			kAkseedModelDefault		= 89,
		};

		class AkseedCalculator {

			public:

				static std::tuple<int, int, int> calculate(int number1, int number2, int number3, int addOffset = kAkseedModelDefault);
				static std::string calculateAsString(int number1, int number2, int number3, int addOffset = kAkseedModelDefault);

		};
	}
}

#endif // _SAMSUNG_AKSEED_H


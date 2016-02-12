/**
* LICENSE PLACEHOLDER
*
* @file akseed.cpp
* @package OpenPST
* @brief Samsung AKSEED Calculations
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "akseed_calculator.h"

using namespace OpenPST::Samsung;


std::tuple<int, int, int> AkseedCalculator::calculate(int number1, int number2, int number3, int addOffset)
{
	int i = 1;
	int calc1, calc2, calc3;

	calc1 = number2;
	while (i <= (number3 / 2)){
		calc1 = (((number2 * number2) % number1) * calc1) % number1;
		i++;
	}

	calc1 += addOffset;

	calc2 = round(fmod((number1 * 0x181E5), log(number1))) + addOffset;
	calc3 = round(((number2 * number1) / number1) / log(number2)) + addOffset;

	return std::make_tuple(calc1, calc2, calc3);
}

std::string AkseedCalculator::calculateAsString(int number1, int number2, int number3, int addOffset)
{
	std::stringstream ss; 
	std::tuple<int, int, int> res = calculate(number1, number2, number3, addOffset);

	ss << std::get<0>(res) << "-" << std::get<1>(res) << "-" << std::get<2>(res);

	return ss.str();
}

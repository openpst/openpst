#include "include/definitions.h"
#include "samsung/akseed.h"


using namespace std;
using OpenPST::Samsung::AkseedCalculator;

int testData[3][2][3] = {
	// In				  // Expected
	{ { 2021, 145, 167 }, { 425, 96, 118 } },
	{ { 2491, 83, 101 }, { 1179, 95, 108 } },
	{ { 3959, 124, 143 }, { 1219, 94, 115 } },
};

int main() {
	int i;
	std::string result;

	for (i = 0; i < 3; i++) {
		printf("[*] Testing: %d-%d-%d\n", testData[i][0][0], testData[i][0][1], testData[i][0][2]); 
		auto res = AkseedCalculator::calculate(testData[i][0][0], testData[i][0][1], testData[i][0][2]);
		
		if (std::get<0>(res) != testData[i][1][0] || std::get<1>(res) != testData[i][1][1] || std::get<2>(res) != testData[i][1][2]) {
			printf("[-] Result: %d-%d-%d Expected: %d-%d-%d\n", std::get<0>(res), std::get<1>(res), std::get<2>(res), testData[i][1][0], testData[i][1][1], testData[i][1][2]);
		} else {
			printf("[+] Result: %d-%d-%d\n", std::get<0>(res), std::get<1>(res), std::get<2>(res));
		}
	}

	return 0;
}

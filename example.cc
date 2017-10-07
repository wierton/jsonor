#include "json.h"


int main() {
	Json json("{'12':[1, 2, 45, '45', {'67':'what'}], 'nmb':123}");

	std::cout << json["12"].to_string() << std::endl;
	std::cout << json["12"][3].to_int() << std::endl;
	std::cout << json["12"][4].to_string() << std::endl;
	std::cout << json["12"][4]["67"].to_string() << std::endl;
	std::cout << json.to_string() << std::endl;
	return 0;
}

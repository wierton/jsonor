#include "json.h"
#include <iostream>


int main() {
	Json json = Json::parse("{'12':[1, 2, 45, '45', {'67':'what'}], 'nm':123}");

	std::cout << json["12"].to_string() << std::endl;
	std::cout << json["12"][2].to_integer() << std::endl;
	std::cout << json["12"][4].to_string() << std::endl;
	std::cout << json["12"][4]["67"].to_string() << std::endl;
	std::cout << json.to_string() << std::endl;

	json["k4"] = 7;
	std::cout << json << std::endl;
	json["k4"] = "123";
	std::cout << json << std::endl;
	json["k4"] = Json::Array {2, 4.2, "as"};
	std::cout << json << std::endl;
	json["k4"] = Json::Object {{"c4","v5"}, {"r1", "t7"}};
	std::cout << json << std::endl;

	return 0;
}

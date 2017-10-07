#include "json.h"


int main() {
	Json json("{'12':[1, 2, 45, '45', {'67':'what'}], 'nm':123}");

	std::cout << json["12"].to_string() << std::endl;
	std::cout << json["12"][3].to_int() << std::endl;
	std::cout << json["12"][4].to_string() << std::endl;
	std::cout << json["12"][4]["67"].to_string() << std::endl;
	std::cout << json.to_string() << std::endl;


	json["k4"] = Json("{'c4':'v5', '7': ' h e l l o ' }");
	std::cout << json["k4"] << std::endl;

	Json array("[]");
	array.extend(json["12"]);
	std::cout << array << std::endl;

	array.append(json);
	std::cout << array << std::endl;

	return 0;
}

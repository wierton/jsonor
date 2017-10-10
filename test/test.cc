#include "json.h"
#include "debug.h"
#include <string>
#include <fstream>

int main(const int argc, const char * const argv[]) {
	for(int i = 1; i < argc; i++) {
		std::cout << "==========TEST@" << argv[i] << "===========\n";
		std::ifstream ifs(std::string(argv[i]));
		try {
			Json json = Json::parse(ifs);
			std::cout << json.to_string() << std::endl;
		} catch(ParseException &e) {
			std::cout << VT_COLOR_RED << e.reason << VT_RESET << std::endl;
		}
	}
	return 0;
}

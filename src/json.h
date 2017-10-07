#ifndef JSON_H
#define JSON_H


#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace wt {

inline void ostream_print(std::ostream &os, const char * fmt) {
	while(*fmt) {
		if(*fmt == '%')
			fmt ++;
		os << *fmt;
		fmt ++;
	}
}

template<class T, class... Args>
void ostream_print(std::ostream &os, const char * fmt, T firstArg, Args... restArgs) {
	while(*fmt) {
		if(*fmt == '%') {
			fmt ++;
			if(*fmt == '%') {
				os << '%';
				fmt ++;
				continue;
			} else {
				os << firstArg;
				ostream_print(os, fmt, restArgs...);
				return;
			}
		}
		os << *fmt;
		fmt ++;
	}
}

template<class... Args>
inline std::size_t sprint(std::string &sout, const char *fmt, Args... args) {
	std::ostringstream os;
	ostream_print(os, fmt, args...);
	sout = os.str();
	return sizeof...(Args);
}

template<class... Args>
inline std::string sformat(const char *fmt, Args... args) {
	std::ostringstream os;
	ostream_print(os, fmt, args...);
	return os.str();
}

}

class ParseException {
public:
	std::string reason;

	ParseException(const char *reason) : reason(reason) {}
	ParseException(std::string &reason) : reason(reason) {}
	ParseException(std::string &&reason) : reason(std::move(reason)) {}
};

class Json {
public:
	enum JsonType {
		JT_NONE,
		JT_NULL,
		JT_TRUE,
		JT_FALSE,
		JT_ARRAY,
		JT_DICT,
		JT_INT,
		JT_FLOAT,
		JT_STRING,
		JT_ERROR,
	};

private:
	JsonType type;

	int integer;
	double floating;
	std::string string;
	std::vector<Json> array;
	std::map<std::string, Json> dict;

	static void EraseSpace(std::istream & is);
	static void MatchPrimary(Json & to, std::istream & is);
	static void MatchIdent(Json & to, std::istream & is);
	static void MatchNumber(Json & to, std::istream & is);
	static void MatchString(Json & to, std::istream & is);
	static void MatchArray(Json & to, std::istream & is);
	static void MatchDict(Json & to, std::istream & is);

	void stringify(std::string & to);
public:
	Json() = default;
	Json(std::istream & is);
	Json(const char *string);
	Json(std::string &string);
	Json(const Json & other) = default;
	Json(Json && other) = default;

	Json & operator [] (const int idx);
	Json & operator [] (const std::string &key);

	JsonType gettype();
	int to_int();
	double to_float();
	std::string to_string();
};


#endif

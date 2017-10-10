#ifndef JSON_H
#define JSON_H


#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>

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
		JT_OBJECT,
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
	std::map<std::string, Json> object;

	static void EraseSpace(std::istream & is);
	static void MatchPrimary(Json & to, std::istream & is);
	static void MatchIdent(Json & to, std::istream & is);
	static void MatchNumber(Json & to, std::istream & is);
	static void MatchString(Json & to, std::istream & is);
	static void MatchArray(Json & to, std::istream & is);
	static void MatchDict(Json & to, std::istream & is);

	void stringify(std::string & to);

public:
	using Array = std::vector<Json>;
	using Object = std::map<std::string, Json>;

	Json() = default;

	Json(Array &&);
	Json(Object &&);

	Json(const Json &) = default;
	Json(Json &&) = default;
	Json & operator = (const Json &) = default;
	Json & operator = (Json &&) = default;

	Json(int i);
	Json(double f);
	Json(const char *s);
	Json(std::string &string);
	Json(std::string &&string);

	Json & operator [] (const int idx);
	Json & operator [] (const std::string &key);

	size_t size();
	void append(const Json &json);
	void append(Json && json);
	void extend(const Json &json);
	void extend(Json && json);

	static Json parse(std::istream & is);
	static Json parse(std::string s);

	JsonType gettype();
	int to_int();
	double to_float();
	std::string to_string();

	friend std::ostream &operator << (std::ostream &, Json &);
	friend std::ostream &operator << (std::ostream &, Json &&);
};


#endif

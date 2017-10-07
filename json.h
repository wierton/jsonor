#ifndef JSON_H
#define JSON_H


#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

class Json {
public:
	enum JsonType {
		JT_NONE,
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

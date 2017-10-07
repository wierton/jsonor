#include "json.h"


inline bool isEof(std::istream & is) {
	is.peek();
	return !is.good();
}

Json::Json(std::istream &is) {
	MatchPrimary(*this, is);
}

Json::Json(const char *inputString) {
	std::istringstream iss = std::istringstream(std::string(inputString));
	MatchPrimary(*this, iss);
}

Json::Json(std::string &inputString) {
	std::istringstream iss(inputString);
	MatchPrimary(*this, iss);
}

void Json::EraseSpace(std::istream & is) {
	while(!isEof(is)) {
		char ch = is.peek();
		if(isspace(ch)) {
			is.ignore();
		} else {
			break;
		}
	}
}

void Json::MatchPrimary(Json & to, std::istream & is) {
	EraseSpace(is);
	char ch = is.peek();

	switch(ch) {
		case '_':
		case 'a'...'z':
		case 'A'...'Z':
			MatchIdent(to, is);
			break;
		case '+':
		case '-':
		case '0'...'9':
			MatchNumber(to, is);
			break;
		case '\'':
		case '"':
			MatchString(to, is);
			break;
		case '[':
			MatchArray(to, is);
			break;
		case '{':
			MatchDict(to, is);
			break;
		default:
			to.type = JT_ERROR;
			throw ParseException(wt::sformat("unexpected character '%'", ch));
			break;
	}
}

void Json::MatchIdent(Json & to, std::istream & is) {
	while(!isEof(is)) {
		char ch = is.get();

		if(isalnum(ch) || ch == '_') {
			to.string.push_back(ch);
		} else {
			is.unget();
			break;
		}
	}

	if(to.string == "true") {
		to.type = JT_TRUE;
	} else if(to.string == "false") {
		to.type = JT_FALSE;
	} else if(to.string == "null") {
		to.type = JT_NULL;
	} else {
		throw ParseException(wt::sformat("unexpected identifier %", to.string));
	}
}

void Json::MatchNumber(Json & to, std::istream & is) {
	to.type = JT_INT;
	while(!isEof(is)) {
		char ch = is.get();

		if(isdigit(ch) || ch == 'x' || ch == 'X'
				|| ch == '+' || ch == '-') {
			to.string.push_back(ch);
		} else if(ch == '.' || ch == 'e' || ch == 'E') {
			to.type = JT_FLOAT;
			to.string.push_back(ch);
		} else {
			is.unget();
			break;
		}
	}

	std::istringstream iss(to.string);
	if(to.type == JT_INT) {
		iss >> to.integer;
	} else  {
		iss >> to.floating;
	}

	if(!isEof(iss)) {
		throw ParseException(wt::sformat("invalid number %", to.string));
	}
}


void Json::MatchString(Json & to, std::istream & is) {
	to.type = JT_STRING;
	char quote = is.get();
	while(!isEof(is)) {
		char ch = is.get();

		if(ch == quote) {
			break;
		} else if(ch != '\\') {
			if(ch == '\n')
				throw ParseException("line break in string literal");
			to.string.push_back(ch);
			continue;
		}

		is.get(ch);
		switch(ch) {
			case '\\': to.string.push_back('\\'); break;
			case 'n':  to.string.push_back('\n'); break;
			case 'r':  to.string.push_back('\r'); break;
			case 'f':  to.string.push_back('\f'); break;
			case 'v':  to.string.push_back('\v'); break;
			case 't':  to.string.push_back('\t'); break;
			case 'b':  to.string.push_back('\b'); break;
			case 'u':
			case 'U': {
				uint16_t u16 = 0;
				char *p = reinterpret_cast<char*>(&u16);
				is >> std::hex >> u16;
				if(u16 != static_cast<char>(u16))
				   to.string.push_back(p[1]);
				to.string.push_back(p[0]);

				if(is.fail()) {
					throw ParseException("invalid unicode character");
				}
				break;
			}
			case 'x':
			case 'X':
				is >> std::hex >> ch;
				to.string.push_back(ch);

				if(is.fail()) {
					is.sync();
					throw ParseException("invalid unicode character");
				}
				break;
			case '0':
				is >> std::oct >> ch;
				to.string.push_back(ch);
				break;
			case '\n':
				throw ParseException("line break in string literal");
			default:
				to.string.push_back(ch);
				break;
		}
	}
}

void Json::MatchArray(Json & to, std::istream & is) {
	to.type = JT_ARRAY;
	is.ignore();

	EraseSpace(is);
	char ch = is.peek();
	if(ch == ']') {
		is.ignore();
		return;
	}

	while(!isEof(is)) {
		Json subJson;
		MatchPrimary(subJson, is);

		to.array.push_back(std::move(subJson));

		EraseSpace(is);
		is.get(ch);

		if(ch == ']')
			break;
		else if(ch == ',')
			continue;
		else
			throw ParseException(wt::sformat("unexpected character '%'", ch));
	}

	// log("JT_ARRAY:%:%\n", to.to_string());
}

void Json::MatchDict(Json & to, std::istream & is) {
	to.type = JT_DICT;
	is.ignore();

	EraseSpace(is);
	char ch = is.peek();
	if(ch == '}') {
		is.ignore();
		return;
	}

	while(!isEof(is)) {
		/* match key */
		Json keyJson;
		MatchPrimary(keyJson, is);

		/* only string can be key */
		if(keyJson.type != JT_STRING) {
			to.type = JT_ERROR;
			throw ParseException("key of object isn't string");
		}

		/* match ':' */
		EraseSpace(is);
		is.get(ch);
		if(ch != ':') {
			to.type = JT_ERROR;
			throw ParseException(wt::sformat("missing colon"));
		}

		/* match value */
		Json valueJson;

		MatchPrimary(valueJson, is);

		to.dict.insert(std::make_pair<std::string, Json>(std::move(keyJson.string), std::move(valueJson)));

		EraseSpace(is);
		is.get(ch);

		if(ch == '}')
			break;
		else if(ch == ',')
			continue;
		else
			throw ParseException(wt::sformat("unexpected character '%'", ch));
	}
}

Json & Json::operator [] (const int idx) {
	if(type == JT_ARRAY) {
		if(idx >= array.size())
			throw std::out_of_range("index overflow");
		return array[idx];
	}

	throw std::out_of_range("use subscript to access something not a array");
}

Json & Json::operator [] (const std::string &key) {
	return dict[key];
}

void Json::stringify(std::string & to) {
	switch(type) {
		case JT_NONE:
			to += "<none>";
			break;
		case JT_NULL:
			to += "null";
			break;
		case JT_TRUE:
			to += "true";
			break;
		case JT_FALSE:
			to += "false";
			break;
		case JT_INT:
			to += std::to_string(integer);
			break;
		case JT_FLOAT:
			to += std::to_string(floating);
			break;
		case JT_STRING:
			to.push_back('"');
			to += string;
			to.push_back('"');
			break;
		case JT_ARRAY:
			to.push_back('[');
			for(int i = 0; i < array.size(); i++) {
				array[i].stringify(to);

				if(i + 1 < array.size())
					to += ", ";
			}
			to.push_back(']');
			break;
		case JT_DICT:
			to.push_back('{');
			for(auto it = dict.begin(); it != dict.end(); it ++) {
				if(it->second.type == JT_NONE)
					continue;

				to.push_back('"');
				to += it->first;
				to.push_back('"');

				to.push_back(':');
				(it->second).stringify(to);

				to += ", ";
			}
			if(dict.begin() != dict.end())
				to.erase(to.size() - 2);
			to.push_back('}');
			break;
		default:
			throw ParseException("internal error");
	}
}

size_t Json::size() {
	return array.size();
}

void Json::append(const Json &json) {
	if(type == JT_ARRAY) {
		array.push_back(json);
	} else {
		throw std::out_of_range("use subscript to access something not a array");
	}
}

void Json::append(Json && json) {
	if(type == JT_ARRAY) {
		array.push_back(std::move(json));
		json.type = JT_NONE;
	} else {
		throw std::out_of_range("use subscript to access something not a array");
	}
}

void Json::extend(const Json &json) {
	if(type == JT_ARRAY && json.type == JT_ARRAY) {
		for(auto & subJson : json.array)
			array.push_back(subJson);
	} else {
		throw std::out_of_range("use subscript to access something not a array");
	}
}

void Json::extend(Json &&json) {
	if(type == JT_ARRAY && json.type == JT_ARRAY) {
		for(auto & subJson : json.array) {
			array.push_back(std::move(subJson));
			subJson.type = JT_NONE;
		}
	} else {
		throw std::out_of_range("use subscript to access something not a array");
	}
}

Json::JsonType Json::gettype() {
	return type;
}

int Json::to_int() {
	if(type == JT_INT)
		return integer;
	else if(type == JT_FLOAT)
		return floating;
	else {
		int ret;
		std::istringstream(string) >> ret;
		return ret;
	}
}

double Json::to_float() {
	if(type == JT_INT)
		return integer;
	else if(type == JT_FLOAT)
		return floating;
	else {
		double ret;
		std::istringstream(string) >> ret;
		return ret;
	}
}

std::string Json::to_string() {
	std::string to;
	stringify(to);
	return to;
}

std::ostream &operator << (std::ostream &os, Json &json) {
	os << json.to_string();
	return os;
}

std::ostream &operator << (std::ostream &os, Json &&json) {
	os << json.to_string();
	return os;
}

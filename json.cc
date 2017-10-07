#include "json.h"
#include "debug.h"


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
	dynamic_assert(iss.good(), "convert to istringstream fails...\n");
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
			loge("JT_ERROR:'%:%'\n", ch, static_cast<int>(ch));
			break;
	}
}

void Json::MatchNumber(Json & to, std::istream & is) {
	to.type = JT_INT;
	while(!isEof(is)) {
		char ch = is.get();

		if(isdigit(ch)) {
			to.string.push_back(ch);
		} else if(ch == '.' || ch == 'e') {
			to.type = JT_FLOAT;
			to.string.push_back(ch);
		} else {
			is.unget();
			break;
		}
	}

	if(to.type == JT_INT) {
		std::istringstream(to.string) >> to.integer;
	} else  {
		std::istringstream(to.string) >> to.floating;
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
			case 'x':
			case 'X':
					   is >> std::hex >> ch;
					   to.string.push_back(ch);
					   break;
			case '0':
					   is >> std::oct >> ch;
					   to.string.push_back(ch);
					   break;
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
			loge("expected ',' or ']'\n");
	}

	log("JT_ARRAY:%:%\n", to.to_string());
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
			loge("JT_ERROR\n");
			break;
		}

		/* match ':' */
		EraseSpace(is);
		is.get(ch);
		if(ch != ':') {
			to.type = JT_ERROR;
			loge("JT_ERROR\n");
			break;
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
			loge("expected ',' or '}' here");
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
	auto it = dict.find(key);

	if(it != dict.end()) {
		return it->second;
	}

	throw std::out_of_range("use key to access something not a dict");
}

void Json::stringify(std::string & to) {
	switch(type) {
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
			loge("JT_NONE or JT_ERROR\n");
			break;
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


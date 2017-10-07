#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "vt100.h"

#define ERROR_TAG    VT_COLOR_RED    "[ERROR] "    VT_RESET
#define WARNING_TAG  VT_COLOR_YELLOW "[WARNING] "  VT_RESET
#define DEBUG_TAG    VT_COLOR_BLUE   "[DEBUG] "    VT_RESET

/* wt's debug */
namespace wtd {

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
void log_r(const char *fmt, Args... args) {
	ostream_print(std::cerr, fmt, args...);
}

/* FIXME: should not be put here */
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

template<class... Args>
size_t log_tag(const char *tag, const char *file, const char *func, const int line, const char *fmt, Args... args) {
	std::clog << tag << file << ": " << func << ": " << line << ": ";
	log_r(fmt, args...);
	return sizeof...(Args);
}

} // end of namespace wt

#define log(...) wtd::log_tag(DEBUG_TAG, __FILE__, __func__, __LINE__, __VA_ARGS__)

#define loge(...) do {	\
	wtd::log_tag(ERROR_TAG, __FILE__, __func__, __LINE__, __VA_ARGS__);	\
	abort();			\
} while(0)

#define dynamic_assert(cond, ...) do {	\
	if(!(cond)) {						\
		loge("" __VA_ARGS__);				\
	}									\
} while(0)

#endif

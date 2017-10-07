#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "vt100.h"
#include "json.h"

#define ERROR_TAG    VT_COLOR_RED    "[ERROR] "    VT_RESET
#define WARNING_TAG  VT_COLOR_YELLOW "[WARNING] "  VT_RESET
#define DEBUG_TAG    VT_COLOR_BLUE   "[DEBUG] "    VT_RESET

/* wt's debug */
namespace wtd {

template<class... Args>
void log_r(const char *fmt, Args... args) {
	wt::ostream_print(std::cerr, fmt, args...);
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

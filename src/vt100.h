#ifndef VT100_H
#define VT100_H

/* reset terminal style and color */
#define VT_RESET           "\033[0m"

/*output style*/
#define VT_STYLE_BOLD      "\033[1m"
#define VT_STYLE_DARK      "\033[2m"
#define VT_STYLE_BACK      "\033[3m"
#define VT_STYLE_UNDERLINE "\033[4m"

/* background color */
#define VT_BG_BLACK     "\033[40m"
#define VT_BG_RED       "\033[41m"
#define VT_BG_GREEN     "\033[42m"
#define VT_BG_YELLOW    "\033[43m"
#define VT_BG_BLUE      "\033[44m"
#define VT_BG_PURPLE    "\033[45m"
#define VT_BG_SKYBLUE   "\033[46m"
#define VT_BG_WHITE     "\033[47m"
#define VT_BG_NORMAL    "\033[48m"

/*output color*/
#define VT_COLOR_BLACK     "\033[30m"
#define VT_COLOR_RED       "\033[31m"
#define VT_COLOR_GREEN     "\033[32m"
#define VT_COLOR_YELLOW    "\033[33m"
#define VT_COLOR_BLUE      "\033[34m"
#define VT_COLOR_PURPLE    "\033[35m"
#define VT_COLOR_SKYBLUE   "\033[36m"
#define VT_COLOR_WHITE     "\033[37m"
#define VT_COLOR_NORMAL    "\033[38m"

#endif

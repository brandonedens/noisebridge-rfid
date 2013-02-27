// Routines for logging to the console
// Copyright (C) 2012 Zed Shaw. All Rights Reserved
// Copyright (C) 2013 Brandon Edens. All Rights Reserved
// Author: Zed Shaw
//         Brandon Edens
// Date: 2013-02-13

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "color.h"

/** Enable color logging. */
#define LOG_COLOR

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }
#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define check_mem(A) check((A), "Out of memory.")


#ifdef LOG_COLOR
#define LOG_FILENAME COLOR_YELLOW "%s" COLOR_NORMAL
#define LOG_LINENO COLOR_GREEN "%d" COLOR_NORMAL
#define LOG_ERRNO COLOR_BOLD_BLACK "errno: %s" COLOR_NORMAL
#define LOG_SOURCE "(" LOG_FILENAME ":" LOG_LINENO ")"
#define LOG_SOURCE_AND_ERRNO "(" LOG_FILENAME ":" LOG_LINENO ": " LOG_ERRNO ")"
#ifdef NDEBUG
#define log_debug(M, ...)
#else /* NDEBUG */
#define log_debug(M, ...) fprintf(stderr, COLOR_BG_BLUE " D " COLOR_NORMAL LOG_SOURCE " " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* NDEBUG */
#define log_err(M, ...)  fprintf(stderr, COLOR_BG_RED " E " COLOR_NORMAL LOG_SOURCE_AND_ERRNO " " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, COLOR_BG_YELLOW " W " COLOR_NORMAL LOG_SOURCE_AND_ERRNO " " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, COLOR_BG_GREEN " I " COLOR_NORMAL LOG_SOURCE " " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#else /* LOG_COLOR */
#ifdef NDEBUG
#define log_debug(M, ...)
#else
#define log_debug(M, ...) fprintf(stderr, "[D] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#define log_err(M, ...) fprintf(stderr, "[E] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "[W] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[I] (%s:%d) " M "\n" COLOR_NORMAL, __FILE__, __LINE__, ##__VA_ARGS__)
#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#endif /* LOG_COLOR */

#endif /* LOG_H_ */

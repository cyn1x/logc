#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __clang__
#include <stddef.h>
#endif

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

typedef void (*log_custom_t)(bool lock);
typedef void (*log_lock_t)(bool lock);

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_init(log_level_t level, FILE *output, FILE *fp);
void log_log(log_level_t level, const char *file, int line, const char *format,
             ...);
void log_set_lock(log_lock_t lock_fn);
void log_set_level(log_level_t level);

#endif // LOG_H

#include "log.h"
#include <stdarg.h>
#include <time.h>

#pragma pack(4)
typedef struct log_event
{
    va_list     args;
    struct tm   datetime;
    log_level_t level;
    const char *fmt;
    const char *file;
    int         line;
} log_event_t;

static log_level_t CURRENT_LOG_LEVEL;
static FILE       *LOG_OUTPUT_STREAM;
static FILE       *LOG_OUTPUT_FILE;

static const char *level_strings[] = {"TRACE", "DEBUG", "INFO",
                                      "WARN",  "ERROR", "FATAL"};

static const char *level_colors[]  = {"\x1b[94m", "\x1b[36m", "\x1b[32m",
                                      "\x1b[33m", "\x1b[31m", "\x1b[35m"};

static void        output_terminal(log_event_t *event);
static void        output_file(log_event_t *event);
static void        lock(void);
static void        unlock(void);
static struct tm   get_datetime(void);

log_lock_t         lock_fn;

void               log_init(log_level_t level, FILE *output, FILE *fp)
{
    CURRENT_LOG_LEVEL = level;
    LOG_OUTPUT_STREAM = output ? output : stderr;
    LOG_OUTPUT_FILE   = fp;
}

void log_log(log_level_t level, const char *file, int line, const char *fmt,
             ...)
{
    if (level < CURRENT_LOG_LEVEL || !LOG_OUTPUT_STREAM) {
        return;
    }

    log_event_t event = {.datetime = get_datetime(),
                         .level    = level,
                         .fmt      = fmt,
                         .line     = line,
                         .file     = file};

    lock();

    if (LOG_OUTPUT_STREAM) {
        va_start(event.args, fmt);
        output_terminal(&event);
        va_end(event.args);
    }

    if (LOG_OUTPUT_FILE) {
        va_start(event.args, fmt);
        output_file(&event);
        va_end(event.args);
    }

    unlock();
}

static void output_terminal(log_event_t *event)
{
    char t_buf[32];
    strftime(t_buf, 32, "%T", &event->datetime);

    fprintf(LOG_OUTPUT_STREAM, "[%s] %s%-5s \x1b[90m%s:%d:\x1b[0m ", t_buf,
            level_colors[event->level], level_strings[event->level],
            event->file, event->line);

    vfprintf(LOG_OUTPUT_STREAM, event->fmt, event->args);
    fprintf(LOG_OUTPUT_STREAM, "\n");
    fflush(LOG_OUTPUT_STREAM);
}

static void output_file(log_event_t *event)
{
    char t_buf[32];
    strftime(t_buf, 32, "%F %T", &event->datetime);

    fprintf(LOG_OUTPUT_FILE, "[%s] %-5s %s:%d: ", t_buf,
            level_strings[event->level], event->file, event->line);

    vfprintf(LOG_OUTPUT_FILE, event->fmt, event->args);
    fprintf(LOG_OUTPUT_FILE, "\r\n");
    fflush(LOG_OUTPUT_FILE);
}

static struct tm get_datetime(void)
{
    time_t t;
    time(&t);
    struct tm dt;

#ifdef __linux__
    dt = *localtime(&t);
#else
    _localtime64_s(&dt, &t);
#endif

    return dt;
}

static void lock(void)
{
    if (lock_fn) {
        lock_fn(true);
    }
}

static void unlock(void)
{
    if (lock_fn) {
        lock_fn(false);
    }
}

void log_set_lock(log_lock_t fn) { lock_fn = fn; }

void log_set_level(log_level_t level) { CURRENT_LOG_LEVEL = level; }

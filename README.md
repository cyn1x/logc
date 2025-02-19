<div align="center">

# logc
Simple cross-platform logging library written in C with thread safety.

</div>

![Preview](https://i.imgur.com/mucD4cC.png)

## Usage

Either clone the repository or manually download into your project for compilation with your current program. The first call to the translation unit should be `log_init()` with the log level and output stream as a bare minimum.

### Basic Usage

```c
#include "log.h"

int  main(int argc, char *argv[])
{
    log_init(LOG_TRACE, stdout, NULL);
    
    log_trace("This is a trace log.");
    log_debug("This is a debug log.");
    log_info("Starting the program...");
    log_warn("This is a warning.");
    log_error("An error occurred: %s", "Something went wrong");
    log_fatal("Fatal error: %d", 123);

    return 0;
}
```

The example below shows how to make use of the locking mechanism when developing multithreaded applications as well as passing in a `FILE` pointer to an open file.

### Advanced Usage

```c
#include "log.h"

#ifdef _WIN32
void win_lock(bool lock)
{
    if (lock)
        WaitForSingleObject(hRunMutex, INFINITE);
    else
        ReleaseMutex(hRunMutex);
}
#else
void posix_lock(bool lock)
{
    if (lock)
        pthread_mutex_lock(&log_mutex);
    else
        pthread_mutex_unlock(&log_mutex);
}
#endif

int  main(int argc, char *argv[])
{
    FILE *fp;

    // Open log file in the project root directory and set the lock function pointer
#ifdef __linux__
    fp = fopen("log.txt", "w");
    log_set_lock(posix_lock);
#else
    fopen_s(&fp, "log.txt", "w");
    log_set_lock(win_lock);
#endif

    log_init(LOG_TRACE, stdout, fp);

    app_run();

    return 0;
}
```

### Changing Configurations at Runtime

The log level can be dynamically set at runtime 

```c
log_set_level(LOG_WARN);
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://github.com/cyn1x/logc/blob/master/LICENSE)

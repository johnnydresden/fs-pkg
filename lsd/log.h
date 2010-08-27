#if	!defined(ap_log_h)
#define	ap_log_h
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdarg.h>

class Logger {
  private:
     FILE *fp;
     char *Path;

  public:
     Logger(const char *path);
     ~Logger();
     int Write(int level, const char *fmt, ...);
     int Write(int level, const char *fmt, va_list ap);
};

extern Logger *Log;

#endif	// !defined(ap_log_h)

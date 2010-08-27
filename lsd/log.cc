#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <lsd/conf.h>
#include <lsd/log.h>

Logger::Logger(const char *path) {
   Path = g_strdup(path);

   if (Path[0] == ':') {
      if (strcmp(Path, ":stderr:") == 0)
         fp = stderr;
      else if (strcmp(Path, ":stdout:") == 0)
         fp = stdout;
   } else if ((fp = fopen(Path, "a+")) == NULL) {
         fprintf(stderr, "Logger: failed to start, logging to stderr. Reason: [%d] %s\n", errno, strerror(errno));

         if (Path)
            g_free(Path);

         Path = g_strdup(":stderr:");
         fp = stderr;
   }
}

Logger::~Logger() {
   if (Path)
      g_free(Path);

   if (fp && fp != stderr && fp != stdout)
      fclose(fp);
}

// The main code is stdarg-based, for flexibility
int Logger::Write(int priority, const char *fmt, va_list ap) {
    char buf[2048];
    char ts_buf[128];
    const char *level = NULL;
    const time_t now = (conf ? conf->now : time(NULL));
    struct tm *now_tm = localtime(&now);

    if (conf && priority < (const int)conf->Get("core.log.level", (const int)LOG_WARNING))
       return ENODEV;

#define	__CASE(val, label)	case val: level = (const char *)#label; break;
  switch(priority) {
     __CASE(LOG_ALERT, alert);
     __CASE(LOG_CRIT, critical);
     __CASE(LOG_DEBUG, debug);
     __CASE(LOG_EMERG, emergency);
     __CASE(LOG_ERR, error);
     __CASE(LOG_INFO, info);
     __CASE(LOG_NOTICE, notice);
     __CASE(LOG_WARNING, warning);

     default:
        level = (const char *)"unknown";
        break;
  }
#undef	__CASE

    // assume we're using syslog if no log fp 
    if (fp) {
       strftime(ts_buf, sizeof(ts_buf) - 1, "%Y/%m/%d@%H:%M:%S", now_tm);
       vsnprintf(buf, sizeof(buf), fmt, ap);
       fprintf(fp, "%s", ts_buf);
       fprintf(fp, " %s\n", buf);
    } else {
       vsyslog(priority, fmt, ap);
    }

    return 0;
}

int Logger::Write(int priority, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

    // Call our varargs version
    Write(priority, fmt, ap);
    va_end(ap);
}

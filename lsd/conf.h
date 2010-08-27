#if	!defined(lsd_conf_h)
#define	lsd_conf_h
#include <lsd/dict.h>
#include <time.h>
#include <glib.h>
#define	CONF_BUFSIZE	256

class Conf {
   private:
       Dict<gchar>  *dict;
       bool        dconf_line(const gchar *section, gchar *line);

   public:
       // this really shouldnt all be public...
       FILE       *log_fp;
       int         log_level;
       gchar       *logfile;
       int         argc;
       gchar       *argv[6];
       int         dying;
       time_t      born;
       time_t      now;

       // interface
       Conf(const gchar *file);
       ~Conf();
       bool Get(const gchar *key, bool def);
       double Get(const gchar *key, double def);
       int Get(const gchar *key, int def);
       gchar *Get(const gchar *key, const gchar *def);
       time_t Get(const gchar *key, time_t def);
       int Set(const gchar *key, const gchar *val);
       void Unset(const gchar *key);
       bool line_dconf(const gchar *section, gchar *line);
       bool line_module(const gchar *section, gchar *line);
       bool Load(const gchar *file);
};

struct config_list {
   struct config_list *next;
   enum { T_INT = 0, T_STRING, T_BOOL } type;
   void       *data;
};
typedef struct config_list conf_list_t;

extern	Conf *conf;

#endif	// !defined(lsd_conf_h)

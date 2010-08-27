#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <lsd/dict.h>
#include <lsd/conf.h>
#include <lsd/time.h>
#include <lsd/log.h>
#include <lsd/module.h>
#include <lsd/stringhelper.h>

// XXX:These belong in dict.cc object ;)
bool Conf::Get(const gchar *key, bool def) {
   gchar       *tmp;
   int         rv = 0;

   tmp = (gchar *)dict->Get(key, "unset");

   if (strcmp(tmp, "unset") == 0)
      return def;
   else if (strcasecmp(tmp, "true") == 0 || strcasecmp(tmp, "on") == 0 ||
            strcasecmp(tmp, "yes") == 0 || (int)strtol(tmp, NULL, 0) == 1)
      rv = 1;
   else if (strcasecmp(tmp, "false") == 0 || strcasecmp(tmp, "off") == 0 ||
            strcasecmp(tmp, "no") == 0 || (int)strtol(tmp, NULL, 0) == 0)
      rv = 0;

   return rv;
}

double Conf::Get(const gchar *key, const double def) {
   gchar       *tmp;

   tmp = (gchar *)dict->Get(key, "unset");

   if (strcmp(tmp, "unset") == 0)
      return def;

   return atof(tmp);
}

int Conf::Get(const gchar *key, const int def) {
   gchar       *tmp;

   tmp = (gchar *)dict->Get(key, "unset");
   if (strcmp(tmp, "unset") == 0)
      return def;

   return (int)strtol(tmp, NULL, 0);
}

gchar       *Conf::Get(const gchar *key, const gchar *def) {
   if (dict == NULL || key == NULL)
      return NULL;

   return dict->Get(key, def);
}

time_t Conf::Get(const gchar *key, const time_t def) {
      return (dhms_to_sec((gchar *)dict->Get(key, "0"), def));
}

int Conf::Set(const gchar *key, const gchar *val) {
   return dict->Set(key, val);
}

void Conf::Unset(const gchar *key) {
   dict->Unset(key);
}

Conf::Conf(const gchar *file) {
   dict = new Dict<gchar>(0);
   born = now = time(NULL);
   dying = false;
}

bool Conf::line_dconf(const gchar *section, gchar *line) {
   gchar *val = NULL,
        *key = line;

   // seperated by a space
   if ((val = strchr(line, ' ')) != NULL) {
      *val = '\0';
      val++;
   } else if ((val = strchr(line, '\t')) != NULL) {
      *val = '\0';
      val++;
   }

   val = ax_strunquote(val);
   Log->Write(LOG_DEBUG, "config.load: [%s] SET %s = %s", section, key, val);
   Set((const gchar *)key, (const gchar *)val);

   return false;
}

bool Conf::line_module(const gchar *section, gchar *line) {
  gchar path[PATH_MAX];

  memset(path, 0, sizeof(path));
  snprintf(path, sizeof(path), "%s/%s", Get("path.modules", "mods"), line);

//  if (!AX_Module_Load(path))
     Log->Write(LOG_INFO, "module.load %s", path);
//  else
//     Log->Write(LOG_WARNING, "module.load: failed loading %s [%d:%s]", path, errno, strerror(errno));
  return false;
}

bool Conf::Load(const gchar *file) {
   FILE       *fp;
   gchar       *p;
   gchar        buf[512];
   gchar        section[128];
   int         in_comment = 0;
   int         line = 0;


   if (!(fp = fopen(file, "r"))) {
      fprintf(stderr, "unable to open config file %s, bailing\n", file);
      exit(EXIT_FAILURE);
   }

   while (!feof(fp)) {
      line++;
      memset(buf, 0, 512);

      // we dont care...
      if (!fgets(buf, sizeof(buf), fp))
         continue;

      if (buf == NULL)
         continue;

      p = ax_strdewhitespace(buf);
      ax_strstrip(p);

      // did we eat the whole line?
      if (!p || strlen(p) == 0)
         continue;

      // fairly flexible comment handling
      if (in_comment && p[0] == '*' && p[1] == '/') {
         in_comment = 0;               // end of block comment
         continue;
      } else if (p[0] == ';' || p[0] == '#' || (p[0] == '/' && p[1] == '/')) { 
         continue;                     // line comment
      } else if (p[0] == '/' && p[1] == '*') {
         in_comment = 1;               // start of block comment
         continue;
      } else if (in_comment)
         continue;                     // ignored line, in block comment
      /////////////////////
      // Handle Sections //
      /////////////////////
      if (p[0] == '[' && p[strlen(p) - 1] == ']') {
         memset(section, 0, sizeof(section));
         memcpy(section, p + 1, strlen(p) - 2);
         continue;
      }

      if (strncmp(section, "dconf", strlen(section)) == 0)
         line_dconf(section, p);
      else if (strncmp(section, "module", strlen(section)) == 0)
         line_module(section, p);
   }

   fclose(fp);
   return false;
}

Conf::~Conf(void) {
   delete dict;
}

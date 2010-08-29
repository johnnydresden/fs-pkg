#define	StatTmpl \
   struct stat sb; \
\
   if (stat(path, &sb)) \
      return false;

static inline bool is_dir(const gchar *path) { StatTmpl; return S_ISDIR(sb.st_mode); }
static inline bool is_readable(const gchar *path) { return access(path, R_OK); }
static inline bool is_writable(const gchar *path) { return access(path, W_OK); }
static inline bool is_executable(const gchar *path) { return access(path, X_OK); }

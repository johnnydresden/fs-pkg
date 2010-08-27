#if	!defined(ap_module_h)
#define	ap_module_h
#include <glib.h>
#include <gmodule.h>

/* public interface for modules */
typedef struct module_info {
   gchar       *name;                // module name
   gchar       *author;              // module author
   gchar       *copyright;           // copyright/license
   u_int32_t   version;              // version encoded with MODULE_VERSION macro
   gchar      *type;                 // what sort of mod is this?
   // APMessageList *msgs;           // Table of supported messages and functions to call
} module_info_t;

class Module {
   private:
      module_info_t *mod_info;		// Returned by dlsym(__modinfo__)
      gchar *path;			// Module path
      GModule *mod_ptr;			// GModule pointer
      int refcnt;
   public:
      Module(const char *name);		// Load a module
      ~Module();			// Unload a module
      Dict<gpointer> *sym_cache;
//      APMessagehandler;		// Message handler
};

#endif                                 /* !defined(module_h) */

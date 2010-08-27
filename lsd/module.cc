#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <lsd/conf.h>
#include <lsd/log.h>
#include <lsd/module.h>
#include <lsd/stringhelper.h>

// Should be in G (globals)
GSList *modules = NULL;		 			// Modules single-linked list 
GStaticMutex modules_mutex = G_STATIC_MUTEX_INIT;	// lock on the modules list

Module::Module(const char *name) {
   int rv, spin_count = 0;
   gchar *tmp = NULL;

   // Configuration file isnt fully loaded and as such load from
   // compiled-in default
   if (!conf->born)
      tmp = g_module_build_path(PLUGINDIR, path);
   else
      tmp = g_module_build_path(conf->Get("path.modules", "/opt/apxd/modules"), path);

   if ((mod_ptr = g_module_open(path, G_MODULE_BIND_LAZY))) {
      if (tmp)
         g_free(tmp);

      Log->Write(LOG_WARNING, "failed to load module %s:%d", path, g_module_error());
      throw g_module_error();
   }

   if (tmp)
      g_free(tmp);

   if (!g_module_symbol(mod_ptr, "__modinfo__", (gpointer *)&mod_info)) {
      g_module_close(mod_ptr);

      Log->Write(LOG_WARNING, "failed to find __modinfo__ in %s", path);
      throw EINVAL;
   }

   sym_cache = new Dict<gpointer>(0);
   path = g_strdup(path);
   g_static_mutex_lock(&modules_mutex);
   modules = g_slist_prepend(modules, this);
   g_static_mutex_unlock(&modules_mutex);

   Log->Write(LOG_INFO, "module.load: Loaded %s (%s) succesfully", name, path);
}

Module::~Module() {
   int         rv, spin_count = 0;
   gboolean force = false;

   if (refcnt > 0)
      Log->Write(LOG_WARNING, "core.module.unload for %s with refcnt>0 (%d)", path, refcnt);

   g_module_close(mod_ptr);

   // Locked access to the list prevents threads from simultaneously modifying
   // the core module list.
   // XXX: Big design change needed here--
   // XXX: We need to create a copy (COW if possible) of the module
   // XXX: system within each thread, so that we can avoid the
   // XXX: overhead of cloning un-touched modules.
   // XXX: This will improve security with a reduced impact
   // XXX: on runtime performance.
   g_static_mutex_lock(&modules_mutex);
   modules = g_slist_remove_all(modules, this);
   g_static_mutex_unlock(&modules_mutex);

   Log->Write(LOG_INFO, "module.unload: Unloaded %s (%s) successfully", mod_info->name, path);
}

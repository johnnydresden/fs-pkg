#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lsd/conf.h>
#include <lsd/daemon.h>
#include <lsd/dict.h>
#include <lsd/signal.h>
#include <lsd/log.h>
#include <lsd/access.h>
#include <glib.h>
#include <ev.h>

Conf *_G_Config;
Logger *Log;
struct ev_loop *event_loop = NULL;

// start up various subsystems and run the event loop
int main(int argc, char **argv) {
    setup_corefile();
#if	defined(DEBUG_ALLOC)
    g_mem_set_vtable(glib_mem_profiler_table);
#endif	// defined(DEBUG_ALLOC)
    g_thread_init(NULL);

    _G_Config = new Conf();
    _G_Config->Set("core.log.level", 0);	// Set inital logging to all
    Log = new Logger(_G_Config->Get("path.log", ":syslog:"));

    if (!is_dir("~/.appworx")) {
       Conf *defaults;

       mkdir("~/.appworx", 0700);
       defaults = new Conf();

       if (is_readable("/etc/appworx/master.cf")) {
          Log->Write(LOG_NOTICE, "Trying to copy /etc/appworx/masster.cf to ~/.appworx/");
          system("cp /etc/appworx/example.cf ~/.appworx");
       }
    }

    // try per-user then system-wide _G_Configuration files
    if (!_G_Config->Load("~/.appworx/master.cf")) {
       Log->Write(LOG_NOTICE, "No local configuration found (~/.appworx/master.c) trying system-wide");

       if (!_G_Config->Load("/etc/appworx/master.cf")) {
          Log->Write(LOG_ERR, "No master.cf in ~/.appworx/master.cf nor /etc/apporx/master.cf, bailing!");
          exit(EXIT_FAILURE);
       }
    }

    priv_drop();
    daemonize();
    pidfile_write();
    g_assert((event_loop = ev_default_loop(EVFLAG_AUTO)));
    signal_init(event_loop);

    while (!_G_Config->dying) {
       ev_loop(event_loop, 0);
    }

    Log->Write(LOG_NOTICE, "Shutting down");
    pidfile_remove();
    return EXIT_SUCCESS;
}
 
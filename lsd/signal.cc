#include <ev.h>
#include <lsd/signal.h>
#include <lsd/conf.h>
#include <lsd/daemon.h>
#include <lsd/log.h>
ev_signal   signal_die;
ev_signal   signal_reload;
ev_signal   signal_stop;
ev_signal   signal_cont;

extern struct ev_loop *event_loop;	// from main.c
static void signal_handler_die(struct ev_loop *loop, ev_signal *w, int revents) {
   Log->Write(LOG_INFO, "core.exit.signal: caught SIGINT|SIGTERM");
//   Event->Send("core.exit.signal");
   conf->dying = 1;
   ev_unloop(event_loop, EVUNLOOP_ALL);
   pidfile_remove();
}

static void signal_handler_reload(struct ev_loop *loop, ev_signal *w, int revents) {
   Log->Write(LOG_INFO, "core.config.reload: caught SIGHUP");
//   Event->Send("core.config.reload");
// XXX:   conf->Reload();
}

static void signal_handler_stop(struct ev_loop *loop, ev_signal *w, int revents) {
   Log->Write(LOG_INFO, "core.event.suspend: SIGSTOP|SIGTSTP");
//   Event->Send("core.event.suspend");
   ev_suspend(event_loop);
}

static void signal_handler_cont(struct ev_loop *loop, ev_signal *w, int revents) {
   ev_resume(event_loop);
   Log->Write(LOG_INFO, "core.event.resume: SIGCONT");
//   Event->Send("core.event.resume");
}

// Map POSIX signals to core events
// XXX: Must we call ev_signal_start each time?
void signal_init(void) {
   // core.exit.signal: SIGINT|SIGTERM
   ev_signal_init(&signal_die, signal_handler_die, SIGINT);
   ev_signal_start(event_loop, &signal_die);
   ev_signal_init(&signal_die, signal_handler_die, SIGTERM);
   ev_signal_start(event_loop, &signal_die);

   // core.conf.reload: SIGHUP
   ev_signal_init(&signal_reload, signal_handler_reload, SIGHUP);
   ev_signal_start(event_loop, &signal_reload);

   // core.event.suspend: SIGSTOP|SIGTSTP
   ev_signal_init(&signal_stop, signal_handler_stop, SIGSTOP);
   ev_signal_start(event_loop, &signal_stop);
   ev_signal_init(&signal_stop, signal_handler_stop, SIGTSTP);
   ev_signal_start(event_loop, &signal_stop);

   // core.event.resume: SIGCONT
   ev_signal_init(&signal_cont, signal_handler_cont, SIGCONT);
   ev_signal_start(event_loop, &signal_cont);
   signal(SIGPIPE, SIG_IGN);
}

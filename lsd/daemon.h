#if	!defined(lsd_daemon_h)
#define	lsd_daemon_h

extern int  daemonize(void);
extern int  priv_drop(void);
extern void pidfile_remove(void);
extern void pidfile_write(void);
extern void setup_corefile(void);

#endif	// !defined(lsd_daemon_h)

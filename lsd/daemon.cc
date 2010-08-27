#define	BSD_SOURCE
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <lsd/conf.h>
#include <lsd/log.h>

int priv_drop(void) {
   char       *s;
   struct group *grp;
   struct passwd *pwd;

   /*
    * Set umask to go-rwx 
    */
   umask(0077);

   /*
    * Only needed if started by root (and very important in this case) 
    */
   if (getuid() == 0 || getgid() == 0) {
      if (strcmp("unset", (s = conf->Get("path.croot", "unset"))) != 0) {
            if (chroot(s) != 0) {
               Log->Write(LOG_ERR, "chroot failed:%d:%s", errno, strerror(errno));
               conf->dying = 1;
               raise(SIGABRT);
            }

            if (chdir("/") != 0) {
               Log->Write(LOG_ERR, "chdir(/) failed:%d:%s", errno, strerror(errno));
               conf->dying = 1;
               raise(SIGABRT);
            }
      }

      /*
       * Drop groups 
       */
      s = conf->Get("privileges.group", "unset");
      if (strcmp(s, "unset") != 0)
         if ((grp = getgrnam(s)) != NULL)
            setregid(grp->gr_gid, grp->gr_gid);

      /*
       * Drop user privilege 
       */
      s = conf->Get("privileges.user", "unset");
      if (strcmp(s, "unset") != 0)
         if ((pwd = getpwnam(s)) != NULL)
            setreuid(pwd->pw_uid, pwd->pw_uid);

      return 1;
   }

   return 0;
}

void pidfile_remove(void) {
   unlink(conf->Get("path.pidfile", "apxd.pid"));
}

void pidfile_write(void) {
   FILE       *fp;

   if (!(fp = fopen(conf->Get("path.pidfile", "apxd.pid"), "w"))) {
      perror("pidfile_write: fopen");
      exit(EXIT_FAILURE);
   }

   fprintf(fp, "%d\n", getpid());
   fclose(fp);
   atexit(pidfile_remove);
}

int daemonize(void) {
   pid_t       pid;

   if (conf->Get("core.background", false) == 0) {
      Log->Write(LOG_INFO, "config:core.background == false, not detaching");
      return 0;
   }

   if ((pid = fork()) != 0) {
      if (pid == -1) {
         Log->Write(LOG_ERR, "daemonize:fork():%d:%s", errno, strerror(errno));
         exit(EXIT_FAILURE);
      } else {
         Log->Write(LOG_INFO, "forked into background pid=%d", pid);
         exit(EXIT_SUCCESS);
      }
   }

   return -1;
}

void setup_corefile(void) {
   struct rlimit rlim;                 /* resource limits */

   /*
    * Set corefilesize to maximum 
    */
   if (!getrlimit(RLIMIT_CORE, &rlim)) {
      rlim.rlim_cur = rlim.rlim_max;
      setrlimit(RLIMIT_CORE, &rlim);
   }
}

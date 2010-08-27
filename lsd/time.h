#if	!defined(lsd_time_h)
#define	lsd_time_h
#include <time.h>
extern time_t dhms_to_sec(const char *str, const time_t def);
extern char *sec_to_dhms(time_t itime);

#endif	// !defined(lsd_time_h)


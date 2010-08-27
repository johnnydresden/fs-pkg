#if	!defined(ap_stringhelper_h)
#define ap_stringhelper_h

#define MAXPARA	15                   /* The maximum number of parameters in an IRC message */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

extern char *ax_strctime(time_t);
extern int  ax_strtokenize(char *, char **);
extern int  ax_strtokenize_generic(char, int, char *, char **);
extern void ax_strexplode(char *, int, int, int, char **);
extern int  ax_strislower(char *);
extern void ax_strstrip(char *);
extern void ax_strtohex(char *dst, const char *src, unsigned int length);
extern char *ax_strunquote(char *str);
extern char *ax_strdewhitespace(const char *str);
extern char *ax_strtolower(char *s);

#ifndef HAVE_STRLCAT
size_t      strlcat(char *, const char *, size_t);
#endif

#ifndef HAVE_STRLCPY
size_t      strlcpy(char *, const char *, size_t);
#endif

#endif	// !defined(ap_stringhelper_h)

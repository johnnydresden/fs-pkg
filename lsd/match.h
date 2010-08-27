/*
 * Copyright (c) 2003-2004 E. Will et al.
 * Copyright (c) 2005-2006 Atheme Development Group
 * Rights to this code are as defined in doc/LICENSE.
 *
 * String matching
 *
 * $Id: match.h 7779 2007-03-03 13:55:42Z pippijn $
 */

#if	!defined(ap_match_h)
#define ap_match_h

#include <regex.h>
/* cidr.c */
extern int  nn2_match_ips(const char *mask, const char *address);
extern int  nn2_match_cidr(const char *mask, const char *address);

/* match.c */
#define MATCH_RFC1459   0
#define MATCH_ASCII     1

extern int  match_mapping;

#define IsLower(c)  ((unsigned char)(c) > 0x5f)
#define IsUpper(c)  ((unsigned char)(c) < 0x60)

#define C_ALPHA 0x00000001
#define C_DIGIT 0x00000002

extern const unsigned int charattrs[];

#define IsAlpha(c)      (charattrs[(unsigned char) (c)] & C_ALPHA)
#define IsDigit(c)      (charattrs[(unsigned char) (c)] & C_DIGIT)
#define IsAlphaNum(c)   (IsAlpha((c)) || IsDigit((c)))
#define IsNon(c)        (!IsAlphaNum((c)))

extern const unsigned char ToLowerTab[];
extern const unsigned char ToUpperTab[];

void        set_match_mapping(int);

extern int  ToLower(int);
extern int  ToUpper(int);

extern int  irccasecmp(const char *, const char *);
extern int  ircncasecmp(const char *, const char *, int);

extern int  match(const char *, const char *);
extern char *collapse(char *);

/* regex_create() flags */
#define AREGEX_ICASE	1                 /* case insensitive */

extern regex_t *regex_create(char *pattern, int flags);
extern char *regex_extract(char *pattern, char **pend, int *pflags);
extern int  regex_match(regex_t * preg, char *string);
extern int  regex_destroy(regex_t * preg);

#define EmptyString(x) ((x == NULL) || (*(x) == '\0'))

#endif	// !defined(ap_match_h)

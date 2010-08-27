#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <syslog.h>
#include <lsd/match.h>
#include <lsd/stringhelper.h>
#include <lsd/log.h>

/*
 * strlcat and strlcpy extracted from ircd-hybrid, originally taken
 * from the FreeBSD source.  This copy fixes a bug.
 *
 * They had the following Copyright info:
 *
 *
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t siz) {
   char       *d = dst;

   const char *s = src;

   size_t      n = siz, dlen;

   while (n-- != 0 && *d != '\0')
      d++;
   dlen = d - dst;
   n = siz - dlen;

   if (n == 0)
      return (dlen + strlen(s));
   while (*s != '\0') {
      if (n != 1) {
         *d++ = *s;
         n--;
      }
      s++;
   }
   *d = '\0';
   return (dlen + (s - src));          /* count does not include NUL */
}
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t siz) {
   char       *d = dst;

   const char *s = src;

   size_t      n = siz;

   /*
    * Copy as many bytes as will fit 
    */
   if (n != 0 && --n != 0) {
      do {
         if ((*d++ = *s++) == 0)
            break;
      }
      while (--n != 0);
   }
   /*
    * Not enough room in dst, add NUL and traverse rest of src 
    */
   if (n == 0) {
      if (siz != 0)
         *d = '\0';                    /* NUL-terminate dst */
      while (*s++) ;
   }

   return (s - src - 1);               /* count does not include NUL */
}
#endif

void ax_strtohex(char *dst, const char *src, unsigned int length) {
   char        hex[17] = "0123456789abcdef";

   unsigned int i;

   for (i = 0; i < length; i++) {
      dst[2 * i] = hex[(src[i] & 0xf0) >> 4];
      dst[2 * i + 1] = hex[(src[i] & 0x0f)];
   }

   dst[2 * i] = 0;
}

/* Sentinel - IRC Statistical and Operator Services
** s_string.c - String manipulation functions
**
** Copyright W. Campbell and others.  See README for more details
** Some code Copyright: Jonathan George, Kai Seidler, ircd-hybrid Team,
**                      IRCnet IRCD developers.
**
** $Id: s_string.c,v 1.4 2003/11/06 20:41:53 wcampbel Exp $
*/

/* Our own ctime function */
/* Modified for l10n - 12 f\xe9v 2002 -Hwy */
char       *ax_strctime(time_t stuff) {
   static char buf[32];
   struct tm  *ltm = localtime(&stuff);

   strftime(buf, 32, "%c", ltm);
   return buf;
}

/* tokenize splits apart a message (in our case with the origin and command
** picked off already...
*/
int ax_strtokenize(char *message, char **parv) {
   char       *pos = NULL;

   char       *next;

   int         count = 0;

   if (!message) {
      /*
       * Something is seriously wrong...bail 
       */
      return -1;
   }

   /*
    * First we find out if there's a : in the message, save that string
    * ** somewhere so we can set it to the last param in parv
    * ** Also make sure there's a space before it...if not, then we're
    * ** screwed
    */
   pos = message;
   while (TRUE) {
      if ((pos = strchr(pos, ':'))) {
         pos--;
         if (*pos != ' ') {
            /*
             * There SHOULDN'T be a problem with this... 
             */
            pos += 2;
            continue;
         }
         *pos = '\0';
         /*
          * This is BAD - get rid of the space before the : 
          */
         pos++;
         *pos = '\0';
         /*
          * VERY BAD...over now though 
          */
         pos++;
         break;
      } else {
         break;
      }
   }

   /*
    * Now we take the beginning of message and find all the spaces...
    * ** set them to \0 and use 'next' to go through the string
    */

   next = message;
   parv[0] = message;
   count = 1;

   while (*next) {
      if (count == MAXPARA - 1) {
         /*
          * We've reached one less than our max limit
          * ** to handle the parameter we already ripped off
          */
         Log->Write(LOG_DEBUG, "DEBUG: Reached the MAXPARA limit!");
         return count;
      }
      if (*next == ' ') {
         *next = '\0';
         next++;
         /*
          * Eat any additional spaces 
          */
         while (*next == ' ')
            next++;
         /*
          * If it's the end of the string, it's simply
          * ** an extra space before the :parameter.  Here we
          * ** break.
          */
         if (*next == '\0')
            break;
         parv[count] = next;
         count++;
      } else {
         next++;
      }
   }

   if (pos) {
      parv[count] = pos;
      count++;
   }

   return count;
}

/* A generic tokenizer, tokenizes based on a specified character */
int ax_strtokenize_generic(char delim, int size, char *message, char **parv) {
   char       *next;

   int         count;

   if (!message) {
      /*
       * Something is seriously wrong...bail 
       */
      parv[0] = NULL;
      return 0;
   }

   /*
    * Now we take the beginning of message and find all the spaces...
    * ** set them to \0 and use 'next' to go through the string
    */
   next = message;
   parv[0] = next;
   count = 1;

   while (*next) {
      /*
       * This is fine here, since we don't have a :delimited
       * ** parameter like tokenize
       */
      if (count == size) {
         /*
          * We've reached our limit 
          */
         Log->Write(LOG_DEBUG, "Reached the size limit!");
         return count;
      }
      if (*next == delim) {
         *next = '\0';
         next++;
         /*
          * Eat any additional delimiters 
          */
         while (*next == delim)
            next++;
         /*
          * If it's the end of the string, it's simply
          * ** an extra space at the end.  Here we break.
          */
         if (*next == '\0')
            break;
         parv[count] = next;
         count++;
      } else {
         next++;
      }
   }

   return count;
}

void ax_strexplode(char *dest, int max, int initial, int parc, char *parv[]) {
   int         i;

   if (parv[initial])
      strlcpy(dest, parv[initial], max);
   for (i = initial + 1; i < parc; i++) {
      strlcat(dest, " ", max);
      strlcat(dest, parv[i], max);
   }
}

char       *ax_strtolower(char *s) {
   char       *t;

   for (t = s; *t != '\0'; t++)
      *t = ToLower(*t);

   return s;
}

/* Return 1 if the string s is all lower case, return 0 if not */
int stris_lower(char *s) {
   char       *t;

   if (s == NULL)
      return 1;
   for (t = s; *t != '\0'; t++) {
      if (islower((int)*t) == 0) {
         return 0;
      }
   }
   return 1;
}

/* Remove CR and LF from the line input. */
void ax_strstrip(char *line) {
   char       *c;

   if ((c = strrchr(line, '\r'))) {
      /*
       * If we receive a CR, then we can just terminate the
       * ** call here
       */
      *c = '\0';
      return;
   }

   if ((c = strrchr(line, '\n'))) {
      *c = '\0';
      return;
   }
}

char       *ax_strunquote(char *str) {
   char       *tmp;

   if (str && *str == '"')
      str++;
   if ((tmp = index(str, '"')))
      *tmp = '\0';

   return str;
}

char       *ax_strdewhitespace(const char *str) {
   char *p = (char *)str;

   /*
    * Skip over white space 
    */
   while (*p && (*p == ' ' || *p == '\t'))
      p++;
   return p;
}

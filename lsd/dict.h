   #if	!defined(ap_dict_h)
#define	ap_dict_h

/*
 * dictionary.h <N. Devillard> Sep 2007 $Revision: 1.12 $
 *	Implements a dictionary for string variables.
 * This module implements a simple dictionary object, i.e. a list
 * of string/string associations. This object is useful to store e.g.
 * informations retrieved from a configuration file (ini files).
 *
 * $Id: dictionary.h,v 1.12 2007-11-23 21:37:00 ndevilla Exp $
 * $Author: ndevilla $
 * $Date: 2007-11-23 21:37:00 $
 * $Revision: 1.12 $
 */
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#define MAXVALSZ	1024	// max size for int/double
#define DICTMINSZ	128	// minimum entries in dict
#define DICT_INVALID_KEY    ((char*)-1)	// invalid key token

template <class T>
class Dict {
  private:
     int         g_n;          /** Number of entries in dictionary */
     int         g_size;       /** Storage size */
     gchar     **g_val;        /** List of string values */
     gchar     **g_key;        /** List of string keys */
     unsigned   *g_hash;       /** List of hash values for keys */
     /*
      *   @brief    Compute the hash key for a string.
      *   @param    key     Character string to use for key.
      *   @return   1 unsigned int on at least 32 bits.
      * This hash function has been taken from an Article in Dr Dobbs Journal.
      * This is normally a collision-free function, distributing keys evenly.
      * The key is stored anyway in the struct so that collision can be avoided
      * by comparing the key itself in last resort.
      */
      unsigned    Hash(const gchar *key);

  public:
      /*
       *   @brief    Create a new dictionary object.
       *   @param    size    Optional initial size of the dictionary.
       *   @return   1 newly allocated dictionary objet.
       * Allocates a new dictionary object of given size and returns
       * it. If you do not know in advance (roughly) the number of
       * entries in the dictionary, give size=0.
       */
      Dict(int size);

      /*
       *   @brief    Delete a dictionary object
       *   @param    d   dictionary object to deallocate.
       *   @return   void
       * Deallocate a dictionary object and all memory associated to it.
       */
      ~Dict(); 

      /*
       *   @brief    Get a value from a dictionary.
       *   @param    d       dictionary object to search.
       *   @param    key     Key to look for in the dictionary.
       *   @param    def     Default value to return if key not found.
       *   @return   1 pointer to internally allocated character string.
       * This function locates a key in a dictionary and returns a pointer
       * to its value, or the passed 'def' pointer if no such key can be
       * found in dictionary. The returned character pointer points to
       * data internal to the dictionary object, you should not try to
       * free it or modify it.
       */
       gchar *Get(const gchar *key, const gchar *def);

      /*
       *   @brief    Set a value in a dictionary.
       *   @param    d       dictionary object to modify.
       *   @param    key     Key to modify or add.
       *   @param    val     Value to add.
       *   @return   int     0 if Ok, anything else otherwise
       * If the given key is found in the dictionary, the associated
       * value is replaced by the provided one. If the key cannot be
       * found in the dictionary, it is added to it.
       * It is Ok to provide a NULL value for val, but NULL values for
       * the key are considered as errors: the function will return
       * immediately in such a case.
       *
       * Notice that if you dictionary_set a variable to NULL, a call to
       * dictionary_get will return a NULL value: the variable will be
       * found, and its value (NULL) is returned. In other words,
       * setting the variable content to NULL is equivalent to
       * deleting the variable from the dictionary. It is not 
       * possible (in this implementation) to have a key in the
       * dictionary without value.
       *
       * This function returns non-zero in case of failure.
       */
       int Set(const gchar *key, const gchar *val);

/**
  @brief    Delete a key in a dictionary
  @param    d       dictionary object to modify.
  @param    key     Key to remove.
  @return   void

  This function deletes a key in a dictionary. Nothing is done if the
  key cannot be found.
 */
void        Unset(const gchar *key);

/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump
  @param    f   Opened file pointer.
  @return   void

  Dumps a dictionary onto an opened file pointer. Key pairs are printed out
  as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
  output file pointers.
 */
void        Dump(FILE * out);
};


// Doubles the allocated size associated to a pointer
// 'size' is the current allocated size. 
static void *mem_double(void *ptr, int size) {
   void       *newptr;

   newptr = g_malloc0(2 * size);

   if (newptr == NULL)
      return NULL;

   memcpy(newptr, ptr, size);
   g_free(ptr);
   return newptr;
}

// brief	Compute the hash key for a string.
// param	key		Character string to use for key.
// return	1 unsigned int on at least 32 bits.
//  This hash function has been taken from an Article in Dr Dobbs Journal.
//  This is normally a collision-free function, distributing keys evenly.
//  The key is stored anyway in the struct so that collision can be avoided
//  by comparing the key itself in last resort.
template <class T>
unsigned Dict<T>::Hash(const gchar *key) {
   int         len;

   unsigned    hash;

   int         i;

   len = strlen(key);

   for (hash = 0, i = 0; i < len; i++) {
      hash += (unsigned)key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
   }

   hash += (hash << 3);
   hash ^= (hash >> 11);
   hash += (hash << 15);
   return hash;
}

//  brief	Create a new dictionary object.
//  param	size	Optional initial size of the dictionary.
//  return	1 newly allocated dictionary objet.
//  This function allocates a new dictionary object of given size and returns
//  it. If you do not know in advance (roughly) the number of entries in the
//  dictionary, give size=0.
template <class T>
Dict<T>::Dict(int size) {
   // If no size was specified, allocate space for DICTMINSZ 
   if (size < DICTMINSZ)
      size = DICTMINSZ;

   g_size = size;
   g_val = (gchar **)g_malloc0(size * sizeof(gchar *));
   g_key = (gchar **)g_malloc0(size * sizeof(gchar *));
   g_hash = (unsigned int *)g_malloc0(size * sizeof(unsigned));
}

//  brief	Delete a dictionary object
//  param	d	dictionary object to deallocate.
//  return	void
//     Deallocate a dictionary object and all memory associated to it.
template <class T>
Dict<T>::~Dict() {
   int         i;

   for (i = 0; i < g_size; i++) {
      if (g_key[i] != NULL)
         g_free(g_key[i]);
      if (g_val[i] != NULL)
         g_free(g_val[i]);
   }

   g_free(g_val);
   g_free(g_key);
   g_free(g_hash);

   return;
}

//  brief	Get a value from a dictionary.
//  param	d		dictionary object to search.
//  param	key		Key to look for in the dictionary.
//  param    def     Default value to return if key not found.
//  return	1 pointer to internally allocated character string.
//     This function locates a key in a dictionary and returns a pointer to its
//  value, or the passed 'def' pointer if no such key can be found in
//  dictionary. The returned character pointer points to data internal to the
//  dictionary object, you should not try to free it or modify it.
template <class T>
gchar *Dict<T>::Get(const gchar *key, const gchar *def) {
   unsigned    hash;

   int         i;

   hash = Hash(key);

   for (i = 0; i < g_size; i++) {
      if (g_key[i] == NULL)
         continue;

      // Compare hash
      if (hash == g_hash[i])
         // Compare string, to avoid hash collisions 
         if (!strcmp(key, g_key[i]))
            return g_val[i];
   }
   return (gchar *)def;
}

//  brief    Set a value in a dictionary.
//  param    d       dictionary object to modify.
//  param    key     Key to modify or add.
//  param    val     Value to add.
//  return   int     0 if Ok, anything else otherwise
//     If the given key is found in the dictionary, the associated value is
//  replaced by the provided one. If the key cannot be found in the
//  dictionary, it is added to it.
//     It is Ok to provide a NULL value for val, but NULL values for the dictionary
//  or the key are considered as errors: the function will return immediately
//  in such a case.
//     Notice that if you dictionary_set a variable to NULL, a call to
//  dictionary_get will return a NULL value: the variable will be found, and
//  its value (NULL) is returned. In other words, setting the variable
//  content to NULL is equivalent to deleting the variable from the
//  dictionary. It is not possible (in this implementation) to have a key in
//  the dictionary without value.
//     This function returns non-zero in case of failure.
template <class T>
int Dict<T>::Set(const gchar *key, const gchar *val) {
   int         i;

   unsigned    hash;

   if (key == NULL)
      return -1;

   // Compute hash for this key 
   hash = Hash(key);

   // Find if value is already in dictionary 
   if (g_n > 0) {
      for (i = 0; i < g_size; i++) {
         if (g_key[i] == NULL)
            continue;

         if (hash == g_hash[i]) {     // Same hash value
            if (!strcmp(key, g_key[i])) {   // Same key
               // Found a value: modify and return 
               if (g_val[i] != NULL)
                  g_free(g_val[i]);
               g_val[i] = val ? g_strdup(val) : NULL;
               // Value has been modified: return 
               return 0;
            }
         }
      }
   }
   // Add a new value 

   // See if dictionary needs to grow 
   if (g_n == g_size) {
      // Reached maximum size: reallockate dictionary 
      g_val = (gchar **)mem_double(g_val, g_size * sizeof(gchar *));
      g_key = (gchar **)mem_double(g_key, g_size * sizeof(gchar *));
      g_hash = (unsigned int *)mem_double(g_hash, g_size * sizeof(unsigned));
      if ((g_val == NULL) || (g_key == NULL) || (g_hash == NULL)) {
         // Cannot grow dictionary 
         return -1;
      }
      // Double size 
      g_size *= 2;
   }

   // Insert key in the first empty slot 
   for (i = 0; i < g_size; i++) {
      if (g_key[i] == NULL) {
         // Add key here 
         break;
      }
   }

   // Copy key 
   g_key[i] = g_strdup(key);
   g_val[i] = val ? g_strdup(val) : NULL;
   g_hash[i] = hash;
   g_n++;
   return 0;
}

//  brief	Delete a key in a dictionary
//  param	d		dictionary object to modify.
//  param	key		Key to remove.
//  return   void
//     This function deletes a key in a dictionary. Nothing is done if the
//  key cannot be found.
template <class T>
void Dict<T>::Unset(const gchar *key) {
   unsigned    hash;
   int         i;

   if (key == NULL)
      return;

   hash = Hash(key);

   for (i = 0; i < g_size; i++) {
      if (g_key[i] == NULL)
         continue;

      // Compare hash 
      if (hash == g_hash[i])
         // Compare string, to avoid hash collisions 
         if (!strcmp(key, g_key[i]))
            // Found key 
            break;
   }

   // key not found
   if (i >= g_size)
      return;

   g_free(g_key[i]);
   g_key[i] = NULL;

   if (g_val[i] != NULL) {
      g_free(g_val[i]);
      g_val[i] = NULL;
   }

   g_hash[i] = 0;
   g_n--;
   return;
}

//  brief	Dump a dictionary to an opened file pointer.
//  param	d	Dictionary to dump
//  param	f	Opened file pointer.
//  return	void
//     Dumps a dictionary onto an opened file pointer. Key pairs are printed out
//  as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
//  output file pointers.
template <class T>
void Dict<T>::Dump(FILE * out) {
   int         i;

   if (out == NULL)
      return;

   if (g_n < 1) {
      fprintf(out, "empty dictionary\n");
      return;
   }

   for (i = 0; i < g_size; i++)
      if (g_key[i])
         fprintf(out, "%20s\t[%s]\n", g_key[i], g_val[i] ? g_val[i] : "UNDEF");

   return;
}

#endif	// !defined(ap_dict_h)

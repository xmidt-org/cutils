/* SPDX-FileCopyrightText: 2020 Neil Henning, et. al */
/* SPDX-License-Identifier: Unlicense */

/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/hashmap.h
*/

/*
   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>
*/
#ifndef SHEREDOM_HASHMAP_H_INCLUDED
#define SHEREDOM_HASHMAP_H_INCLUDED

#include <stdlib.h>

/* We need to keep keys and values. */
struct hashmap_element {
    const char *key;
    size_t key_len;
    int in_use;
    void *data;
};

/* A hashmap has some maximum size and current size, as well as the data to
 * hold. */
typedef struct {
    size_t table_size;
    size_t size;
    struct hashmap_element *data;
} hashmap_t;


/**
 *  Create a hashmap.
 *
 *  Optional if the hashmap_t object is set to zero.
 *
 *  @param initial_size The initial size of the hashmap.
 *  @param out_hashmap  The storage for the created hashmap.
 *
 *  @return On success 0 is returned.
 *          -1 is returned if an input is invalid
 *          -2 is returned if there was a memory failure
 */
int hashmap_create(size_t initial_size, hashmap_t *const out_hashmap);


/**
 *  Put an element into the hashmap.
 *
 *  @note: The key string slice is not copied when creating the hashmap entry,
 *         and thus must remain a valid pointer until the hashmap entry is
 *         removed or the hashmap is destroyed.
 *
 *  @param hashmap The hashmap to insert into.
 *  @param key     The string key to use.
 *  @param len     The length of the string key.
 *  @param value   The value to insert.
 *
 *  @return On success 0 is returned.
 *          -1 is returned if the input is invalid
 *          -2 is returned if there was a memory failure
 *          -3 is returned if there was not space due to hash collisions
 */
int hashmap_put(hashmap_t *const hashmap, const char *const key,
                size_t len, void *const value);


/**
 *  Get an element from the hashmap.
 *
 *  @param hashmap The hashmap to get from.
 *  @param key     The string key to use.
 *  @param len     The length of the string key.
 *
 *  @return The previously set element, or NULL if none exists.
 */
void *hashmap_get(const hashmap_t *const hashmap,
                  const char *const key, size_t len);


/**
 *  Remove an element from the hashmap.
 *
 *  @param hashmap The hashmap to remove from.
 *  @param key     The string key to use.
 *  @param len     The length of the string key.
 *
 *  @return 0 is returned if the element was removed
 *          1 is returned if no element was found to remove
 */
int hashmap_remove(hashmap_t *const hashmap,
                   const char *const key, size_t len);


/**
 *  Remove an element from the hashmap and return the key.  This call provides
 *  a way to get the key back so it can be freed.
 *
 *  @param hashmap The hashmap to remove from.
 *  @param key     The string key to use.
 *  @param len     The length of the string key.
 *
 *  @return On success the original stored key pointer is returned,
 *          on failure NULL is returned.
 */
const char *hashmap_remove_and_return_key(hashmap_t *const hashmap,
                                          const char *const key, size_t len);


/**
 *  Iterate over all the values in a hashmap.
 *
 *  @param hashmap The hashmap to iterate over.
 *  @param f       The function pointer to call on each element.
 *  @param context The context to pass as the first argument to f.
 *
 *  @return If the entire hashmap was iterated then 0 is returned. Otherwise if
 *          the callback function f returned non-zero then non-zero is returned.
 */
int hashmap_iterate(const hashmap_t *const hashmap,
                    int (*f)(void *const context, void *const value),
                    void *const context);


/**
 *  Iterate over all the elements in a hashmap.
 *
 *  @param hashmap The hashmap to iterate over.
 *  @param f       The function pointer to call on each element.
 *  @param context The context to pass as the first argument to f.
 *
 *  @return If the entire hashmap was iterated then 0 is returned.
 *          If the callback function f returned positive then the positive
 *              value is returned.
 *          If the callback function returns -1, the current item is removed
 *              and iteration continues.
 */
int hashmap_iterate_pairs(hashmap_t *const hashmap,
                          int (*f)(void *const context, struct hashmap_element *const),
                          void *const context);


/**
 *  Get the size of the hashmap.
 *
 *  @param hashmap The hashmap to get the size of.
 *
 *  @return The size of the hashmap.
 */
size_t hashmap_num_entries(const hashmap_t *const hashmap);


/**
 *  Destroy the hashmap.
 *
 * @param hashmap The hashmap to destroy.
 */
void hashmap_destroy(hashmap_t *const hashmap);


#endif

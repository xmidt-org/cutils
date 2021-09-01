/* SPDX-FileCopyrightText: 2020 Neil Henning, et. al */
/* SPDX-License-Identifier: Unlicense */

/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/hashmap.h

   Commit: c78ba67ab540c3ee2044094f9c0945f877b367d2
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
#include <stdint.h>
#include <string.h>

#include "hashmap.h"

#define HASHMAP_MAX_CHAIN_LENGTH (8)
#define HASHMAP_MAX_SIZE (1 << 31) /* Up to 2G entries work */
#define HASHMAP_DEFAULT_SIZE (16)

/*----------------------------------------------------------------------------*/
/*                            Function Prototypes                             */
/*----------------------------------------------------------------------------*/

extern uint32_t hashmap_crc32_helper(const char *const s, const size_t len);


static size_t hashmap_hash_helper_int_helper(const hashmap_t *const m,
                                             const char *const keystring,
                                             const size_t len);
static int hashmap_match_helper(const struct hashmap_element *const element,
                                const char *const key,
                                const size_t len);
static int hashmap_hash_helper(const hashmap_t *const m,
                               const char *const key, const size_t len,
                               size_t *const out_index);
static int hashmap_rehash_iterator(void *const new_hash,
                                   struct hashmap_element *const e);
static int hashmap_rehash_helper(hashmap_t *const m);
static size_t num_to_pow2(size_t num);


/*----------------------------------------------------------------------------*/
/*                            External Functions                              */
/*----------------------------------------------------------------------------*/


int hashmap_create(size_t initial_size, hashmap_t *const out_hashmap)
{
    /* Exit if the number is too large, or if we'd crash. */
    if (!out_hashmap || (HASHMAP_MAX_SIZE < initial_size)) {
        return -1;
    }

    /* Simplify and assume there will be a default. */
    if (0 == initial_size) {
        initial_size = HASHMAP_DEFAULT_SIZE;
    }
    initial_size = num_to_pow2(initial_size);

    out_hashmap->table_size = initial_size;
    out_hashmap->size = 0;

    out_hashmap->data = calloc(initial_size, sizeof(struct hashmap_element));
    if (!out_hashmap->data) {
        return -2;
    }

    return 0;
}


int hashmap_put(hashmap_t *const m, const char *const key,
                size_t len, void *const value)
{
    size_t index = 0;

    if (!m) {
        return -1;
    }

    /* Make a new hashmap if this is the first put */
    if (!m->data) {
        int rv = hashmap_create(0, m);
        if (rv) {
            return rv;
        }
    }

    /* Find a place to put our value. */
    while (!hashmap_hash_helper(m, key, len, &index)) {
        int rv = hashmap_rehash_helper(m);
        if (rv) {
            return rv;
        }
    }

    /* Set the data. */
    m->data[index].data = value;
    m->data[index].key = key;
    m->data[index].key_len = len;

    /* If the hashmap element was not already in use, set that it is being used
     * and bump our size. */
    if (0 == m->data[index].in_use) {
        m->data[index].in_use = 1;
        m->size++;
    }

    return 0;
}


void *hashmap_get(const hashmap_t *const m, const char *const key, size_t len)
{
    size_t curr;

    /* Return empty if the hash is not created */
    if (!m || !m->data) {
        return NULL;
    }

    /* Find data location */
    curr = hashmap_hash_helper_int_helper(m, key, len);

    /* Linear probing, if necessary */
    for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
        if (m->data[curr].in_use) {
            if (hashmap_match_helper(&m->data[curr], key, len)) {
                return m->data[curr].data;
            }
        }

        curr = (curr + 1) % m->table_size;
    }

    /* Not found */
    return NULL;
}


int hashmap_remove(hashmap_t *const m, const char *const key, size_t len)
{
    size_t curr;

    /* Nothing to remove. */
    if (!m || !m->data) {
        return 1;
    }

    /* Find key */
    curr = hashmap_hash_helper_int_helper(m, key, len);

    /* Linear probing, if necessary */
    for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
        if (m->data[curr].in_use) {
            if (hashmap_match_helper(&m->data[curr], key, len)) {
                /* Blank out the fields including in_use */
                memset(&m->data[curr], 0, sizeof(struct hashmap_element));

                /* Reduce the size */
                m->size--;

                return 0;
            }
        }

        curr = (curr + 1) % m->table_size;
    }

    return 1;
}


const char *hashmap_remove_and_return_key(hashmap_t *const m,
                                          const char *const key,
                                          size_t len)
{
    size_t curr;

    /* Nothing to remove. */
    if (!m || !m->data) {
        return NULL;
    }

    /* Find key */
    curr = hashmap_hash_helper_int_helper(m, key, len);

    /* Linear probing, if necessary */
    for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
        if (m->data[curr].in_use) {
            if (hashmap_match_helper(&m->data[curr], key, len)) {
                const char *const stored_key = m->data[curr].key;

                /* Blank out the fields */
                m->data[curr].in_use = 0;
                m->data[curr].data = NULL;
                m->data[curr].key = NULL;

                /* Reduce the size */
                m->size--;

                return stored_key;
            }
        }
        curr = (curr + 1) % m->table_size;
    }

    return NULL;
}


int hashmap_iterate(const hashmap_t *const m,
                    int (*f)(void *const, void *const), void *const context)
{
    if (!m) {
        return 0;
    }

    /* Linear probing */
    for (size_t i = 0; i < m->table_size; i++) {
        if (m->data[i].in_use) {
            if (!f(context, m->data[i].data)) {
                return 1;
            }
        }
    }
    return 0;
}


int hashmap_iterate_pairs(hashmap_t *const m,
                          int (*f)(void *const, struct hashmap_element *const),
                          void *const context)
{
    if (!m) {
        return 0;
    }

    /* Linear probing */
    for (size_t i = 0; i < m->table_size; i++) {
        struct hashmap_element *p = &m->data[i];

        if (p->in_use) {
            int r = f(context, p);

            switch (r) {
            case -1: /* remove item */
                memset(p, 0, sizeof(struct hashmap_element));
                m->size--;
                break;
            case 0: /* continue iterating */
                break;
            default: /* early exit */
                return 1;
            }
        }
    }
    return 0;
}


void hashmap_destroy(hashmap_t *const m)
{
    if (m) {
        if (m->data) {
            free(m->data);
        }
        memset(m, 0, sizeof(hashmap_t));
    }
}


size_t hashmap_num_entries(const hashmap_t *const m)
{
    if (m) {
        return m->size;
    }

    return 0;
}


/*----------------------------------------------------------------------------*/
/*                            Internal Functions                              */
/*----------------------------------------------------------------------------*/


static size_t hashmap_hash_helper_int_helper(const hashmap_t *const m,
                                             const char *const keystring,
                                             const size_t len)
{
    uint32_t key = hashmap_crc32_helper(keystring, len);

    /* Robert Jenkins' 32 bit Mix Function */
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);

    /* Knuth's Multiplicative Method */
    key = (key >> 3) * 2654435761;

    return key % m->table_size;
}


static int hashmap_match_helper(const struct hashmap_element *const element,
                                const char *const key, const size_t len)
{
    return (element->key_len == len) && (0 == memcmp(element->key, key, len));
}


static int hashmap_hash_helper(const hashmap_t *const m, const char *const key,
                               const size_t len, size_t *const out_index)
{
    size_t curr = 0;
    size_t first_empty = SIZE_MAX;
    int total_in_use = 0;

    /* If full, return immediately */
    if (m->size >= m->table_size) {
        return 0;
    }

    /* Find the best index */
    curr = hashmap_hash_helper_int_helper(m, key, len);

    /* First linear probe to check if we've already insert the element */
    total_in_use = 0;

    for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
        if (m->data[curr].in_use) {
            total_in_use++;

            if (hashmap_match_helper(&m->data[curr], key, len)) {
                /* exit if we found it. */
                *out_index = curr;
                return 1;
            }
        } else if (SIZE_MAX == first_empty) {
            first_empty = curr;
        }

        curr = (curr + 1) % m->table_size;
    }

    /* Actually insert our element at the first found empty slot. */
    if (SIZE_MAX != first_empty) {
        /* exit if we found a place for it. */
        *out_index = first_empty;
        return 1;
    }

    /* No room for the element. */
    return 0;
}


static int hashmap_rehash_iterator(void *const new_hash,
                                   struct hashmap_element *const e)
{
    int temp = hashmap_put((hashmap_t *)new_hash, e->key, e->key_len, e->data);
    if (0 != temp) {
        return 1;
    }
    /* clear old value to avoid stale pointers */
    return -1;
}


/*
 * Doubles the size of the hashmap, and rehashes all the elements
 */
static int hashmap_rehash_helper(hashmap_t *const m)
{
    /* If this multiplication overflows hashmap_create will fail. */
    size_t new_size = 0;
    int rv = 0;
    hashmap_t new_hash;

    if ((2 * m->size) < m->table_size) {
        return -3;
    }

    /* While this will result in doubling the size, this will not accidentally
     * overflow the size. */
    new_size = m->table_size + HASHMAP_DEFAULT_SIZE;
    rv = hashmap_create(new_size, &new_hash);
    if (0 == rv) {

        /* copy the old elements to the new table */
        rv = hashmap_iterate_pairs(m, hashmap_rehash_iterator, &new_hash);
        if (0 == rv) {

            hashmap_destroy(m);
            /* put new hash into old hash structure by copying */
            memcpy(m, &new_hash, sizeof(hashmap_t));
        }
    }
    return rv;
}


/**
 * Figure out the power of 2 size that fits the ask.
 *
 * https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 *
 * @param num The number to find the next higher ^2 that fits
 *
 * @return The ^2 number that fits the desired number
 */
static size_t num_to_pow2(size_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

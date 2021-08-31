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

/*----------------------------------------------------------------------------*/
/*                            Function Prototypes                             */
/*----------------------------------------------------------------------------*/
static uint32_t hashmap_crc32_helper(const char *const s,
                                     const size_t len);
static uint32_t hashmap_hash_helper_int_helper(const hashmap_t *const m,
                                               const char *const keystring,
                                               const size_t len);
static int hashmap_match_helper(const struct hashmap_element *const element,
                                const char *const key,
                                const size_t len);
static int hashmap_hash_helper(const hashmap_t *const m,
                               const char *const key, const size_t len,
                               uint32_t *const out_index);
static int hashmap_rehash_iterator(void *const new_hash,
                                   struct hashmap_element *const e);
static int hashmap_rehash_helper(hashmap_t *const m);
static size_t num_to_pow2(size_t num);


/*----------------------------------------------------------------------------*/
/*                            External Functions                              */
/*----------------------------------------------------------------------------*/


int hashmap_create(size_t initial_size, hashmap_t *const out_hashmap)
{
    out_hashmap->table_size = initial_size;
    out_hashmap->size = 0;

    /* Only exit if the number is too large. */
    if (HASHMAP_MAX_SIZE < initial_size) {
        return 1;
    }

    /* Simplify and assume there will be 1 vs. failing and returning. */
    if (0 == initial_size) {
        initial_size = 1;
    }
    initial_size = num_to_pow2(initial_size);

    out_hashmap->data = calloc(initial_size, sizeof(struct hashmap_element));
    if (!out_hashmap->data) {
        return 1;
    }

    return 0;
}


int hashmap_put(hashmap_t *const m, const char *const key,
                size_t len, void *const value)
{
    uint32_t index = 0;

    /* Find a place to put our value. */
    while (!hashmap_hash_helper(m, key, len, &index)) {
        if (hashmap_rehash_helper(m)) {
            return 1;
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
    free(m->data);
    memset(m, 0, sizeof(hashmap_t));
}


size_t hashmap_num_entries(const hashmap_t *const m)
{
    return m->size;
}


/*----------------------------------------------------------------------------*/
/*                            Internal Functions                              */
/*----------------------------------------------------------------------------*/


static uint32_t hashmap_crc32_helper(const char *const s, const size_t len)
{
    uint32_t crc32val = 0;

    // Using polynomial 0x11EDC6F41 to match SSE 4.2's crc function.
    static const uint32_t crc32_tab[] = {
        0x00000000U, 0xF26B8303U, 0xE13B70F7U, 0x1350F3F4U, 0xC79A971FU,
        0x35F1141CU, 0x26A1E7E8U, 0xD4CA64EBU, 0x8AD958CFU, 0x78B2DBCCU,
        0x6BE22838U, 0x9989AB3BU, 0x4D43CFD0U, 0xBF284CD3U, 0xAC78BF27U,
        0x5E133C24U, 0x105EC76FU, 0xE235446CU, 0xF165B798U, 0x030E349BU,
        0xD7C45070U, 0x25AFD373U, 0x36FF2087U, 0xC494A384U, 0x9A879FA0U,
        0x68EC1CA3U, 0x7BBCEF57U, 0x89D76C54U, 0x5D1D08BFU, 0xAF768BBCU,
        0xBC267848U, 0x4E4DFB4BU, 0x20BD8EDEU, 0xD2D60DDDU, 0xC186FE29U,
        0x33ED7D2AU, 0xE72719C1U, 0x154C9AC2U, 0x061C6936U, 0xF477EA35U,
        0xAA64D611U, 0x580F5512U, 0x4B5FA6E6U, 0xB93425E5U, 0x6DFE410EU,
        0x9F95C20DU, 0x8CC531F9U, 0x7EAEB2FAU, 0x30E349B1U, 0xC288CAB2U,
        0xD1D83946U, 0x23B3BA45U, 0xF779DEAEU, 0x05125DADU, 0x1642AE59U,
        0xE4292D5AU, 0xBA3A117EU, 0x4851927DU, 0x5B016189U, 0xA96AE28AU,
        0x7DA08661U, 0x8FCB0562U, 0x9C9BF696U, 0x6EF07595U, 0x417B1DBCU,
        0xB3109EBFU, 0xA0406D4BU, 0x522BEE48U, 0x86E18AA3U, 0x748A09A0U,
        0x67DAFA54U, 0x95B17957U, 0xCBA24573U, 0x39C9C670U, 0x2A993584U,
        0xD8F2B687U, 0x0C38D26CU, 0xFE53516FU, 0xED03A29BU, 0x1F682198U,
        0x5125DAD3U, 0xA34E59D0U, 0xB01EAA24U, 0x42752927U, 0x96BF4DCCU,
        0x64D4CECFU, 0x77843D3BU, 0x85EFBE38U, 0xDBFC821CU, 0x2997011FU,
        0x3AC7F2EBU, 0xC8AC71E8U, 0x1C661503U, 0xEE0D9600U, 0xFD5D65F4U,
        0x0F36E6F7U, 0x61C69362U, 0x93AD1061U, 0x80FDE395U, 0x72966096U,
        0xA65C047DU, 0x5437877EU, 0x4767748AU, 0xB50CF789U, 0xEB1FCBADU,
        0x197448AEU, 0x0A24BB5AU, 0xF84F3859U, 0x2C855CB2U, 0xDEEEDFB1U,
        0xCDBE2C45U, 0x3FD5AF46U, 0x7198540DU, 0x83F3D70EU, 0x90A324FAU,
        0x62C8A7F9U, 0xB602C312U, 0x44694011U, 0x5739B3E5U, 0xA55230E6U,
        0xFB410CC2U, 0x092A8FC1U, 0x1A7A7C35U, 0xE811FF36U, 0x3CDB9BDDU,
        0xCEB018DEU, 0xDDE0EB2AU, 0x2F8B6829U, 0x82F63B78U, 0x709DB87BU,
        0x63CD4B8FU, 0x91A6C88CU, 0x456CAC67U, 0xB7072F64U, 0xA457DC90U,
        0x563C5F93U, 0x082F63B7U, 0xFA44E0B4U, 0xE9141340U, 0x1B7F9043U,
        0xCFB5F4A8U, 0x3DDE77ABU, 0x2E8E845FU, 0xDCE5075CU, 0x92A8FC17U,
        0x60C37F14U, 0x73938CE0U, 0x81F80FE3U, 0x55326B08U, 0xA759E80BU,
        0xB4091BFFU, 0x466298FCU, 0x1871A4D8U, 0xEA1A27DBU, 0xF94AD42FU,
        0x0B21572CU, 0xDFEB33C7U, 0x2D80B0C4U, 0x3ED04330U, 0xCCBBC033U,
        0xA24BB5A6U, 0x502036A5U, 0x4370C551U, 0xB11B4652U, 0x65D122B9U,
        0x97BAA1BAU, 0x84EA524EU, 0x7681D14DU, 0x2892ED69U, 0xDAF96E6AU,
        0xC9A99D9EU, 0x3BC21E9DU, 0xEF087A76U, 0x1D63F975U, 0x0E330A81U,
        0xFC588982U, 0xB21572C9U, 0x407EF1CAU, 0x532E023EU, 0xA145813DU,
        0x758FE5D6U, 0x87E466D5U, 0x94B49521U, 0x66DF1622U, 0x38CC2A06U,
        0xCAA7A905U, 0xD9F75AF1U, 0x2B9CD9F2U, 0xFF56BD19U, 0x0D3D3E1AU,
        0x1E6DCDEEU, 0xEC064EEDU, 0xC38D26C4U, 0x31E6A5C7U, 0x22B65633U,
        0xD0DDD530U, 0x0417B1DBU, 0xF67C32D8U, 0xE52CC12CU, 0x1747422FU,
        0x49547E0BU, 0xBB3FFD08U, 0xA86F0EFCU, 0x5A048DFFU, 0x8ECEE914U,
        0x7CA56A17U, 0x6FF599E3U, 0x9D9E1AE0U, 0xD3D3E1ABU, 0x21B862A8U,
        0x32E8915CU, 0xC083125FU, 0x144976B4U, 0xE622F5B7U, 0xF5720643U,
        0x07198540U, 0x590AB964U, 0xAB613A67U, 0xB831C993U, 0x4A5A4A90U,
        0x9E902E7BU, 0x6CFBAD78U, 0x7FAB5E8CU, 0x8DC0DD8FU, 0xE330A81AU,
        0x115B2B19U, 0x020BD8EDU, 0xF0605BEEU, 0x24AA3F05U, 0xD6C1BC06U,
        0xC5914FF2U, 0x37FACCF1U, 0x69E9F0D5U, 0x9B8273D6U, 0x88D28022U,
        0x7AB90321U, 0xAE7367CAU, 0x5C18E4C9U, 0x4F48173DU, 0xBD23943EU,
        0xF36E6F75U, 0x0105EC76U, 0x12551F82U, 0xE03E9C81U, 0x34F4F86AU,
        0xC69F7B69U, 0xD5CF889DU, 0x27A40B9EU, 0x79B737BAU, 0x8BDCB4B9U,
        0x988C474DU, 0x6AE7C44EU, 0xBE2DA0A5U, 0x4C4623A6U, 0x5F16D052U,
        0xAD7D5351U
    };

    for (size_t i = 0; i < len; i++) {
        crc32val = crc32_tab[((uint8_t)crc32val) ^ ((uint8_t)s[i])] ^ (crc32val >> 8);
    }
    return crc32val;
}


static uint32_t hashmap_hash_helper_int_helper(const hashmap_t *const m,
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
                               const size_t len, uint32_t *const out_index)
{
    unsigned int start, curr;
    int total_in_use;

    /* If full, return immediately */
    if (m->size >= m->table_size) {
        return 0;
    }

    /* Find the best index */
    curr = start = hashmap_hash_helper_int_helper(m, key, len);

    /* First linear probe to check if we've already insert the element */
    total_in_use = 0;

    for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
        const int in_use = m->data[curr].in_use;

        total_in_use += in_use;

        if (in_use && hashmap_match_helper(&m->data[curr], key, len)) {
            *out_index = curr;
            return 1;
        }

        curr = (curr + 1) % m->table_size;
    }

    curr = start;

    /* Second linear probe to actually insert our element (only if there was at
   * least one empty entry) */
    if (HASHMAP_MAX_CHAIN_LENGTH > total_in_use) {
        for (int i = 0; i < HASHMAP_MAX_CHAIN_LENGTH; i++) {
            if (!m->data[curr].in_use) {
                *out_index = curr;
                return 1;
            }

            curr = (curr + 1) % m->table_size;
        }
    }

    return 0;
}


static int hashmap_rehash_iterator(void *const new_hash,
                                   struct hashmap_element *const e)
{
    int temp = hashmap_put((hashmap_t *)new_hash, e->key, e->key_len, e->data);
    if (0 < temp) {
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
    size_t new_size = 2 * m->table_size;

    hashmap_t new_hash;

    int flag = hashmap_create(new_size, &new_hash);

    if (0 != flag) {
        return flag;
    }

    /* copy the old elements to the new table */
    flag = hashmap_iterate_pairs(m, hashmap_rehash_iterator, &new_hash);

    if (0 != flag) {
        return flag;
    }

    hashmap_destroy(m);
    /* put new hash into old hash structure by copying */
    memcpy(m, &new_hash, sizeof(hashmap_t));

    return 0;
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

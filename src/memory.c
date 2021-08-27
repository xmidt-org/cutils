/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

void *saferealloc(void *ptr, size_t size)
{
    void *p = NULL;

    /* Consistently always return NULL in this case. */
    if (!ptr && !size) {
        return NULL;
    }

    p = realloc(ptr, size);

    /* If size == 0, ptr has been freed. */
    if (!p && ptr && (0 < size)) {
        free(ptr);
    }

    return p;
}


void *memdup(const void *src, size_t len)
{
    void *dest = NULL;

    if (src && len) {
        dest = malloc(len);
        if (dest) {
            memcpy(dest, src, len);
        }
    }

    return dest;
}


void *memappend(void **buf, size_t *buf_len, const void *src, size_t len)
{
    *buf = saferealloc(*buf, (*buf_len + len));

    if (*buf) {
        unsigned char *p = *buf;
        memcpy(&p[*buf_len], src, len);
        *buf_len += len;

        return *buf;
    }

    *buf_len = 0;
    return NULL;
}


/*-- Must versions -----------------------------------------------------------*/


void *must_calloc(size_t nmemb, size_t size)
{
    void *rv = calloc(nmemb, size);

    if (!rv) {
        abort();
    }

    return rv;
}


void *must_realloc(void *ptr, size_t size)
{
    void *p = NULL;

    if (!size) {
        if (ptr) {
            free(ptr);
        }
        return NULL;
    }

    p = realloc(ptr, size);

    if (!p) {
        abort();
    }

    return p;
}


void *must_memdup(const void *src, size_t len)
{
    void *rv = memdup(src, len);

    if (len && !rv) {
        abort();
    }

    return rv;
}


void *must_memappend(void **buf, size_t *buf_len, const void *src, size_t len)
{
    void *rv = NULL;
    size_t resulting_len = *buf_len + len;

    rv = memappend(buf, buf_len, src, len);

    if (resulting_len && !rv) {
        abort();
    }

    return rv;
}


/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdlib.h>
#include <string.h>

#include "must.h"

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
    return must(calloc(nmemb, size));
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

    p = must(realloc(ptr, size));

    return p;
}


void *must_memdup(const void *src, size_t len)
{
    if (src && len) {
        return must(memdup(src, len));
    }

    return NULL;
}


void *must_memappend(void **buf, size_t *buf_len, const void *src, size_t len)
{
    size_t resulting_len = *buf_len + len;

    if (!resulting_len) {
        return NULL;
    }

    return must(memappend(buf, buf_len, src, len));
}

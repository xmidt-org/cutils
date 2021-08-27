/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nl_strings.h"
#include "strings.h"

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

char *cu_strdup(const char *s)
{
    return cu_strndup(s, SIZE_MAX);
}


size_t cu_strnlen(const char *s, size_t maxlen)
{
    for (size_t i = 0; i < maxlen; i++) {
        if ('\0' == s[i]) {
            return i;
        }
    }

    return maxlen;
}


char *cu_strndup(const char *s, size_t maxlen)
{
    char *rv = NULL;

    if (s && (0 < maxlen)) {
        size_t len = cu_strnlen(s, maxlen);

        rv = malloc(len + 1); /* +1 for trailing '\0' */
        if (rv) {
            memcpy(rv, s, len);
            rv[len] = '\0';
        }
    }

    return rv;
}


char *cu_must_strdup(const char *s)
{
    return cu_strndup(s, SIZE_MAX);
}


char *cu_must_strndup(const char *s, size_t maxlen)
{
    char *rv = NULL;

    if (s && (0 < maxlen)) {
        rv = cu_strndup(s, maxlen);
        if (!rv) {
            abort();
        }
    }

    return rv;
}

int nl_toupper(int c)
{
    /* Note: Does not work on EBCDIC systems */
    if (('a' <= c) && (c <= 'z')) {
        return 'A' + c - 'a';
    }

    return c;
}


int nl_tolower(int c)
{
    /* Note: Does not work on EBCDIC systems */
    if (('A' <= c) && (c <= 'Z')) {
        return 'a' + c - 'A';
    }

    return c;
}


int nl_strncasecmp(const char *s1, const char *s2, size_t maxlen)
{
    int c;

    if ((0 == maxlen) || (s1 == s2)) {
        return 0;
    }

    do {
        c = nl_tolower(*s1) - nl_tolower(*s2);
        maxlen--;
        s1++;
        s2++;
    } while (0 < maxlen && 0 == c);

    return c;
}

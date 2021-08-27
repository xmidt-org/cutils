/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "printf.h"

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

char *maprintf(const char *format, ...)
{
    va_list args;
    char *buf = NULL;

    va_start(args, format);
    buf = mlvaprintf(NULL, format, args);
    va_end(args);

    return buf;
}


char *mvaprintf(const char *format, va_list args)
{
    return mlvaprintf(NULL, format, args);
}


char *mlaprintf(size_t *len, const char *format, ...)
{
    va_list args;
    char *buf = NULL;

    va_start(args, format);
    buf = mlvaprintf(len, format, args);
    va_end(args);

    return buf;
}


char *mlvaprintf(size_t *len, const char *format, va_list args)
{
    va_list copy;
    char *buf = NULL;
    int l;

    va_copy(copy, args);
    l = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (0 < l) {

        buf = malloc(l + 1);
        if (buf) {
            int rv = vsprintf(buf, format, args);

            if (rv != l) {
                free(buf);
                buf = NULL;
                l = 0;
            }
        } else {
            l = 0;
        }
    }

    if (len) {
        *len = (size_t)l;
    }

    return buf;
}


/*-- Must versions -----------------------------------------------------------*/


char *must_maprintf(const char *format, ...)
{
    va_list args;
    char *buf = NULL;

    va_start(args, format);
    buf = must_mlvaprintf(NULL, format, args);
    va_end(args);

    return buf;
}


char *must_mvaprintf(const char *format, va_list args)
{
    return must_mlvaprintf(NULL, format, args);
}


char *must_mlaprintf(size_t *len, const char *format, ...)
{
    va_list args;
    char *buf = NULL;

    va_start(args, format);
    buf = must_mlvaprintf(len, format, args);
    va_end(args);

    return buf;
}


char *must_mlvaprintf(size_t *len, const char *format, va_list args)
{
    char *rv = NULL;

    if (!format) {
        *len = 0;
        return NULL;
    }

    rv = mlvaprintf(len, format, args);

    if (!rv) {
        abort();
    }

    return rv;
}


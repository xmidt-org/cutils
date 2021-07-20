/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __NL_STRINGS_H__
#define __NL_STRINGS_H__

#include <stddef.h>

/* These are special non-locale affected implementations of the standard
 * functions.
 *
 * https://daniel.haxx.se/blog/2008/10/15/strcasecmp-in-turkish/
 */
int nl_toupper(int c);
int nl_tolower(int c);
int nl_strncasecmp(const char *s1, const char *s2, size_t maxlen);

#endif

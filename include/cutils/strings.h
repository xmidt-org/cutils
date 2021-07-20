/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <stddef.h>

/**
 * 'standard' but often missing functions
 */
size_t cu_strnlen(const char *s, size_t maxlen);
char *cu_strdup(const char *s);
char *cu_strndup(const char *s, size_t maxlen);

#endif

/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>
#include <stddef.h>

/* Versions of printf() that provide an on demand allocated buffer
 * that fits the data.  The returned buffer must be called with free(). */
char *maprintf(const char *format, ...);
char *mvaprintf(const char *format, va_list args);

/* Versions of printf() that provide an on demand allocated buffer that
 * fits the data.  The strlen() value of the string is returned in len if
 * it is not NULL.  The returned buffer must be called with free(). */
char *mlaprintf(size_t *len, const char *format, ...);
char *mlvaprintf(size_t *len, const char *format, va_list args);

#endif

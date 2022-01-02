/* SPDX-FileCopyrightText: 1998-2021 Daniel Stenberg, <daniel@haxx.se>, et al */
/* SPDX-License-Identifier: MIT */

// clang-format off
#ifndef __NL_CTYPE_H__
#define __NL_CTYPE_H__

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* Originally from:
 * https://github.com/curl/curl/blob/master/lib/curl_ctype.h
 *
 * Commit:
 * https://github.com/curl/curl/commit/ac0a88fd2552524955233198de96cc66f6b15a07
 *
 * Date:
 * July 16, 2021
 *
 * nl = Non-Localized
 *
 * Changes:
 *    - Renamed Curl_* to be nl_*
 *    - Replaced the wrapper #ifdef with the filename
 *    - Removed EBCDIC ifdef
 */


int nl_isspace(int c);
int nl_isdigit(int c);
int nl_isalnum(int c);
int nl_isxdigit(int c);
int nl_isgraph(int c);
int nl_isprint(int c);
int nl_isalpha(int c);
int nl_isupper(int c);
int nl_islower(int c);
int nl_iscntrl(int c);

#define NL_ISSPACE(x) (nl_isspace((int)((unsigned char)x)))
#define NL_ISDIGIT(x) (nl_isdigit((int)((unsigned char)x)))
#define NL_ISALNUM(x) (nl_isalnum((int)((unsigned char)x)))
#define NL_ISXDIGIT(x) (nl_isxdigit((int)((unsigned char)x)))
#define NL_ISGRAPH(x) (nl_isgraph((int)((unsigned char)x)))
#define NL_ISALPHA(x) (nl_isalpha((int)((unsigned char)x)))
#define NL_ISPRINT(x) (nl_isprint((int)((unsigned char)x)))
#define NL_ISUPPER(x) (nl_isupper((int)((unsigned char)x)))
#define NL_ISLOWER(x) (nl_islower((int)((unsigned char)x)))
#define NL_ISCNTRL(x) (nl_iscntrl((int)((unsigned char)x)))
#define NL_ISASCII(x) (((x) >= 0) && ((x) <= 0x80))

#define NL_ISBLANK(x) (int)((((unsigned char)x) == ' ') || (((unsigned char)x) == '\t'))

#endif
// clang-format on

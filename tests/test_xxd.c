/* SPDX-FileCopyrightText: 2008, 2021-2022 Ed Rose, Weston Schmidt */
/* SPDX-License-Identifier: Apache-2.0 */
/*
 *  xxd.c - formatted (like xxd) buffer dump
 *
 *  Written by Ed Rose, Weston Schmidt (2008)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -- or --
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  In other words, you are welcome to use, share and improve this program.
 *  You are forbidden to forbid anyone else to use, share and improve
 *  what you give them.   Help stamp out software-hoarding!
 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xxd.h"

static char out[1000];
static int index = 0;

void reset_test(void)
{
    index = 0;
    memset(out, 0, sizeof(out));
}

int fputc(int c, FILE *stream)
{
    CU_ASSERT(stdout == stream);

    out[index++] = (char) c;

    return c;
}

void test_xxd(void)
{
    // clang-format off
    struct test {
        const char *in;
        size_t len;
        const char *expect;
    } tests[] = {
        {
            .in = "01234567",
            .len = 8,
            .expect = "0000000: 3031 3233 3435 3637                      01234567        \n",
        },
        {
            .in = "0123456701\x00\x7f"
                  "4567",
            .len = 16,
            .expect = "0000000: 3031 3233 3435 3637 3031 007f 3435 3637  0123456701..4567\n",
        },
        {
            .in = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
                  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
            .len = 32,
            .expect = "0000000: f0f1 f2f3 f4f5 f6f7 f8f9 fafb fcfd feff  ................\n"
                      "0000010: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f  ................\n",
        },
        {
            .in = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"
                  "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
            .len = 30,
            .expect = "0000000: f0f1 f2f3 f4f5 f6f7 f8f9 fafb fcfd feff  ................\n"
                      "0000010: 0001 0203 0405 0607 0809 0a0b 0c0d       ..............  \n",
        },
        {
            .in = NULL,
            .len = 30,
            .expect = "",
        },
        {
            .in = "01234567",
            .len = 0,
            .expect = "",
        },
    };
    // clang-format on

    for (size_t i = 0; i < sizeof(tests) / sizeof(struct test); i++) {
        size_t len = strlen(tests[i].expect);

        reset_test();
        xxd(tests[i].in, tests[i].len, stdout);
#if 0
        printf("index = %d, len = %zd\n", index, len);
        printf("got:\n'%.*s'\nwanted:\n'%s'\n", index, out, tests[i].expect);
#endif
        CU_ASSERT(index == (int) len);
        CU_ASSERT_NSTRING_EQUAL(out, tests[i].expect, tests[i].len);
    }
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("utils.c tests", NULL, NULL);
    CU_add_test(*suite, "xxd() Tests", test_xxd);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    unsigned rv     = 1;
    CU_pSuite suite = NULL;

    if (CUE_SUCCESS == CU_initialize_registry()) {
        add_suites(&suite);

        if (NULL != suite) {
            CU_basic_set_mode(CU_BRM_VERBOSE);
            CU_basic_run_tests();
            printf("\n");
            CU_basic_show_failures(CU_get_failure_list());
            printf("\n\n");
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();
    }

    if (0 != rv) {
        return 1;
    }

    return 0;
}

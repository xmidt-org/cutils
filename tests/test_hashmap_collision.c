/* SPDX-FileCopyrightText: 2020 Neil Henning, et. al*/
/* SPDX-License-Identifier: Unlicense */

/*
 * Copied from https://github.com/sheredom/hashmap.h/blob/master/test/test.c
 * and modified to work in this build ecosystem.
 */
#include <CUnit/Basic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

uint32_t hashmap_crc32_helper(const char *const s, const size_t len)
{
    (void) s;
    (void) len;

    /* Always hash to the same slot to fill the hashmap up at just
     * the one slot. */
    return 0;
}


void test_collision()
{
    hashmap_t h;
    const char *list[] = {
        "000", "one", "two", "333",
        "444", "555", "six", "777",
        "888", "999", "aaa", "bbb",
        "ccc", "ddd", "eee", "fff"
    };
    int ignored = 0;

    CU_ASSERT(0 == hashmap_create(16, &h));

    for (int i = 0; i < 16; i++) {
        int rv = hashmap_put(&h, list[i], 3, &ignored);
        if (0 != rv) {
            CU_ASSERT(0 < i);
            CU_ASSERT(NULL != hashmap_get(&h, list[i - 1], 3));
            CU_ASSERT(NULL == hashmap_get(&h, list[i], 3));
            break;
        }
    }

    hashmap_destroy(&h);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("hashmap.c tests", NULL, NULL);
    CU_add_test(*suite, "Collision Test", test_collision);
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

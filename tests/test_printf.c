/*
 * SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <CUnit/Basic.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "printf.h"


void test_maprintf(void)
{
    char *got;
    size_t len;

    got = maprintf("Hello, %s.", "world");
    CU_ASSERT_FATAL(NULL != got);
    CU_ASSERT_STRING_EQUAL(got, "Hello, world.");
    free(got);

    got = mlaprintf(&len, "Answer: %d", 42);
    CU_ASSERT_FATAL(NULL != got);
    CU_ASSERT(10 == len);
    CU_ASSERT_STRING_EQUAL(got, "Answer: 42");
    free(got);

    got = mlaprintf(&len, NULL);
    CU_ASSERT_FATAL(NULL == got);

    got = mlaprintf(NULL, NULL);
    CU_ASSERT_FATAL(NULL == got);
}


void test_must_maprintf(void)
{
    char *got;
    size_t len;

    got = must_maprintf("Hello, %s.", "world");
    CU_ASSERT_FATAL(NULL != got);
    CU_ASSERT_STRING_EQUAL(got, "Hello, world.");
    free(got);

    got = must_mlaprintf(&len, "Answer: %d", 42);
    CU_ASSERT_FATAL(NULL != got);
    CU_ASSERT(10 == len);
    CU_ASSERT_STRING_EQUAL(got, "Answer: 42");
    free(got);

    got = must_mlaprintf(&len, NULL);
    CU_ASSERT_FATAL(NULL == got);

    got = must_mlaprintf(NULL, NULL);
    CU_ASSERT_FATAL(NULL == got);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("printf.c tests", NULL, NULL);
    CU_add_test(*suite, "maprintf() Tests", test_maprintf);
    CU_add_test(*suite, "must_maprintf() Tests", test_must_maprintf);
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(void)
{
    unsigned rv = 1;
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

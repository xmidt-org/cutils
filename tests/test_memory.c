/*SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>

#include <stdlib.h>

#include "memory.h"

void test_saferealloc()
{
    void *p = NULL;

    CU_ASSERT_FATAL(NULL == saferealloc(NULL, 0));

    /* alloc a new buffer */
    p = saferealloc(NULL, 10);
    CU_ASSERT_FATAL(NULL != p);

    /* increase the size of the buffer */
    p = saferealloc(p, 20);
    CU_ASSERT_FATAL(NULL != p);

    /* free the buffer */
    CU_ASSERT_FATAL(NULL == saferealloc(p, 0));
}


void test_memdup()
{
    void *p = NULL;
    const char *in = "foobar";

    CU_ASSERT_FATAL(NULL == memdup(in, 0));
    CU_ASSERT_FATAL(NULL == memdup(NULL, 0));

    p = memdup(in, 6);
    CU_ASSERT_FATAL(NULL != p);

    CU_ASSERT_NSTRING_EQUAL("foobar", p, 6);

    free(p);
}


void test_memappend()
{
    void *p = NULL;
    size_t len = 0;

    CU_ASSERT_FATAL(NULL == memappend(&p, &len, NULL, 0));

    CU_ASSERT_FATAL(NULL != memappend(&p, &len, "hello", 5));
    CU_ASSERT_FATAL(5 == len);
    CU_ASSERT_NSTRING_EQUAL("hello", p, 5);

    CU_ASSERT_FATAL(NULL != memappend(&p, &len, ", world", 7));
    CU_ASSERT_FATAL(12 == len);
    CU_ASSERT_NSTRING_EQUAL("hello, world", p, 12);

    free(p);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("memory.c tests", NULL, NULL);
    CU_add_test(*suite, "saferealloc() Tests", test_saferealloc);
    CU_add_test(*suite, "memdup() Tests", test_memdup);
    CU_add_test(*suite, "memappend() Tests", test_memappend);
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

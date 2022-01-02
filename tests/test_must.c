/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>

#include "must.h"

static int abort_calls = 0;
void __test_abort(void)
{
    abort_calls++;
}

void test_must(void)
{
    int foo = 0;
    CU_ASSERT(abort_calls == 0);
    CU_ASSERT(&foo == must(&foo));
    CU_ASSERT(abort_calls == 0);

    /* Normally this will not return if NULL, but we're testing here... */
    CU_ASSERT(NULL == must(NULL));
    CU_ASSERT(abort_calls == 1);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("must.c tests", NULL, NULL);
    CU_add_test(*suite, "must() Tests", test_must);
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

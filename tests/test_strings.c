/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>

#include "strings.h"


void test_cu_strnlen()
{
    CU_ASSERT(4 == cu_strnlen("1234", 10));
    CU_ASSERT(4 == cu_strnlen("12345678", 4));
}


void test_cu_strdup()
{
    char *tmp;
    char *empty = "e";

    CU_ASSERT(NULL == cu_strdup(NULL));

    tmp = cu_strdup(&empty[1]);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = cu_strdup("asdf");
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);
}


void test_cu_strndup()
{
    char *tmp;
    char *empty = "e";
    CU_ASSERT(NULL == cu_strndup(NULL, 0));

    tmp = cu_strndup(&empty[1], 5);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = cu_strndup(&empty[1], 0);
    CU_ASSERT(NULL == tmp);

    tmp = cu_strndup("foo", 0);
    CU_ASSERT(NULL == tmp);

    tmp = cu_strndup("asdf", 12);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);

    tmp = cu_strndup("asdf", 2);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "as");
    free(tmp);
}


void test_cu_must_strdup()
{
    char *tmp;
    char *empty = "e";

    CU_ASSERT(NULL == cu_must_strdup(NULL));

    tmp = cu_must_strdup(&empty[1]);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = cu_must_strdup("asdf");
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);
}


void test_cu_must_strndup()
{
    char *tmp;
    char *empty = "e";
    CU_ASSERT(NULL == cu_must_strndup(NULL, 0));

    tmp = cu_must_strndup(&empty[1], 5);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT('\0' == *tmp);
    free(tmp);

    tmp = cu_must_strndup(&empty[1], 0);
    CU_ASSERT(NULL == tmp);

    tmp = cu_must_strndup("foo", 0);
    CU_ASSERT(NULL == tmp);

    tmp = cu_must_strndup("asdf", 12);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "asdf");
    free(tmp);

    tmp = cu_must_strndup("asdf", 2);
    CU_ASSERT(NULL != tmp);
    CU_ASSERT_STRING_EQUAL(tmp, "as");
    free(tmp);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("strings.c tests", NULL, NULL);
    CU_add_test(*suite, "cu_strnlen() Tests", test_cu_strnlen);
    CU_add_test(*suite, "cu_strdup() Tests", test_cu_strdup);
    CU_add_test(*suite, "cu_strndup() Tests", test_cu_strndup);
    CU_add_test(*suite, "cu_must_strdup() Tests", test_cu_must_strdup);
    CU_add_test(*suite, "cu_must_strndup() Tests", test_cu_must_strndup);
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

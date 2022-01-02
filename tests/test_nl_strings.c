/* SPDX-FileCopyrightText: 2021-2022 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#include <CUnit/Basic.h>

#include "nl_strings.h"


void test_nl_tocase()
{
    // clang-format off
    struct test {
        int in;
        int upper;
        int lower;
    } vector[] = {
        { .in =   0, .upper =  0,  .lower =  0  },
        { .in =  10, .upper = 10,  .lower = 10  },
        { .in = '0', .upper = '0', .lower = '0' },
        { .in = '@', .upper = '@', .lower = '@' },
        { .in = 'A', .upper = 'A', .lower = 'a' },
        { .in = 'B', .upper = 'B', .lower = 'b' },
        { .in = 'I', .upper = 'I', .lower = 'i' },
        { .in = 'Z', .upper = 'Z', .lower = 'z' },
        { .in = '[', .upper = '[', .lower = '[' },
        { .in = '`', .upper = '`', .lower = '`' },
        { .in = 'a', .upper = 'A', .lower = 'a' },
        { .in = 'c', .upper = 'C', .lower = 'c' },
        { .in = 'i', .upper = 'I', .lower = 'i' },
        { .in = 'z', .upper = 'Z', .lower = 'z' },
        { .in = '{', .upper = '{', .lower = '{' },
        { .in = 1000, .upper = 1000, .lower = 1000 },
    };
    // clang-format on

    for (size_t i = 0; i < sizeof(vector) / sizeof(struct test); i++) {
        CU_ASSERT(vector[i].upper == nl_toupper(vector[i].in));
        CU_ASSERT(vector[i].lower == nl_tolower(vector[i].in));
    }
}


void test_nl_strncasecmp()
{
    const char foo[] = "foo";
    CU_ASSERT(0 == nl_strncasecmp("a", "b", 0));
    CU_ASSERT(0 == nl_strncasecmp("a", "a", 1));
    CU_ASSERT(0 == nl_strncasecmp(foo, foo, 3));
    CU_ASSERT(0 == nl_strncasecmp(foo, "FOO", 1));
    CU_ASSERT(0 == nl_strncasecmp(foo, "FOO", 2));
    CU_ASSERT(0 == nl_strncasecmp(foo, "FOO", 3));
    CU_ASSERT(0 == nl_strncasecmp(foo, "FOO", 4));
    CU_ASSERT(0 > nl_strncasecmp("a", "bOO", 4));
    CU_ASSERT(0 < nl_strncasecmp("c", "bOO", 4));
    CU_ASSERT(0 > nl_strncasecmp("foo", "FOOd", 4));
    CU_ASSERT(0 == nl_strncasecmp("foo", "FOOd", 3));
    CU_ASSERT(0 != nl_strncasecmp("foo", "food", 4));
    CU_ASSERT(0 != nl_strncasecmp("caduceus", "caduceus stick", 14));
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("nl_strings.c tests", NULL, NULL);
    CU_add_test(*suite, "nl_toupper/lower() Tests", test_nl_tocase);
    CU_add_test(*suite, "nl_strncasecmp() Tests", test_nl_strncasecmp);
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

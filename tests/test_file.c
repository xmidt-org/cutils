/* SPDX-FileCopyrightText: 2008, 2021 Ed Rose, Weston Schmidt */
/* SPDX-License-Identifier: Apache-2.0 */
#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "file.h"

void test_freadall(void)
{
    int fd = 0;
    char filename[7] = "XXXXXX";
    const char *text = "Hello, world.";
    char *data = NULL;
    size_t len = 0;

    /* Seed the file */
    fd = mkstemp(filename);
    CU_ASSERT_FATAL(-1 != fd);
    CU_ASSERT_FATAL(strlen(text) == (size_t)write(fd, text, strlen(text)));
    CU_ASSERT_FATAL(0 == fsync(fd));

    /* Use the file & check the output. */
    CU_ASSERT_FATAL(0 == freadall(filename, 0, (void **)&data, &len));
    CU_ASSERT_FATAL(NULL != data);
    CU_ASSERT(strlen(text) == len);
    CU_ASSERT_NSTRING_EQUAL(text, data, strlen(text));
    free(data);
    data = NULL;
    len = 0;

    /* Specify a length larger than the file. */
    CU_ASSERT_FATAL(0 == freadall(filename, 100, (void **)&data, &len));
    CU_ASSERT_FATAL(NULL != data);
    CU_ASSERT(strlen(text) == len);
    CU_ASSERT_NSTRING_EQUAL(text, data, strlen(text));
    free(data);
    data = NULL;
    len = 0;

    /* Specify a length equal than the file. */
    CU_ASSERT_FATAL(0 == freadall(filename, strlen(text), (void **)&data, &len));
    CU_ASSERT_FATAL(NULL != data);
    CU_ASSERT(strlen(text) == len);
    CU_ASSERT_NSTRING_EQUAL(text, data, strlen(text));
    free(data);
    data = NULL;
    len = 0;

    /* Specify a length shorter than the file. */
    CU_ASSERT_FATAL(E2BIG == freadall(filename, 5, (void **)&data, &len));
    CU_ASSERT_FATAL(NULL == data);
    CU_ASSERT(0 == len);

    CU_ASSERT_FATAL(EINVAL == freadall("__invalid__", 0, (void **)&data, &len));

    close(fd);

    free(data);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("utils.c tests", NULL, NULL);
    CU_add_test(*suite, "freadall() Tests", test_freadall);
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

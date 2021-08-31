/* SPDX-FileCopyrightText: 2020 Neil Henning, et. al*/
/* SPDX-License-Identifier: Unlicense */

/*
 * Copied from https://github.com/sheredom/hashmap.h/blob/master/test/test.c
 * and modified to work in this build ecosystem.
 */
#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

static int set_context(void *const context, void *const element)
{
    *(int *)context = *(int *)element;
    return 1;
}


void test_create()
{
    hashmap_t h;

    CU_ASSERT(0 == hashmap_create(1, &h));
    hashmap_destroy(&h);
}

extern size_t num_to_pow2(size_t);

void test_create_boundary()
{
    hashmap_t h;

    CU_ASSERT(0 == hashmap_create(0, &h));
    hashmap_destroy(&h);

    CU_ASSERT(0 == hashmap_create(3, &h));
    hashmap_destroy(&h);

    CU_ASSERT(0 != hashmap_create(0x80000001, &h));
}

void test_put()
{
    hashmap_t h;
    int x = 42;
    int y = 13;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &x));
    CU_ASSERT(0 == hashmap_iterate(&h, set_context, &y));
    CU_ASSERT(x == y);

    hashmap_destroy(&h);
}


void test_put_twice()
{
    hashmap_t h;
    int x = 42;
    int y = 13;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &x));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &y));
    CU_ASSERT(1 == hashmap_num_entries(&h));
    CU_ASSERT(0 == hashmap_iterate(&h, set_context, &x));
    CU_ASSERT(x == y);

    hashmap_destroy(&h);
}


void test_get_exists()
{
    hashmap_t h;
    int x = 42;
    int *y;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &x));
    y = (int *)hashmap_get(&h, "foo", 3);
    CU_ASSERT(*y == x);

    hashmap_destroy(&h);
}


void test_get_does_not_exists()
{
    hashmap_t h;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(NULL == hashmap_get(&h, "foo", 3));

    hashmap_destroy(&h);
}


void test_remove()
{
    hashmap_t h;
    int x = 42;
    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &x));
    CU_ASSERT(0 == hashmap_remove(&h, "foo", 3));
    hashmap_destroy(&h);
}

void test_remove_and_return_key()
{
    /* The '&bar' portion of the string just uniques the constant from the 'foo'
     * used later. */
    const char *const key = "foo&bar";

    hashmap_t h;
    int x = 42;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, key, 3, &x));

    /* Use a new string here so that we definitely have a different pointer key
     * being provided. */
    CU_ASSERT(key == hashmap_remove_and_return_key(&h, "foo", 3));
    hashmap_destroy(&h);
}


static int early_exit(void *const context, void *const element)
{
    *(int *)context += 1;
    *(int *)element += 1;
    return 0;
}


void test_iterate_early_exit()
{
    hashmap_t h;
    int x[27] = { 0 };
    int total = 0;
    char s[27];
    char c;
    CU_ASSERT(0 == hashmap_create(1, &h));

    for (c = 'a'; c <= 'z'; c++) {
        s[c - 'a'] = c;
    }

    for (c = 'a'; c <= 'z'; c++) {
        const int index = c - 'a';
        CU_ASSERT(0 == hashmap_put(&h, s + index, 1, x + index));
    }

    CU_ASSERT(1 == hashmap_iterate(&h, early_exit, &total));
    CU_ASSERT(1 == total);

    total = 0;

    for (c = 'a'; c <= 'z'; c++) {
        const int index = c - 'a';
        CU_ASSERT(x[index] <= 1);
        if (x[index]) {
            total += 1;
        }
    }

    CU_ASSERT(1 == total);

    hashmap_destroy(&h);
}


static int all(void *const context, void *const element)
{
    *(int *)context += 1;
    *(int *)element += 1;
    return 1;
}

void test_iterate_all()
{
    hashmap_t h;
    int x[27] = { 0 };
    int total = 0;
    char s[27];
    char c;
    CU_ASSERT(0 == hashmap_create(1, &h));

    for (c = 'a'; c <= 'z'; c++) {
        s[c - 'a'] = c;
    }

    for (c = 'a'; c <= 'z'; c++) {
        const int index = c - 'a';
        CU_ASSERT(0 == hashmap_put(&h, s + index, 1, x + index));
    }

    CU_ASSERT(0 == hashmap_iterate(&h, all, &total));
    CU_ASSERT(26 == total);

    for (c = 'a'; c <= 'z'; c++) {
        const int index = c - 'a';
        CU_ASSERT(1 == x[index]);
    }

    hashmap_destroy(&h);
}

void test_num_entries()
{
    hashmap_t h;
    int x = 42;
    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_num_entries(&h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &x));
    CU_ASSERT(1 == hashmap_num_entries(&h));
    CU_ASSERT(0 == hashmap_remove(&h, "foo", 3));
    CU_ASSERT(0 == hashmap_num_entries(&h));
    hashmap_destroy(&h);
}

static int rem_all(void *context, struct hashmap_element *e)
{
    (*(int *)context) += e->key_len;
    return -1;
}

void test_remove_all()
{
    hashmap_t h;
    int x[27] = { 0 };
    int total = 0;
    char s[27];
    char c;

    CU_ASSERT(0 == hashmap_create(16, &h));

    for (c = 'a'; c <= 'z'; c++) {
        s[c - 'a'] = c;
    }

    for (c = 'a'; c <= 'z'; c++) {
        const int index = c - 'a';
        CU_ASSERT(0 == hashmap_put(&h, s + index, 1, x + index));
    }
    CU_ASSERT(26 == hashmap_num_entries(&h));
    CU_ASSERT(0 == hashmap_iterate_pairs(&h, rem_all, &total));
    CU_ASSERT(26 == total);
    CU_ASSERT(0 == hashmap_num_entries(&h));
    hashmap_destroy(&h);
}


void test_hash_conflict()
{
    hashmap_t h;

    int x = 42;
    int y = 13;
    int z = -53;

    CU_ASSERT(0 == hashmap_create(4, &h));

    // These all hash to the same value.
    CU_ASSERT(0 == hashmap_put(&h, "000", 3, &x));
    CU_ASSERT(0 == hashmap_put(&h, "002", 3, &y));
    CU_ASSERT(0 == hashmap_put(&h, "007", 3, &z));
    CU_ASSERT(3 == hashmap_num_entries(&h));

    // Now we remove the middle value.
    CU_ASSERT(0 == hashmap_remove(&h, "002", 3));
    CU_ASSERT(2 == hashmap_num_entries(&h));

    // And now attempt to insert the last value again. There was a bug where this
    // would insert a new entry incorrectly instead of resolving to the previous
    // entry.
    CU_ASSERT(0 == hashmap_put(&h, "007", 3, &z));
    CU_ASSERT(2 == hashmap_num_entries(&h));

    hashmap_destroy(&h);
}

void test_issue_20()
{
    hashmap_t h;
    const char *key = "192.168.2.2hv_api.udache.com/abc/def";
    unsigned int len = sizeof(key) - 1;

    char *value = "1";
    char *ptr = NULL;

    hashmap_create(1024, &h);
    hashmap_put(&h, key, len, value);

    ptr = (char *)hashmap_get(&h, key, len);

    CU_ASSERT(value == ptr);

    hashmap_destroy(&h);
}


void simple_test()
{
    hashmap_t h;
    int z = 5;
    int y = 4;
    int x = 3;
    int *got;

    CU_ASSERT(0 == hashmap_create(1, &h));
    CU_ASSERT(0 == hashmap_put(&h, "foo", 3, &z));
    CU_ASSERT(0 == hashmap_put(&h, "bar", 3, &y));
    CU_ASSERT(0 == hashmap_put(&h, "foobar", 6, &x));

    got = hashmap_get(&h, "foo", 3);
    CU_ASSERT(*got == 5);
    got = hashmap_get(&h, "bar", 3);
    CU_ASSERT(*got == 4);
    got = hashmap_get(&h, "foobar", 6);
    CU_ASSERT(*got == 3);

    hashmap_destroy(&h);
}

void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("hashmap.c tests", NULL, NULL);
    CU_add_test(*suite, "hashmap_create() Test", test_create);
    CU_add_test(*suite, "hashmap_create() Test (boundary)", test_create_boundary);
    CU_add_test(*suite, "hashmap_put() Test", test_put);
    CU_add_test(*suite, "hashmap_put() Test (same key 2x)", test_put_twice);
    CU_add_test(*suite, "hashmap_get() Test (exists)", test_get_exists);
    CU_add_test(*suite, "hashmap_get() Test (does not exists)", test_get_does_not_exists);
    CU_add_test(*suite, "hashmap_remove() Test", test_remove);
    CU_add_test(*suite, "hashmap_remove_and_return_key() Test", test_remove_and_return_key);
    CU_add_test(*suite, "hashmap_iterate() Test (early exit)", test_iterate_early_exit);
    CU_add_test(*suite, "hashmap_iterate() Test (all)", test_iterate_all);
    CU_add_test(*suite, "hashmap_num_entries() Test", test_num_entries);
    CU_add_test(*suite, "Test remove all", test_remove_all);
    CU_add_test(*suite, "Test hash conflict", test_hash_conflict);
    CU_add_test(*suite, "Test issue 20", test_issue_20);
    CU_add_test(*suite, "Simple Test", simple_test);
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

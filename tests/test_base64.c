/* SPDX-FileCopyrightText: 2016-2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "memory.h"

struct test_vector {
    int opts;
    int rv;
    size_t in_len;
    const char *in;
    size_t out_len;
    const char *out;
};

// clang-format off
struct test_vector common_decoder_tests[] = {
        /* Simple NULL, string length tests */
        { .rv =  0, .in_len = 0, .in = NULL      },
        { .rv =  0, .in_len = 0, .in = "asdf123" },

        /* The length must be at least 2 */
        { .rv = -2, .in_len = 1, .in = "a" },

        /* Every character must be valid */
        { .rv = -1, .in_len = 8, .in = "asdf1\xffjj" },
        { .rv = -1, .in_len = 8, .in = "asdf1=jj"    },

        /* Protect against a bogus empty string */
        { .rv = -2, .in_len = 2, .in = "==" },

        /* Invalid, safely fail. */
        { .rv = -2, .in_len = 3, .in = "b==" },

        /* Invalid, padding. */
        { .rv = -2, .in_len = 3, .in = "ba="    },
        { .rv = -2, .in_len = 5, .in = "bad=="  },
        { .rv = -2, .in_len = 6, .in = "bad4==" },
        { .rv = -2, .in_len = 5, .in = "bad4="  },

        /* Disallow other printable charcters. */
        { .rv = -1, .in_len = 4, .in = "as d" },
        { .rv = -1, .in_len = 4, .in = "as!d" },
        { .rv = -1, .in_len = 4, .in = "as\"d" },
        { .rv = -1, .in_len = 4, .in = "as#d" },
        { .rv = -1, .in_len = 4, .in = "as$d" },
        { .rv = -1, .in_len = 4, .in = "as%d" },
        { .rv = -1, .in_len = 4, .in = "as&d" },
        { .rv = -1, .in_len = 4, .in = "as'd" },
        { .rv = -1, .in_len = 4, .in = "as(d" },
        { .rv = -1, .in_len = 4, .in = "as)d" },
        { .rv = -1, .in_len = 4, .in = "as*d" },
        { .rv = -1, .in_len = 4, .in = "as.d" },
        { .rv = -1, .in_len = 4, .in = "as:d" },
        { .rv = -1, .in_len = 4, .in = "as;d" },
        { .rv = -1, .in_len = 4, .in = "as<d" },
        { .rv = -1, .in_len = 4, .in = "as>d" },
        { .rv = -1, .in_len = 4, .in = "as?d" },
        { .rv = -1, .in_len = 4, .in = "as@d" },
        { .rv = -1, .in_len = 4, .in = "as[d" },
        { .rv = -1, .in_len = 4, .in = "as\\d" },
        { .rv = -1, .in_len = 4, .in = "as]d" },
        { .rv = -1, .in_len = 4, .in = "as^d" },
        { .rv = -1, .in_len = 4, .in = "as`d" },
        { .rv = -1, .in_len = 4, .in = "as{d" },
        { .rv = -1, .in_len = 4, .in = "as|d" },
        { .rv = -1, .in_len = 4, .in = "as}d" },
        { .rv = -1, .in_len = 4, .in = "as~d" },

        /* Disallow other non-printable charcters. */
        { .rv = -1, .in_len = 4, .in = "bs\x00p" },
        { .rv = -1, .in_len = 4, .in = "bs\x01p" },
        { .rv = -1, .in_len = 4, .in = "bs\x02p" },
        { .rv = -1, .in_len = 4, .in = "bs\x03p" },
        { .rv = -1, .in_len = 4, .in = "bs\x04p" },
        { .rv = -1, .in_len = 4, .in = "bs\x05p" },
        { .rv = -1, .in_len = 4, .in = "bs\x06p" },
        { .rv = -1, .in_len = 4, .in = "bs\x07p" },
        { .rv = -1, .in_len = 4, .in = "bs\x08p" },
        { .rv = -1, .in_len = 4, .in = "bs\x09p" },
        { .rv = -1, .in_len = 4, .in = "bs\x0ap" },

        /* A remainder of 1 is never valid */
        { .rv = -2, .in_len = 5, .in = "TWFub" },

        /* Simple valid examples */
        { .rv =  0, .in_len = 4, .in = "TWFu", .out_len = 3, .out = "Man" },
        { .rv =  0, .in_len = 4, .in = "TWE=", .out_len = 2, .out = "Ma" },
        { .rv =  0, .in_len = 4, .in = "TQ==", .out_len = 1, .out = "M" },

        /* A longer sample text. */
        {   .rv = 0,
            .in_len = 360,
            .in = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24s"
                  "IGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmlt"
                  "YWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBw"
                  "ZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBp"
                  "bmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRz"
                  "IHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=",
            .out_len = 269,
            .out = "Man is distinguished, not only by his reason, but by this "
                   "singular passion from other animals, which is a lust of "
                   "the mind, that by a perseverance of delight in the "
                   "continued and indefatigable generation of knowledge, "
                   "exceeds the short vehemence of any carnal pleasure.",
        },
    };

// clang-format on

void test_encoded_size()
{
    struct test {
        size_t len;
        size_t url_len;
        size_t std_len;
    } tests[] = {
        // clang-format off
        { .len =  0, .url_len =  0, .std_len =  0 },
        { .len =  1, .url_len =  2, .std_len =  4 },
        { .len =  2, .url_len =  3, .std_len =  4 },
        { .len =  3, .url_len =  4, .std_len =  4 },
        { .len =  4, .url_len =  6, .std_len =  8 },
        { .len = 35, .url_len = 47, .std_len = 48 },
    };
    const char *foo = "123456789abcdefghijklmnopqrstuvwxyz";

    for( size_t i = 0; i < sizeof(tests)/sizeof(struct test); i++) {
        size_t len = 10;
        CU_ASSERT(0 == b64_encode(B64_STD, foo, tests[i].len, NULL, &len));
        CU_ASSERT(tests[i].std_len == len);

        len = 10;
        CU_ASSERT(0 == b64_encode(B64_URL, foo, tests[i].len, NULL, &len));
        CU_ASSERT(tests[i].url_len == len);
    }
}

void test_decoded_size()
{
    struct test {
        size_t length;
        size_t url;
        int url_rv;
        size_t std;
        int std_rv;
    } tests[] = {
        // clang-format off
        { .length =  0, .url =  0, .url_rv =  0, .std =  0, .std_rv =  0 },
        { .length =  1, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length =  2, .url =  1, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length =  3, .url =  2, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length =  4, .url =  3, .url_rv =  0, .std =  3, .std_rv =  0 },
        { .length =  5, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length =  6, .url =  4, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length =  7, .url =  5, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length =  8, .url =  6, .url_rv =  0, .std =  6, .std_rv =  0 },
        { .length =  9, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 10, .url =  7, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 11, .url =  8, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 12, .url =  9, .url_rv =  0, .std =  9, .std_rv =  0 },
        { .length = 13, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 14, .url = 10, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 15, .url = 11, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 16, .url = 12, .url_rv =  0, .std = 12, .std_rv =  0 },
        { .length = 17, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 18, .url = 13, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 19, .url = 14, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 20, .url = 15, .url_rv =  0, .std = 15, .std_rv =  0 },
        { .length = 21, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 22, .url = 16, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 23, .url = 17, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 24, .url = 18, .url_rv =  0, .std = 18, .std_rv =  0 },
        { .length = 25, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 26, .url = 19, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 27, .url = 20, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 28, .url = 21, .url_rv =  0, .std = 21, .std_rv =  0 },
        { .length = 29, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 30, .url = 22, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 31, .url = 23, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 32, .url = 24, .url_rv =  0, .std = 24, .std_rv =  0 },
        { .length = 33, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 34, .url = 25, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 35, .url = 26, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 36, .url = 27, .url_rv =  0, .std = 27, .std_rv =  0 },
        { .length = 37, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 38, .url = 28, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 39, .url = 29, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 40, .url = 30, .url_rv =  0, .std = 30, .std_rv =  0 },
        { .length = 41, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 42, .url = 31, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 43, .url = 32, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 44, .url = 33, .url_rv =  0, .std = 33, .std_rv =  0 },
        { .length = 45, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 46, .url = 34, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 47, .url = 35, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 48, .url = 36, .url_rv =  0, .std = 36, .std_rv =  0 },
        { .length = 49, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 50, .url = 37, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 51, .url = 38, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 52, .url = 39, .url_rv =  0, .std = 39, .std_rv =  0 },
        { .length = 53, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 54, .url = 40, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 55, .url = 41, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 56, .url = 42, .url_rv =  0, .std = 42, .std_rv =  0 },
        { .length = 57, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 58, .url = 43, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 59, .url = 44, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 60, .url = 45, .url_rv =  0, .std = 45, .std_rv =  0 },
        { .length = 61, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 62, .url = 46, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 63, .url = 47, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 64, .url = 48, .url_rv =  0, .std = 48, .std_rv =  0 },
        { .length = 65, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 66, .url = 49, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 67, .url = 50, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 68, .url = 51, .url_rv =  0, .std = 51, .std_rv =  0 },
        { .length = 69, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 70, .url = 52, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 71, .url = 53, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 72, .url = 54, .url_rv =  0, .std = 54, .std_rv =  0 },
        { .length = 73, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 74, .url = 55, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 75, .url = 56, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 76, .url = 57, .url_rv =  0, .std = 57, .std_rv =  0 },
        { .length = 77, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 78, .url = 58, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 79, .url = 59, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 80, .url = 60, .url_rv =  0, .std = 60, .std_rv =  0 },
        { .length = 81, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 82, .url = 61, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 83, .url = 62, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 84, .url = 63, .url_rv =  0, .std = 63, .std_rv =  0 },
        { .length = 85, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 86, .url = 64, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 87, .url = 65, .url_rv =  0, .std =  0, .std_rv = -2 },
        { .length = 88, .url = 66, .url_rv =  0, .std = 66, .std_rv =  0 },
        { .length = 89, .url =  0, .url_rv = -2, .std =  0, .std_rv = -2 },
        { .length = 90, .url = 67, .url_rv =  0, .std =  0, .std_rv = -2 },
        // clang-format on
    };
    const char *foo = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                      "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    for (size_t i = 0; i < sizeof(tests) / sizeof(struct test); i++) {
        size_t len = 0;
        int rv;

        rv = b64_decode(B64_STD, foo, tests[i].length, NULL, &len);
        if (rv != tests[i].std_rv || len != tests[i].std) {
            printf("Standard %zd - Expected length: %zd, got: %zd, rv: %d, got: %d\n",
                   tests[i].length, tests[i].std, len, tests[i].std_rv, rv);
        }
        CU_ASSERT(tests[i].std_rv == rv);
        CU_ASSERT(tests[i].std == len);

        len = 0;
        rv = b64_decode(B64_URL, foo, tests[i].length, NULL, &len);
        if (rv != tests[i].url_rv || len != tests[i].url) {
            printf("URL %zd - Expected length: %zd, got: %zd, rv: %d, got: %d\n",
                   tests[i].length, tests[i].url, len, tests[i].url_rv, rv);
        }
        CU_ASSERT(tests[i].url_rv == rv);
        CU_ASSERT(tests[i].url == len);
    }
}


void test_decode_helper(int opts, const char *raw, size_t raw_size,
                        const char *expected, size_t expected_size,
                        int expected_rv)
{
    size_t result_size = 0;
    uint8_t *result = NULL;
    uint8_t *tmp = NULL;
    int rv = 0;
    char _buf[400];
    char *buf = (char *)&_buf;

    // Copy the data into a malloc'ed buffer so valgrind can help us find problems
    // Use it, then free it.
    tmp = memdup(raw, raw_size);
    rv = b64_decode(opts, tmp, raw_size, (void **)&result, &result_size);
    free(tmp);

    if (expected_rv != rv) {
        printf("rv expected: %d, got: %d\n", expected_rv, rv);
        printf("opts = %d, rv = %d, raw_size = %zd, raw = '%.*s', "
               "expected_size = %zd, expected = '%.*s'\n",
               opts, expected_rv, raw_size, (int)raw_size, raw,
               expected_size, (int)expected_size, expected);
    }
    CU_ASSERT_EQUAL_FATAL(expected_rv, rv);

    if (expected_size != result_size) {
        printf("Size fail: raw='%.*s' expected='%.*s' :: %zd %zd\n",
               (int)raw_size, raw, (int)expected_size, expected,
               expected_size, result_size);
    }
    CU_ASSERT_EQUAL_FATAL(expected_size, result_size);

    for (size_t i = 0; i < expected_size; i++) {
        CU_ASSERT_EQUAL(result[i], (uint8_t)expected[i]);
        if (result[i] != (uint8_t)expected[i]) {
            printf("Decoding Error: Expected[%zd:%c] '%*s' ::Actual[%zd:%c] '%*s'\n",
                   i, expected[i], (int)expected_size, expected,
                   i, result[i], (int)result_size, result);
        }
    }
    free(result);

    /* Do it again with a provided buffer */
    result_size = 400;
    rv = b64_decode(opts | B64_PROVIDED, raw, raw_size, (void **)&buf, &result_size);
    for (size_t i = 0; i < expected_size; i++) {
        if (buf[i] != expected[i]) {
            printf("Decoding Error: Expected[%zd:%c] '%.*s' ::Actual[%zd:%c] '%.*s'\n",
                   i, expected[i], (int)expected_size, expected,
                   i, buf[i], (int)result_size, buf);
            CU_ASSERT_EQUAL(buf[i], (uint8_t)expected[i]);
        }
    }
    //printf("expected_rv: %d, got: %d\n", expected_rv, rv);
    CU_ASSERT_EQUAL_FATAL(expected_rv, rv);
    //printf("expected_size: %zd, got: %zd\n", expected_size, result_size);
    CU_ASSERT_EQUAL_FATAL(expected_size, result_size);
}


void test_decode()
{
    struct test_vector *t = common_decoder_tests;
    // clang-format off
    struct test_vector local[] = {
        /* type    rv len, input         len, expected */
        { B64_STD,  0, 12, "bGVhc3VyZS4=", 8, "leasure."     },
        { B64_STD,  0, 12, "ZWFzdXJlLg==", 7, "easure."      },
        { B64_STD,  0, 8,  "YXN1cmUu",     6, "asure."       },
        { B64_STD,  0, 8,  "c3VyZS4=",     5, "sure."        },
        { B64_STD,  0, 4,  "+/+/",         3, "\xfb\xff\xbf" },
        { B64_STD, -2, 1,  "Y",            0, NULL           },
        { B64_STD, -2, 2,  "==",           0, NULL           },
        { B64_STD, -2, 3,  "b==",          0, NULL           },
        { B64_STD, -2, 5,  "bad4=",        0, NULL           },
        { B64_STD, -2, 6,  "bad4==",       0, NULL           },
        { B64_STD, -2, 5,  "bad==",        0, NULL           },
        { B64_STD, -2, 3,  "ba=",          0, NULL           },
        { B64_STD, -1, 8,  "YXN=cmUu",     0, NULL           },
        { B64_STD, -1, 8,  "YXN\xffmcUu",  0, NULL           },
        { B64_URL,  0, 3, "TWE",          2, "Ma"           },
        { B64_URL,  0, 2, "TQ",           1, "M"            },
        { B64_URL,  0,11, "bGVhc3VyZS4",  8, "leasure."     },
        { B64_URL,  0,10, "ZWFzdXJlLg",   7, "easure."      },
        { B64_URL,  0, 8, "YXN1cmUu",     6, "asure."       },
        { B64_URL,  0, 7, "c3VyZS4",      5, "sure."        },
        { B64_URL,  0, 4, "-_-_",         3, "\xfb\xff\xbf" },
        { B64_URL, -2, 1, "Y",            0, NULL           },
        { B64_URL, -1, 2, "==",           0, NULL           },
        { B64_URL, -1, 3, "b==",          0, NULL           },
        { B64_URL,  0,12, "ZWFzdXJlLg==", 7, "easure."      },
        { B64_URL, -2, 5, "bad4=",        0, NULL           },
        { B64_URL, -1, 6, "bad4==",       0, NULL           },
        { B64_URL, -2, 5, "bad==",        0, NULL           },
        { B64_URL, -1, 3, "ba=",          0, NULL           },
        { B64_URL, -1, 8, "YXN=cmUu",     0, NULL           },
        { B64_URL, -1, 8, "YXN\xffmcUu",  0, NULL           },

        /* Use every character to ensure they all map properly */
        {   .opts = B64_URL,
            .rv = 0,
            .in = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz"
                  "0123456789-_",
            .in_len = 64,
            .out = "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30"
                   "\xd3\x8f\x41\x14\x93\x51\x55\x97\x61\x96"
                   "\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7"
                   "\xa2\x9a\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3"
                   "\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf",
            .out_len = 48,
        },
        {   .opts = B64_STD,
            .rv = 0,
            .in = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "abcdefghijklmnopqrstuvwxyz"
                  "0123456789+/",
            .in_len = 64,
            .out = "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30"
                   "\xd3\x8f\x41\x14\x93\x51\x55\x97\x61\x96"
                   "\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7"
                   "\xa2\x9a\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3"
                   "\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf",
            .out_len = 48,
        },
    };
    // clang-format on

    for (size_t i = 0; i < sizeof(common_decoder_tests) / sizeof(struct test_vector); i++) {
        test_decode_helper(t[i].opts, t[i].in, t[i].in_len, t[i].out, t[i].out_len, t[i].rv);
    }

    t = local;
    for (size_t i = 0; i < sizeof(local) / sizeof(struct test_vector); i++) {
        test_decode_helper(t[i].opts, t[i].in, t[i].in_len, t[i].out, t[i].out_len, t[i].rv);
    }
}

void test_encode_helper(int opts, const char *raw, size_t raw_size,
                        const char *expected, size_t expected_size,
                        int expected_rv)
{
    size_t result_size = 0;
    char *result = NULL;
    uint8_t *tmp = NULL;
    int rv = 0;
    char _buf[100];
    char *buf = (char *)&_buf;

    // Copy the data into a malloc'ed buffer so valgrind can help us find problems
    // Use it, then free it.
    tmp = memdup(raw, raw_size);
    rv = b64_encode(opts, tmp, raw_size, &result, &result_size);
    free(tmp);

    if (expected_rv != rv) {
        printf("rv expected: %d, got: %d\n", expected_rv, rv);
        printf("opts = %d, rv = %d, raw_size = %zd, raw = '%.*s', "
               "expected_size = %zd, expected = '%.*s'\n",
               opts, expected_rv, raw_size, (int)raw_size, raw,
               expected_size, (int)expected_size, expected);
    }
    CU_ASSERT_EQUAL(expected_rv, rv);

    if (expected_size != result_size) {
        printf("Size fail: raw='%.*s' expected='%.*s' :: %zd %zd\n",
               (int)raw_size, raw, (int)expected_size, expected,
               expected_size, result_size);
    }
    CU_ASSERT_EQUAL(expected_size, result_size);

    for (size_t i = 0; i < expected_size; i++) {
        CU_ASSERT_EQUAL(result[i], (uint8_t)expected[i]);
        if (result[i] != (uint8_t)expected[i]) {
            printf("Encoding Error: Expected[%zd:%c] '%*s' ::Actual[%zd:%c] '%*s'\n",
                   i, expected[i], (int)expected_size, expected,
                   i, result[i], (int)result_size, result);
        }
    }
    free(result);

    /* Do it again with a provided buffer */
    result_size = 100;
    rv = b64_encode(opts | B64_PROVIDED, raw, raw_size, &buf, &result_size);
    CU_ASSERT_EQUAL(expected_rv, rv);
    CU_ASSERT_EQUAL(expected_size, result_size);
    for (size_t i = 0; i < expected_size; i++) {
        CU_ASSERT_EQUAL(buf[i], expected[i]);
        if (buf[i] != (uint8_t)expected[i]) {
            printf("Encoding Error: Expected[%zd:%c] '%*s' ::Actual[%zd:%c] '%*s'\n",
                   i, expected[i], (int)expected_size, expected,
                   i, buf[i], (int)result_size, buf);
        }
    }
}


void test_encode()
{
    // clang-format off
    struct test_vector local[] = {
        /* type    rv len, input         len, expected */
        { B64_STD,  0,  8, "leasure.",     12, "bGVhc3VyZS4=" },
        { B64_STD,  0,  7, "easure.",      12, "ZWFzdXJlLg==" },
        { B64_STD,  0,  6, "asure.",        8, "YXN1cmUu"     },
        { B64_STD,  0,  5, "sure.",         8, "c3VyZS4="     },
        { B64_STD,  0,  4, "ure.",          8, "dXJlLg=="     },
        { B64_STD,  0,  3, "re.",           4, "cmUu"         },
        { B64_STD,  0,  2, "e.",            4, "ZS4="         },
        { B64_STD,  0,  1, ".",             4, "Lg=="         },
        { B64_STD,  0,  0, "",              0, ""             },
        { B64_URL,  0,  8, "leasure.",     11, "bGVhc3VyZS4"  },
        { B64_URL,  0,  7, "easure.",      10, "ZWFzdXJlLg"   },
        { B64_URL,  0,  6, "asure.",        8, "YXN1cmUu"     },
        { B64_URL,  0,  5, "sure.",         7, "c3VyZS4"      },
        { B64_URL,  0,  4, "ure.",          6, "dXJlLg"       },
        { B64_URL,  0,  3, "re.",           4, "cmUu"         },
        { B64_URL,  0,  2, "e.",            3, "ZS4"          },
        { B64_URL,  0,  1, ".",             2, "Lg"           },
        { B64_URL,  0,  0, "",              0, ""             },
                /* Use every character to ensure they all map properly */
        {   .opts = B64_STD,
            .rv = 0,
            .out = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "abcdefghijklmnopqrstuvwxyz"
                   "0123456789+/",
            .out_len = 64,
            .in = "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30"
                  "\xd3\x8f\x41\x14\x93\x51\x55\x97\x61\x96"
                  "\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7"
                  "\xa2\x9a\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3"
                  "\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf",
            .in_len = 48,
        },
        {   .opts = B64_URL,
            .rv = 0,
            .out = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "abcdefghijklmnopqrstuvwxyz"
                   "0123456789-_",
            .out_len = 64,
            .in = "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30"
                  "\xd3\x8f\x41\x14\x93\x51\x55\x97\x61\x96"
                  "\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7"
                  "\xa2\x9a\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3"
                  "\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf",
            .in_len = 48,
        },
        // clang-format on
    };
    struct test_vector *t = local;

    for (size_t i = 0; i < sizeof(local) / sizeof(struct test_vector); i++) {
        test_encode_helper(t[i].opts, t[i].in, t[i].in_len, t[i].out, t[i].out_len, t[i].rv);
    }
}

void test_input_validation()
{
    size_t len = 9;
    char buf[10];
    char *b = (char *)&buf;
    char *in = "randomstring";

    CU_ASSERT(-5 == b64_decode(B64_STD, NULL, 0, NULL, NULL));

    len = 10;
    CU_ASSERT(-5 == b64_decode(5, NULL, 0, NULL, &len));
    CU_ASSERT(0 == len);

    len = 10;
    CU_ASSERT(0 == b64_decode(B64_STD | B64_PROVIDED, NULL, 0, NULL, &len));
    CU_ASSERT(0 == len);

    len = 3;
    CU_ASSERT(-3 == b64_decode(B64_STD | B64_PROVIDED, in, strlen(in), (void **)&b, &len));
    CU_ASSERT(0 == len);
}


void add_suites(CU_pSuite *suite)
{
    *suite = CU_add_suite("Base64 encoding tests", NULL, NULL);
    CU_add_test(*suite, "Test the Encoded Size     ", test_encoded_size);
    CU_add_test(*suite, "Test the Decoded Size     ", test_decoded_size);
    CU_add_test(*suite, "Test Encoding             ", test_encode);
    CU_add_test(*suite, "Test Decoding             ", test_decode);
    CU_add_test(*suite, "Test Input Validation     ", test_input_validation);
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

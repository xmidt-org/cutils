/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "base64.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define ENCODE 0
#define DECODE 1

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct b64_cfg {
    const char enc_map[65];
    const int8_t dec_map[256];
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
// clang-format off
static const struct b64_cfg b64_data[2] = {
    {
        /* Standard */
        .enc_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "abcdefghijklmnopqrstuvwxyz"
                   "0123456789+/=",
        // -1 = invalid
        // -2 = padding
        .dec_map = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x00-0x0f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x10-0x1f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, /* 0x20-0x2f */
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, /* 0x30-0x3f */
            -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 0x40-0x4f */
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 0x50-0x5f */
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 0x60-0x6f */
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* 0x70-0x7f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x80-0x8f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x90-0x9f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xa0-0xaf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xb0-0xbf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xc0-0xcf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xd0-0xdf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xe0-0xef */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xf0-0xff */
        },
    },
    {
        /* URL */
        .enc_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "abcdefghijklmnopqrstuvwxyz"
                   "0123456789-_\0",
        // -1 = invalid
        // -2 = padding
        .dec_map = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x00-0x0f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x10-0x1f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, /* 0x20-0x2f */
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1, /* 0x30-0x3f */
            -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 0x40-0x4f */
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 0x50-0x5f */
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 0x60-0x6f */
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* 0x70-0x7f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x80-0x8f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0x90-0x9f */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xa0-0xaf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xb0-0xbf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xc0-0xcf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xd0-0xdf */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xe0-0xef */
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0xf0-0xff */
        },
    },
};
// clang-format on

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static size_t get_enc_size(int type, size_t decoded_size)
{
    size_t remainder = 0;

    if (B64_STD == type) {
        return ((decoded_size + 2) / 3) * 4;
    }

    /* URL otherwise */

    remainder = decoded_size % 3;

    if (remainder) {
        remainder++;
    }

    return (decoded_size / 3) * 4 + remainder;
}


static size_t get_dec_size(int type, size_t encoded_size)
{
    size_t remainder = 0;
    size_t rv = 0;

    if (B64_STD == type) {
        rv = (encoded_size / 4) * 3;

        /* Check that the input is valid. */
        if (get_enc_size(type, rv) != encoded_size) {
            rv = 0;
        }

        return rv;
    }

    /* URL otherwise */

    remainder = 0x03 & encoded_size;

    if (remainder) {
        remainder--;
    }

    rv = (encoded_size / 4) * 3 + remainder;

    /* Check that the input is valid. */
    if (get_enc_size(type, rv) != encoded_size) {
        rv = 0;
    }

    return rv;
}


static int encode(const struct b64_cfg *cfg, const uint8_t *in, size_t len,
                  uint8_t *out, size_t *out_len)
{
    uint32_t bits = 0;
    int bit_count = 0;
    size_t j = 0;
    uint8_t *map = (uint8_t *)cfg->enc_map;

    for (size_t i = 0; i < len; i++) {
        bits = (bits << 8) | in[i];
        bit_count += 8;

        while (6 <= bit_count) {
            bit_count -= 6;
            if (out) {
                out[j] = map[0x3f & (bits >> bit_count)];
            }
            j++;
        }
    }

    /* Handle the extra bits. */
    if (bit_count) {
        bits <<= 8;
        bit_count += 8;
        bit_count -= 6;
        if (out) {
            out[j] = map[0x3f & (bits >> bit_count)];
        }
        j++;
    }

    /* Pad */
    while (('\0' != map[64]) && (0x03 & j)) {
        if (out) {
            out[j] = map[64];
        }
        j++;
    }

    *out_len = j;

    return 0;
}


static int decode(const struct b64_cfg *cfg, const uint8_t *in, size_t len,
                  uint8_t *out, size_t *out_len)
{

    uint32_t bits = 0;
    int bit_count = 0;
    size_t padding = 0;
    size_t j = 0;

    if ('=' == in[len - 1]) {
        padding++;
        if ('=' == in[len - 2]) {
            padding++;
        }

        /* If there is padding then it should only pad to ensure the string
         * has a multiple of 4.  Anything else is an error. */
        if (0 != (0x03 & len)) {
            *out_len = 0;
            return -1;
        }
    }

    len -= padding;

    for (size_t i = 0; i < len; i++) {
        int8_t val;

        val = cfg->dec_map[in[i]];
        if (val < 0) {
            *out_len = 0;
            return -1;
        }
        bits = (bits << 6) | val;
        bit_count += 6;

        if (8 <= bit_count) {
            if (out) {
                out[j] = (uint8_t)(0x0ff & (bits >> (bit_count - 8)));
            }
            j++;
            bit_count -= 8;
        }
    }

    *out_len = j;
    return 0;
}


int process(int opts,
            size_t (*get_size)(int, size_t),
            int (*transform)(const struct b64_cfg *, const uint8_t *, size_t, uint8_t *, size_t *out_len),
            const uint8_t *in, size_t in_len, uint8_t **_out, size_t *out_len)
{

    uint8_t *out = NULL;
    int type = 0;
    size_t dec_size = 0;
    int rv = 0;
    int alloced_buf = 0;

    if (NULL == out_len) {
        return -5;
    }

    /* Validate types */
    switch (0x0f & opts) {
    case B64_STD:
        type = B64_STD;
        break;
    case B64_URL:
        type = B64_URL;
        break;
    default:
        *out_len = 0;
        return -5;
    }

    /* If a NULL buffer or zero length is passed in, return successfully with
     * nothing allocated or done. */
    if (NULL == in || 0 == in_len) {
        if (_out && (B64_PROVIDED != (B64_PROVIDED & opts))) {
            *_out = NULL;
        }
        *out_len = 0;
        return 0;
    }

    /* Figure out the actual size */
    dec_size = (*get_size)(type, in_len);
    if (!dec_size) {
        *out_len = 0;
        return -2;
    }

    /* Allocate the buffer if needed */
    if (_out) {
        if (B64_PROVIDED == (B64_PROVIDED & opts)) {
            if (*out_len < dec_size) {
                *out_len = 0;
                return -3;
            }
            out = *_out;
        } else {
            alloced_buf = 1;
            out = calloc(dec_size, sizeof(uint8_t));
            if (!out) {
                return -4;
            }
        }
    }

    rv = (*transform)(&b64_data[type], in, in_len, out, out_len);

    if (alloced_buf) {
        if (0 != rv) {
            free(out);
        } else if (_out) {
            *_out = out;
        }
    }

    return rv;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


int b64_decode(int opts, const void *in, size_t in_len,
               void **out, size_t *out_len)
{
    return process(opts, get_dec_size, decode, (const uint8_t *)in, in_len,
                   (uint8_t **)out, out_len);
}


int b64_encode(int opts, const void *in, size_t in_len,
               char **out, size_t *out_len)
{
    return process(opts, get_enc_size, encode, (const uint8_t *)in, in_len,
                   (uint8_t **)out, out_len);
}

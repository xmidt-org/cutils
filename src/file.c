/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */


/*----------------------------------------------------------------------------*/
/*                             Internal Functions                             */
/*----------------------------------------------------------------------------*/
static int get_file_size(FILE *f, size_t *filesize)
{
    if (0 == fseek(f, 0, SEEK_END)) {
        long file_len = 0;

        file_len = ftell(f);
        if ((-1 != file_len) && (0 == fseek(f, 0, SEEK_SET))) {
            *filesize = (size_t)file_len;
            return 0;
        }
    }

    return errno;
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int freadall(const char *filename, size_t max, void **data, size_t *len)
{
    FILE *f = NULL;
    size_t file_len = 0;

    f = fopen(filename, "rb");
    if (!f) {
        return EINVAL;
    }

    if (get_file_size(f, &file_len)) {
        fclose(f);
        return errno;
    }

    if ((0 < max) && (max < file_len)) {
        fclose(f);
        return E2BIG;
    }

    *len = 0;
    *data = NULL;
    if (0 < file_len) {
        *data = malloc(file_len);
        if (*data) {
            char *p = (char *)*data;
            size_t want = file_len;
            size_t have = 0;

            while (!feof(f) && (0 < want)) {
                size_t got = fread(p, 1, want, f);
                p += got;
                have += got;
            }
            *len = have;
        }
    }
    fclose(f);

    return 0;
}

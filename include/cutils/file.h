/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __FILE_H__
#define __FILE_H__

#include <stddef.h>

/**
 * Reads the specified file into buffer and returns the buffer and length
 * as data & length.  If the max value is set larger than 0, then only proceed
 * if the file size is less than or equal to the max specified.
 *
 * @param filename the path and name of the file to read
 * @param max      the max file size supported, or 0 for no limit
 *                 will result in EFBIG returned as the return value
 * @param data     the resulting buffer
 * @param len      the size of the buffer
 *
 * @return 0 on success
 *         EFBIG if the file is larger than the max allowed
 *         EINVAL general error
 *         ESPIPE the filename is not a file
 */
int freadall(const char *filename, size_t max, void **data, size_t *len);

#endif


/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>

/**
 * Makes sure to free the pointer ptr in the event that the call to realloc()
 * fails.
 *
 * Note: See realloc() man pages for additional details.
 *
 * @param ptr the pointer to the memory to resize
 * @param size the new size of the memory block
 *
 * @return the pointer to the new memory block, or NULL
 */
void *saferealloc(void *ptr, size_t size);


/**
 * Duplicates a specified block of memory into a new buffer and returns the
 * buffer to the caller.  The new buffer must have free() called on it or it
 * will leak.
 *
 *
 * @param src the pointer to the memory to duplicate
 * @param len the number of bytes to dupliate
 *
 * @return the pointer to the new memory block, or NULL
 */
void *memdup(const void *src, size_t len);


/**
 * Appends a specified block of memory to an existing buffer and returns the
 * new buffer to the caller.  This call uses realloc, so the inputs must be
 * compatible with that call.
 *
 * @note: Resulting buffer is NOT nil terminated.
 * @note: If there is a malloc failure, the buffer being appended to is freed.
 *
 * @param buf     the buffer to append to
 * @param buf_len the length of the a buffer in, and the new buffer length out
 * @param append  the pointer to the memory to duplicate
 * @param len     the number of bytes to dupliate
 *
 * @return the new buffer pointer on success, NULL otherwise
 */
void *memappend(void **buf, size_t *buf_len, const void *src, size_t len);


/*-- Must versions -----------------------------------------------------------*/


/**
 *  A version of calloc that always returns a valid pointer from calloc.  If
 *  for some reason that request fails, abort() is called and the program will
 *  terminate.
 */
void *must_calloc(size_t nmemb, size_t size);


/**
 *  A version of realloc that always returns a valid pointer from realloc.  If
 *  for some reason that request fails, abort() is called and the program will
 *  terminate.  NULL can be returned if the size is 0.
 */
void *must_realloc(void *ptr, size_t size);


/**
 *  A version of memdup that always returns a valid pointer.  If for some reason
 *  that request fails, abort() is called and the program will terminate.  NULL
 *  can be returned if the len is 0.
 */
void *must_memdup(const void *src, size_t len);


/**
 *  A version of memappend that always returns a valid pointer.  If for some
 *  reason that request fails, abort() is called and the program will terminate.
 *  NULL can be returned if the resulting total length is 0.
 */
void *must_memappend(void **buf, size_t *buf_len, const void *src, size_t len);

#endif

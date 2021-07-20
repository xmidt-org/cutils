/* SPDX-FileCopyrightText: 2010-2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */
#ifndef __BASE64_L__
#define __BASE64_L__

#include <stddef.h>
#include <stdint.h>

/* The options to specify. */
// clang-format off
#define B64_STD         (0x00)
#define B64_URL         (0x01)
#define B64_PROVIDED    (0x80)
// clang-format on

/*----------------------------------------------------------------------------*/
/*                             Standard Base64                                */
/*----------------------------------------------------------------------------*/

/**
 *  Encodes the input into base64.  The base 64 produced string will be placed
 *  into the output param.  Consumers of this function are responsible for
 *  making sure the output buffer is able to hold all of the data once it is
 *  encoded.
 *
 *  TODO Fix the words
 *  @note: The output buffer must be large enough to handle the encoded payload.
 *  @note: The returned buffer must have free() called to prevent a memory leak.
 *  @note: The returned buffer is '\0' terminated.
 *  @note: The out_len value is equivalent to strlen() of the returned buffer.
 *
 *
 *  @param opts    the B64_STD or B64_URL form of the call to make
 *  @param in      pointer to the raw data
 *  @param in_len  size of the raw data in bytes
 *  @param out     pointer to where the encoded data should be placed or NULL
 *                 to just get the resulting length
 *  @param out_len pointer to the resulting lenght value
 *
 *  @retval 0 on success
 *  @retval -1 if there is an input error
 *  @retval -2 if the input buffer is not valid base64
 *  @retval -3 if the user specified buffer is too small
 *  @retval -4 if there is a memory allocation issue
 */
int b64_encode(int opts, const void *in, size_t in_len,
               char **out, size_t *out_len);


/**
 * Decodes the base64 encoded buffer.  The produced raw buffer will be placed
 * into the output array.  Consumers of this function are responsible for making
 * sure the output buffer is large enough to hold all of the decoded data.
 *
 * @note: The output buffer must be large enough to handle the decoded payload.
 *
 * @param enc  pointer to the encoded data
 * @param len  size of the raw data
 * @param out  pointer to where the decoded data should be placed
 *
 * @return total number of bytes in the decoded array, or 0 if there was a
 *         decoding error
 *
 *  @retval 0 on success
 *  @retval -1 if there is an input error
 *  @retval -2 if the input buffer is not valid base64
 *  @retval -3 if the user specified buffer is too small
 *  @retval -4 if there is a memory allocation issue
 */
int b64_decode(int opts, const void *in, size_t in_len,
               void **out, size_t *out_len);

#endif /* __BASE64_H__ */

/* SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MUST_H__
#define __MUST_H__

/**
 *  This simple function ensures the object passed in is not NULL or
 *  it calls abort().
 */
void *must(const void *const in);

#endif

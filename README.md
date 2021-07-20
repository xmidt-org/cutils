<!--
SPDX-FileCopyrightText: 2016-2021 Comcast Cable Communications Management, LLC
SPDX-License-Identifier: Apache-2.0
-->
# cutils

Common functions used in the xmidt-org programs.

[![Build Status](https://github.com/xmidt-org/cutils/workflows/CI/badge.svg)](https://github.com/xmidt-org/cutils/actions)
[![codecov.io](http://codecov.io/github/xmidt-org/cutils/coverage.svg?branch=main)](http://codecov.io/github/xmidt-org/cutils?branch=main)
[![coverity](https://img.shields.io/coverity/scan/23416.svg)](https://scan.coverity.com/projects/xmidt-org-cutils)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=xmidt-org_cutils&metric=alert_status)](https://sonarcloud.io/dashboard?id=xmidt-org_cutils)
[![Language Grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/xmidt-org/cutils.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/xmidt-org/cutils/context:cpp)
[![GitHub release](https://img.shields.io/github/release/xmidt-org/cutils.svg)](CHANGELOG.md)

Now that meson makes including subprojects easy, let's move common functions
into a shared library.

## What goes in here?

Generally annoying things you'd expect to just work.  Reliable `strnlen()`,
`strdup()`, `strndup()` are a few.  `maprintf()` that just outputs to a
correctly sized buffer you free (& the common friends).

There are some useful non-localized versions of string functions.  These
are important for libraries that deal with networking focused protocols.

There is a safer version of `realloc()` as well as `memdup()` ... because it
happens & you need to do it.

And a few other favorites: `xxd()` which is a debugger's dream for outputting
buffers.

Plus yet another base64 implementation for both `standard` as well as `url`
versions.

## What doesn't go in here?

If it doesn't feel like it's "missing" from a "standard library" then it goes
elsewhere.

# Building and Testing Instructions

```
meson build
cd build
ninja all test coverage
firefox meson-logs/coveragereport/index.html
```



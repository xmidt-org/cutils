<!--
SPDX-FileCopyrightText: 2021 Comcast Cable Communications Management, LLC
SPDX-License-Identifier: Apache-2.0
-->
# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [v2.1.0]
- Fix a documentation and implementation bug where iterating exited on a return
  value of 0 instead of continuing on zero.
- Add a new must() function to assert not NULL or abort() always.  This is
  different than assert() which is disabled in non-debug code.

## [v2.0.0]
- Update the hashmap api.
    - Change to use size_t vs. unsigned int.
    - Change struct hashmap_s to be a typedef hashmap_t.
    - Change struct hashmap_element_s to be struct hashmap_element.
    - Change the interface so calling hashmap_create() is optional if the
      structure is set to all '0'.
- Add tests for collisions and handling collision depth exhaustion.
- Make all the failures possible "gentle" and only hard fail if needed.
- Split out the crc code into it's own file for testing support.

## [v1.3.0]
- Add hashmap api.

## [v1.2.0]
- Add `must_` versions of the strdup functions.
- Fix a null pointer exception that could happen in must_mlvaprintf.

## [v1.1.0]
- Add `must_` versions of the memory and printf functions.

## [v1.0.0]
- Initial creation

[Unreleased]: https://github.com/xmidt-org/cutils/compare/v2.1.0...HEAD
[v2.1.0]: https://github.com/xmidt-org/cutils/compare/v2.0.0...v2.1.0
[v2.0.0]: https://github.com/xmidt-org/cutils/compare/v1.3.0...v2.0.0
[v1.3.0]: https://github.com/xmidt-org/cutils/compare/v1.2.0...v1.3.0
[v1.2.0]: https://github.com/xmidt-org/cutils/compare/v1.1.0...v1.2.0
[v1.1.0]: https://github.com/xmidt-org/cutils/compare/v1.0.0...v1.1.0
[v1.0.0]: https://github.com/xmidt-org/cutils/compare/de0878703659b82cd1bd1590b4e257119aacb650...v1.0.0

// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_GTEST_UTIL_H_
#define BASE_TEST_GTEST_UTIL_H_

#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

#if defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

#define EXPECT_CHECK_DEATH(statement) EXPECT_DEATH(statement, "Check failed")
#define EXPECT_CHECK_DEATH_WITH(statement, msg) EXPECT_DEATH(statement, msg)
#define ASSERT_CHECK_DEATH(statement) ASSERT_DEATH(statement, "Check failed")
#define EXPECT_NOTREACHED_DEATH(statement) \
  EXPECT_DEATH(statement, "NOTREACHED hit")
#define ASSERT_NOTREACHED_DEATH(statement) \
  ASSERT_DEATH(statement, "NOTREACHED hit")

#else  // defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

// Note GTEST_UNSUPPORTED_DEATH_TEST takes a |regex| only to see whether it is a
// valid regex. It is never evaluated.
#define EXPECT_CHECK_DEATH(statement) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", )
#define EXPECT_CHECK_DEATH_WITH(statement, msg) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", )
#define ASSERT_CHECK_DEATH(statement) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", return)
#define EXPECT_NOTREACHED_DEATH(statement) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", )
#define ASSERT_NOTREACHED_DEATH(statement) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", return)

#endif  // defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

// `BASE_EXPECT_DEATH` is similar to gtest's `EXPECT_DEATH_IF_SUPPORTED`. It
// takes into account that Android does not support them.
#if defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

#define BASE_EXPECT_DEATH EXPECT_DEATH

#else  // defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

#define BASE_EXPECT_DEATH(statement, matcher) \
  GTEST_UNSUPPORTED_DEATH_TEST(statement, "", )

#endif  // defined(GTEST_HAS_DEATH_TEST) && !BUILDFLAG(IS_ANDROID)

#endif  // BASE_TEST_GTEST_UTIL_H_

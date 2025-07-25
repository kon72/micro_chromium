// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/location.h"

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

namespace {

// This is a typical use: taking Location::Current as a default parameter.
// So even though this looks contrived, it confirms that such usage works as
// expected.
Location WhereAmI(const Location& location = Location::Current()) {
  return location;
}

}  // namespace

TEST(LocationTest, CurrentYieldsCorrectValue) {
  [[maybe_unused]] int previous_line = __LINE__;
  Location here = WhereAmI();
  EXPECT_NE(here.program_counter(), WhereAmI().program_counter());
  EXPECT_THAT(here.file_name(), ::testing::EndsWith("location_unittest.cc"));
  EXPECT_EQ(here.line_number(), previous_line + 1);
  EXPECT_STREQ("TestBody", here.function_name());
}

#if 0
TEST(LocationTest, TracingSupport) {
  EXPECT_EQ(perfetto::TracedValueToString(Location::CreateForTesting(
                "func", "file", 42, WhereAmI().program_counter())),
            "{function_name:func,file_name:file,line_number:42}");
}
#endif  // 0

}  // namespace base

// Copyright 2023 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/to_string.h"

#include <ios>
#include <ostream>
#include <string>

#include "base/strings/string_number_conversions.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace {

class NotStringifiable {};
class HasToString {
 public:
  std::string ToString() const { return "yay!"; }
};

// .ToString() support on structs.
static_assert(!internal::SupportsToString<NotStringifiable>,
              "value without ToString() shouldn't be marked SupportsToString");
static_assert(!internal::SupportsToString<NotStringifiable&>,
              "& without ToString() shouldn't be marked SupportsToString");
static_assert(!internal::SupportsToString<const NotStringifiable&>,
              "const& without ToString() shouldn't be marked SupportsToString");
static_assert(!internal::SupportsToString<NotStringifiable&&>,
              "&& without ToString() shouldn't be marked SupportsToString");
static_assert(internal::SupportsToString<HasToString>,
              "value with ToString() should be marked SupportsToString");
static_assert(internal::SupportsToString<HasToString&>,
              "& with ToString() should be marked SupportsToString");
static_assert(internal::SupportsToString<const HasToString&>,
              "const& with ToString() should be marked SupportsToString");
static_assert(internal::SupportsToString<HasToString&&>,
              "&& with ToString() should be marked SupportsToString");

// Booleans should stringify specifically as "true" and "false".
TEST(ToStringTest, Booleans) {
  EXPECT_EQ(ToString(true), "true");
  EXPECT_EQ(ToString(false), "false");
}

TEST(ToStringTest, Streamable) {
  // Types with built-in <<.
  EXPECT_EQ(ToString("foo"), "foo");
  EXPECT_EQ(ToString(123), "123");
}

enum class StreamableTestEnum { kGreeting, kLocation };

std::ostream& operator<<(std::ostream& os, const StreamableTestEnum& value) {
  switch (value) {
    case StreamableTestEnum::kGreeting:
      return os << "hello";
    case StreamableTestEnum::kLocation:
      return os << "world";
  }
}

TEST(ToStringTest, UserDefinedStreamable) {
  // Type with user-defined <<.
  EXPECT_EQ(ToString(StreamableTestEnum::kGreeting), "hello");
}

TEST(ToStringTest, UserDefinedToString) {
  // Type with user-defined ToString().
  EXPECT_EQ(ToString(HasToString()), "yay!");
}

class UnusualToString {
 public:
  HasToString ToString() const { return HasToString(); }
};

TEST(ToStringTest, ToStringReturnsNonStdString) {
  // Types with a ToString() that does not directly return a std::string should
  // still work.
  EXPECT_EQ(ToString(UnusualToString()), "yay!");
}

enum class NonStreamableTestEnum { kGreeting = 0, kLocation };

TEST(ToStringTest, ScopedEnum) {
  // Scoped enums without a defined << should print as their underlying type.
  EXPECT_EQ(ToString(NonStreamableTestEnum::kLocation), "1");
}

TEST(ToStringTest, WideChars) {
  EXPECT_EQ(ToString(u'a'), "97");
  EXPECT_EQ(ToString(L'a'), "97");
}

TEST(ToStringTest, Tuple) {
  // Tuples should correctly format the contained types.
  EXPECT_EQ(ToString(std::make_tuple(StreamableTestEnum::kGreeting,
                                     HasToString(), "a string")),
            "<hello, yay!, a string>");
}

void Func() {}

TEST(ToStringTest, FunctionPointer) {
  // We don't care about the actual address, but a function pointer should not
  // be implicitly converted to bool.
  EXPECT_NE(ToString(&Func), ToString(true));

  // Functions should be treated like function pointers.
  EXPECT_EQ(ToString(Func), ToString(&Func));
}

TEST(ToStringTest, Pointer) {
  int i = 42;
  std::string result_string = ToString(&i);

  ASSERT_GT(result_string.size(), 2);
#ifdef _MSVC_STL_VERSION
  EXPECT_NE(result_string.substr(0, 2), "0x");
#else
  EXPECT_EQ(result_string.substr(0, 2), "0x");
#endif

  // ... and whose contents is the hex representation of the value of the actual
  // pointer value.
  uint64_t result_int;
  ASSERT_TRUE(HexStringToUInt64(result_string, &result_int));
  EXPECT_EQ(reinterpret_cast<uintptr_t>(&i), result_int);
}

class OverloadsAddressOp {
 public:
  OverloadsAddressOp* operator&() { return nullptr; }
  const OverloadsAddressOp* operator&() const { return nullptr; }
};

TEST(ToStringTest, NonStringifiable) {
  // Non-stringifiable types should be printed using a fallback.
  EXPECT_NE(ToString(NotStringifiable()).find("-byte object at 0x"),
            std::string::npos);

  // Non-stringifiable types which overload operator& should print their real
  // address.
  EXPECT_NE(ToString(OverloadsAddressOp()),
            ToString(static_cast<OverloadsAddressOp*>(nullptr)));
}

TEST(ToStringTest, Span) {
  struct S {
    std::string ToString() const { return "S()"; }
  };

  EXPECT_EQ(ToString(span<const int>({1, 2, 3})), "[1, 2, 3]");
  EXPECT_EQ(ToString(span<const S>({S(), S()})), "[S(), S()]");
  EXPECT_EQ(ToString(span<const char>({'a', 'b', 'c'})), "[\"abc\"]");
  EXPECT_EQ(ToString(span<const char>({'a', 'b', 'c', '\0'})),
            std::string_view("[\"abc\0\"]", 8u));
  EXPECT_EQ(ToString(span<const char>({'a', 'b', '\0', 'c', '\0'})),
            std::string_view("[\"ab\0c\0\"]", 9u));
  EXPECT_EQ(ToString(span<int>()), "[]");
  EXPECT_EQ(ToString(span<char>()), "[\"\"]");

  EXPECT_EQ(ToString(span<const char16_t>({u'a', u'b', u'c'})), "[u\"abc\"]");
  EXPECT_EQ(ToString(span<const wchar_t>({L'a', L'b', L'c'})), "[L\"abc\"]");
}

}  // namespace
}  // namespace base

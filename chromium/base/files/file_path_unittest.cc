// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef UNSAFE_BUFFERS_BUILD
// TODO(crbug.com/40284755): Remove this and spanify to fix the errors.
#pragma allow_unsafe_buffers
#endif

#include "base/files/file_path.h"

#include <stddef.h>

#include <array>
#include <sstream>
#include <string_view>

#include "base/files/safe_base_name.h"
#include "base/strings/utf_ostream_operators.h"
#include "base/strings/utf_string_conversions.h"
#include "base/test/gtest_util.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"

#if BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
#include "base/test/scoped_locale.h"
#endif

// This macro helps test `FILE_PATH_LITERAL` macro expansion.
#define TEST_FILE "TestFile"

// This macro helps avoid wrapped lines in the test structs.
#define FPL(x) FILE_PATH_LITERAL(x)

// This macro constructs strings which can contain NULs.
#define FPS(x) FilePath::StringType(FPL(x), std::size(FPL(x)) - 1)

namespace base {

struct UnaryTestData {
  FilePath::StringViewType input;
  FilePath::StringViewType expected;
};

struct UnaryBooleanTestData {
  FilePath::StringViewType input;
  bool expected;
};

struct BinaryTestData {
  FilePath::StringViewType inputs[2];
  FilePath::StringViewType expected;
};

struct BinaryBooleanTestData {
  FilePath::StringViewType inputs[2];
  bool expected;
};

struct BinaryIntTestData {
  FilePath::StringViewType inputs[2];
  int expected;
};

struct UTF8TestData {
  FilePath::StringViewType native;
  std::string_view utf8;
};

// file_util winds up using autoreleased objects on the Mac, so this needs
// to be a PlatformTest
using FilePathTest = PlatformTest;

TEST_F(FilePathTest, DirName) {
  const auto cases = std::to_array<UnaryTestData>({
      {FPL(""), FPL(".")},
      {FPL("aa"), FPL(".")},
      {FPL("/aa/bb"), FPL("/aa")},
      {FPL("/aa/bb/"), FPL("/aa")},
      {FPL("/aa/bb//"), FPL("/aa")},
      {FPL("/aa/bb/ccc"), FPL("/aa/bb")},
      {FPL("/aa"), FPL("/")},
      {FPL("/aa/"), FPL("/")},
      {FPL("/"), FPL("/")},
      {FPL("//"), FPL("//")},
      {FPL("///"), FPL("/")},
      {FPL("aa/"), FPL(".")},
      {FPL("aa/bb"), FPL("aa")},
      {FPL("aa/bb/"), FPL("aa")},
      {FPL("aa/bb//"), FPL("aa")},
      {FPL("aa//bb//"), FPL("aa")},
      {FPL("aa//bb/"), FPL("aa")},
      {FPL("aa//bb"), FPL("aa")},
      {FPL("//aa/bb"), FPL("//aa")},
      {FPL("//aa/"), FPL("//")},
      {FPL("//aa"), FPL("//")},
      {FPL("///aa/"), FPL("/")},
      {FPL("///aa"), FPL("/")},
      {FPL("///"), FPL("/")},
      {FPL("0:"), FPL(".")},
      {FPL("@:"), FPL(".")},
      {FPL("[:"), FPL(".")},
      {FPL("`:"), FPL(".")},
      {FPL("{:"), FPL(".")},
      {FPL("\xB3:"), FPL(".")},
      {FPL("\xC5:"), FPL(".")},
      {FPL("/aa/../bb/cc"), FPL("/aa/../bb")},
#if BUILDFLAG(IS_WIN)
      {FPL("\x0143:"), FPL(".")},
#endif  // BUILDFLAG(IS_WIN)
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:"), FPL("c:")},
      {FPL("C:"), FPL("C:")},
      {FPL("A:"), FPL("A:")},
      {FPL("Z:"), FPL("Z:")},
      {FPL("a:"), FPL("a:")},
      {FPL("z:"), FPL("z:")},
      {FPL("c:aa"), FPL("c:")},
      {FPL("c:/"), FPL("c:/")},
      {FPL("c://"), FPL("c://")},
      {FPL("c:///"), FPL("c:/")},
      {FPL("c:/aa"), FPL("c:/")},
      {FPL("c:/aa/"), FPL("c:/")},
      {FPL("c:/aa/bb"), FPL("c:/aa")},
      {FPL("c:aa/bb"), FPL("c:aa")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("\\aa\\bb"), FPL("\\aa")},
      {FPL("\\aa\\bb\\"), FPL("\\aa")},
      {FPL("\\aa\\bb\\\\"), FPL("\\aa")},
      {FPL("\\aa\\bb\\ccc"), FPL("\\aa\\bb")},
      {FPL("\\aa"), FPL("\\")},
      {FPL("\\aa\\"), FPL("\\")},
      {FPL("\\"), FPL("\\")},
      {FPL("\\\\"), FPL("\\\\")},
      {FPL("\\\\\\"), FPL("\\")},
      {FPL("aa\\"), FPL(".")},
      {FPL("aa\\bb"), FPL("aa")},
      {FPL("aa\\bb\\"), FPL("aa")},
      {FPL("aa\\bb\\\\"), FPL("aa")},
      {FPL("aa\\\\bb\\\\"), FPL("aa")},
      {FPL("aa\\\\bb\\"), FPL("aa")},
      {FPL("aa\\\\bb"), FPL("aa")},
      {FPL("\\\\aa\\bb"), FPL("\\\\aa")},
      {FPL("\\\\aa\\"), FPL("\\\\")},
      {FPL("\\\\aa"), FPL("\\\\")},
      {FPL("aa\\..\\bb\\c"), FPL("aa\\..\\bb")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:\\"), FPL("c:\\")},
      {FPL("c:\\\\"), FPL("c:\\\\")},
      {FPL("c:\\\\\\"), FPL("c:\\")},
      {FPL("c:\\\\aa"), FPL("c:\\\\")},
      {FPL("c:\\\\\\aa"), FPL("c:\\")},
      {FPL("c:\\aa"), FPL("c:\\")},
      {FPL("c:\\aa\\"), FPL("c:\\")},
      {FPL("c:\\aa\\bb"), FPL("c:\\aa")},
      {FPL("c:aa\\bb"), FPL("c:aa")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.DirName();
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST_F(FilePathTest, BaseName) {
  const auto cases = std::to_array<UnaryTestData>({
      {FPL(""), FPL("")},
      {FPL("aa"), FPL("aa")},
      {FPL("/aa/bb"), FPL("bb")},
      {FPL("/aa/bb/"), FPL("bb")},
      {FPL("/aa/bb//"), FPL("bb")},
      {FPL("/aa/bb/ccc"), FPL("ccc")},
      {FPL("/aa"), FPL("aa")},
      {FPL("/"), FPL("/")},
      {FPL("//"), FPL("//")},
      {FPL("///"), FPL("/")},
      {FPL("aa/"), FPL("aa")},
      {FPL("aa/bb"), FPL("bb")},
      {FPL("aa/bb/"), FPL("bb")},
      {FPL("aa/bb//"), FPL("bb")},
      {FPL("aa//bb//"), FPL("bb")},
      {FPL("aa//bb/"), FPL("bb")},
      {FPL("aa//bb"), FPL("bb")},
      {FPL("//aa/bb"), FPL("bb")},
      {FPL("//aa/"), FPL("aa")},
      {FPL("//aa"), FPL("aa")},
      {FPL("0:"), FPL("0:")},
      {FPL("@:"), FPL("@:")},
      {FPL("[:"), FPL("[:")},
      {FPL("`:"), FPL("`:")},
      {FPL("{:"), FPL("{:")},
      {FPL("\xB3:"), FPL("\xB3:")},
      {FPL("\xC5:"), FPL("\xC5:")},
#if BUILDFLAG(IS_WIN)
      {FPL("\x0143:"), FPL("\x0143:")},
#endif  // BUILDFLAG(IS_WIN)
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:"), FPL("")},
      {FPL("C:"), FPL("")},
      {FPL("A:"), FPL("")},
      {FPL("Z:"), FPL("")},
      {FPL("a:"), FPL("")},
      {FPL("z:"), FPL("")},
      {FPL("c:aa"), FPL("aa")},
      {FPL("c:/"), FPL("/")},
      {FPL("c://"), FPL("//")},
      {FPL("c:///"), FPL("/")},
      {FPL("c:/aa"), FPL("aa")},
      {FPL("c:/aa/"), FPL("aa")},
      {FPL("c:/aa/bb"), FPL("bb")},
      {FPL("c:aa/bb"), FPL("bb")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("\\aa\\bb"), FPL("bb")},
      {FPL("\\aa\\bb\\"), FPL("bb")},
      {FPL("\\aa\\bb\\\\"), FPL("bb")},
      {FPL("\\aa\\bb\\ccc"), FPL("ccc")},
      {FPL("\\aa"), FPL("aa")},
      {FPL("\\"), FPL("\\")},
      {FPL("\\\\"), FPL("\\\\")},
      {FPL("\\\\\\"), FPL("\\")},
      {FPL("aa\\"), FPL("aa")},
      {FPL("aa\\bb"), FPL("bb")},
      {FPL("aa\\bb\\"), FPL("bb")},
      {FPL("aa\\bb\\\\"), FPL("bb")},
      {FPL("aa\\\\bb\\\\"), FPL("bb")},
      {FPL("aa\\\\bb\\"), FPL("bb")},
      {FPL("aa\\\\bb"), FPL("bb")},
      {FPL("\\\\aa\\bb"), FPL("bb")},
      {FPL("\\\\aa\\"), FPL("aa")},
      {FPL("\\\\aa"), FPL("aa")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:\\"), FPL("\\")},
      {FPL("c:\\\\"), FPL("\\\\")},
      {FPL("c:\\\\\\"), FPL("\\")},
      {FPL("c:\\aa"), FPL("aa")},
      {FPL("c:\\aa\\"), FPL("aa")},
      {FPL("c:\\aa\\bb"), FPL("bb")},
      {FPL("c:aa\\bb"), FPL("bb")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.BaseName();
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST_F(FilePathTest, Append) {
  const auto cases = std::to_array<BinaryTestData>({
      {{FPL(""), FPL("cc")}, FPL("cc")},
      {{FPL("."), FPL("ff")}, FPL("ff")},
      {{FPL("."), FPL("")}, FPL(".")},
      {{FPL("/"), FPL("cc")}, FPL("/cc")},
      {{FPL("/aa"), FPL("")}, FPL("/aa")},
      {{FPL("/aa/"), FPL("")}, FPL("/aa")},
      {{FPL("//aa"), FPL("")}, FPL("//aa")},
      {{FPL("//aa/"), FPL("")}, FPL("//aa")},
      {{FPL("//"), FPL("aa")}, FPL("//aa")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:"), FPL("a")}, FPL("c:a")},
      {{FPL("c:"), FPL("")}, FPL("c:")},
      {{FPL("c:/"), FPL("a")}, FPL("c:/a")},
      {{FPL("c://"), FPL("a")}, FPL("c://a")},
      {{FPL("c:///"), FPL("a")}, FPL("c:/a")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      // Append introduces the default separator character, so these test cases
      // need to be defined with different expected results on platforms that
      // use different default separator characters.
      {{FPL("\\"), FPL("cc")}, FPL("\\cc")},
      {{FPL("\\aa"), FPL("")}, FPL("\\aa")},
      {{FPL("\\aa\\"), FPL("")}, FPL("\\aa")},
      {{FPL("\\\\aa"), FPL("")}, FPL("\\\\aa")},
      {{FPL("\\\\aa\\"), FPL("")}, FPL("\\\\aa")},
      {{FPL("\\\\"), FPL("aa")}, FPL("\\\\aa")},
      {{FPL("/aa/bb"), FPL("cc")}, FPL("/aa/bb\\cc")},
      {{FPL("/aa/bb/"), FPL("cc")}, FPL("/aa/bb\\cc")},
      {{FPL("aa/bb/"), FPL("cc")}, FPL("aa/bb\\cc")},
      {{FPL("aa/bb"), FPL("cc")}, FPL("aa/bb\\cc")},
      {{FPL("a/b"), FPL("c")}, FPL("a/b\\c")},
      {{FPL("a/b/"), FPL("c")}, FPL("a/b\\c")},
      {{FPL("//aa"), FPL("bb")}, FPL("//aa\\bb")},
      {{FPL("//aa/"), FPL("bb")}, FPL("//aa\\bb")},
      {{FPL("\\aa\\bb"), FPL("cc")}, FPL("\\aa\\bb\\cc")},
      {{FPL("\\aa\\bb\\"), FPL("cc")}, FPL("\\aa\\bb\\cc")},
      {{FPL("aa\\bb\\"), FPL("cc")}, FPL("aa\\bb\\cc")},
      {{FPL("aa\\bb"), FPL("cc")}, FPL("aa\\bb\\cc")},
      {{FPL("a\\b"), FPL("c")}, FPL("a\\b\\c")},
      {{FPL("a\\b\\"), FPL("c")}, FPL("a\\b\\c")},
      {{FPL("\\\\aa"), FPL("bb")}, FPL("\\\\aa\\bb")},
      {{FPL("\\\\aa\\"), FPL("bb")}, FPL("\\\\aa\\bb")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:\\"), FPL("a")}, FPL("c:\\a")},
      {{FPL("c:\\\\"), FPL("a")}, FPL("c:\\\\a")},
      {{FPL("c:\\\\\\"), FPL("a")}, FPL("c:\\a")},
      {{FPL("c:\\"), FPL("")}, FPL("c:\\")},
      {{FPL("c:\\a"), FPL("b")}, FPL("c:\\a\\b")},
      {{FPL("c:\\a\\"), FPL("b")}, FPL("c:\\a\\b")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#else   // FILE_PATH_USES_WIN_SEPARATORS
      {{FPL("/aa/bb"), FPL("cc")}, FPL("/aa/bb/cc")},
      {{FPL("/aa/bb/"), FPL("cc")}, FPL("/aa/bb/cc")},
      {{FPL("aa/bb/"), FPL("cc")}, FPL("aa/bb/cc")},
      {{FPL("aa/bb"), FPL("cc")}, FPL("aa/bb/cc")},
      {{FPL("a/b"), FPL("c")}, FPL("a/b/c")},
      {{FPL("a/b/"), FPL("c")}, FPL("a/b/c")},
      {{FPL("//aa"), FPL("bb")}, FPL("//aa/bb")},
      {{FPL("//aa/"), FPL("bb")}, FPL("//aa/bb")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:/"), FPL("a")}, FPL("c:/a")},
      {{FPL("c:/"), FPL("")}, FPL("c:/")},
      {{FPL("c:/a"), FPL("b")}, FPL("c:/a/b")},
      {{FPL("c:/a/"), FPL("b")}, FPL("c:/a/b")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath root(cases[i].inputs[0]);
    FilePath::StringType leaf(cases[i].inputs[1]);
    FilePath observed_str = root.Append(leaf);
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed_str.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;
    FilePath observed_path = root.Append(FilePath(leaf));
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed_path.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;

    // TODO(erikkay): It would be nice to have a unicode test append value to
    // handle the case when AppendASCII is passed UTF8
#if BUILDFLAG(IS_WIN)
    std::string ascii = WideToUTF8(leaf);
#elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
    std::string ascii = leaf;
#endif
    observed_str = root.AppendASCII(ascii);
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed_str.value())
        << "i: " << i << ", root: " << root.value() << ", leaf: " << leaf;
  }
}

// Test UTF8 values within a file path.
class FilePathUTF8Test : public ::testing::TestWithParam<FilePath::StringType> {
 protected:
  FilePath::StringType testcase() const { return GetParam(); }
};

INSTANTIATE_TEST_SUITE_P(
    FilePathUTF8TestCases,
    FilePathUTF8Test,
    ::testing::Values(FPL("خطأ في عملية التحقق من التحديث 5555"),
                      FPL("अपडेट जांच में यह गड़बड़ी है 5555"),
                      FPL("更新確認エラー 5555。")));

TEST_P(FilePathUTF8Test, Test) {
  FilePath root(FPL(""));
  FilePath::StringType leaf(testcase());
  FilePath observed_str = root.Append(leaf);
  EXPECT_EQ(FilePath::StringType(testcase()), observed_str.value());
  FilePath observed_path = root.Append(FilePath(leaf));
  EXPECT_EQ(FilePath::StringType(testcase()), observed_path.value());

#if BUILDFLAG(IS_WIN)
  std::string utf8 = WideToUTF8(leaf);
#else
  std::string utf8 = leaf;
#endif
  EXPECT_DCHECK_DEATH({ observed_str = root.AppendASCII(utf8); });
  EXPECT_DCHECK_DEATH(
      { observed_str = root.InsertBeforeExtensionASCII(utf8); });
  EXPECT_DCHECK_DEATH({ observed_str = root.AddExtensionASCII(utf8); });

  observed_str = root.AppendUTF8(utf8);
  EXPECT_EQ(FilePath::StringType(testcase()), observed_str.value());

  observed_str = root.InsertBeforeExtensionUTF8(utf8);
  EXPECT_TRUE(observed_str.empty());

  observed_str = root.AddExtensionUTF8(utf8);
  EXPECT_TRUE(observed_str.empty());
}

class FilePathUTF8InvalidTest : public ::testing::TestWithParam<std::string> {
 protected:
  std::string invalid_utf8_value() const { return GetParam(); }
};

INSTANTIATE_TEST_SUITE_P(
    FilePathUTF8InvalidTestCases,
    FilePathUTF8InvalidTest,
    ::testing::Values(
        // Invalid encoding of U+1FFFE (0x8F instead of 0x9F)
        "\xF0\x8F\xBF\xBE",

        // Surrogate code points
        "\xED\xA0\x80\xED\xBF\xBF",
        "\xED\xA0\x8F",
        "\xED\xBF\xBF",

        // Overlong sequences
        "\xC0\x80",                  // U+0000
        "\xC1\x80\xC1\x81",          // "AB"
        "\xE0\x80\x80",              // U+0000
        "\xE0\x82\x80",              // U+0080
        "\xE0\x9F\xBF",              // U+07FF
        "\xF0\x80\x80\x8D",          // U+000D
        "\xF0\x80\x82\x91",          // U+0091
        "\xF0\x80\xA0\x80",          // U+0800
        "\xF0\x8F\xBB\xBF",          // U+FEFF (BOM)
        "\xF8\x80\x80\x80\xBF",      // U+003F
        "\xFC\x80\x80\x80\xA0\xA5",  // U+00A5

        // Beyond U+10FFFF (the upper limit of Unicode codespace)
        "\xF4\x90\x80\x80",          // U+110000
        "\xF8\xA0\xBF\x80\xBF",      // 5 bytes
        "\xFC\x9C\xBF\x80\xBF\x80",  // 6 bytes

        // BOM in UTF-16(BE|LE)
        "\xFE\xFF",
        "\xFF\xFE",

        // Strings in legacy encodings. We can certainly make up strings
        // in a legacy encoding that are valid in UTF-8, but in real data,
        // most of them are invalid as UTF-8.

        // cafe with U+00E9 in ISO-8859-1
        "caf\xE9",
        // U+AC00, U+AC001 in EUC-KR
        "\xB0\xA1\xB0\xA2",
        // U+4F60 U+597D in Big5
        "\xA7\x41\xA6\x6E",
        // "abc" with U+201[CD] in windows-125[0-8]
        // clang-format off
        "\x93" "abc\x94",
        // clang-format on
        // U+0639 U+064E U+0644 U+064E in ISO-8859-6
        "\xD9\xEE\xE4\xEE",
        // U+03B3 U+03B5 U+03B9 U+03AC in ISO-8859-7
        "\xE3\xE5\xE9\xDC"));

TEST_P(FilePathUTF8InvalidTest, Test) {
  FilePath root(FPL(""));
  FilePath observed_str;
  EXPECT_DCHECK_DEATH(
      { observed_str = root.AppendASCII(invalid_utf8_value()); });
  EXPECT_DCHECK_DEATH({
    observed_str = root.InsertBeforeExtensionASCII(invalid_utf8_value());
  });
  EXPECT_DCHECK_DEATH(
      { observed_str = root.AddExtensionASCII(invalid_utf8_value()); });

  EXPECT_DCHECK_DEATH(
      { observed_str = root.AppendUTF8(invalid_utf8_value()); });
  EXPECT_DCHECK_DEATH(
      { observed_str = root.InsertBeforeExtensionUTF8(invalid_utf8_value()); });
  EXPECT_DCHECK_DEATH(
      { observed_str = root.AddExtensionUTF8(invalid_utf8_value()); });
}

TEST_F(FilePathTest, StripTrailingSeparators) {
  const auto cases = std::to_array<UnaryTestData>({
      {FPL(""), FPL("")},
      {FPL("/"), FPL("/")},
      {FPL("//"), FPL("//")},
      {FPL("///"), FPL("/")},
      {FPL("////"), FPL("/")},
      {FPL("a/"), FPL("a")},
      {FPL("a//"), FPL("a")},
      {FPL("a///"), FPL("a")},
      {FPL("a////"), FPL("a")},
      {FPL("/a"), FPL("/a")},
      {FPL("/a/"), FPL("/a")},
      {FPL("/a//"), FPL("/a")},
      {FPL("/a///"), FPL("/a")},
      {FPL("/a////"), FPL("/a")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:"), FPL("c:")},
      {FPL("c:/"), FPL("c:/")},
      {FPL("c://"), FPL("c://")},
      {FPL("c:///"), FPL("c:/")},
      {FPL("c:////"), FPL("c:/")},
      {FPL("c:/a"), FPL("c:/a")},
      {FPL("c:/a/"), FPL("c:/a")},
      {FPL("c:/a//"), FPL("c:/a")},
      {FPL("c:/a///"), FPL("c:/a")},
      {FPL("c:/a////"), FPL("c:/a")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("\\"), FPL("\\")},
      {FPL("\\\\"), FPL("\\\\")},
      {FPL("\\\\\\"), FPL("\\")},
      {FPL("\\\\\\\\"), FPL("\\")},
      {FPL("a\\"), FPL("a")},
      {FPL("a\\\\"), FPL("a")},
      {FPL("a\\\\\\"), FPL("a")},
      {FPL("a\\\\\\\\"), FPL("a")},
      {FPL("\\a"), FPL("\\a")},
      {FPL("\\a\\"), FPL("\\a")},
      {FPL("\\a\\\\"), FPL("\\a")},
      {FPL("\\a\\\\\\"), FPL("\\a")},
      {FPL("\\a\\\\\\\\"), FPL("\\a")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("c:\\"), FPL("c:\\")},
      {FPL("c:\\\\"), FPL("c:\\\\")},
      {FPL("c:\\\\\\"), FPL("c:\\")},
      {FPL("c:\\\\\\\\"), FPL("c:\\")},
      {FPL("c:\\a"), FPL("c:\\a")},
      {FPL("c:\\a\\"), FPL("c:\\a")},
      {FPL("c:\\a\\\\"), FPL("c:\\a")},
      {FPL("c:\\a\\\\\\"), FPL("c:\\a")},
      {FPL("c:\\a\\\\\\\\"), FPL("c:\\a")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.StripTrailingSeparators();
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}

TEST_F(FilePathTest, IsAbsoluteOrNetwork) {
  struct Cases {
    FilePath::StringViewType input;
    bool expected_is_absolute;
    bool expected_is_network;
  };
  const auto cases = std::to_array<Cases>({
      {FPL(""), false, false},       {FPL("a"), false, false},
      {FPL("c:"), false, false},     {FPL("c:a"), false, false},
      {FPL("a/b"), false, false},    {FPL("//"), true, true},
      {FPL("//a"), true, true},      {FPL("c:a/b"), false, false},
      {FPL("?:/a"), false, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("/"), false, false},      {FPL("/a"), false, false},
      {FPL("/."), false, false},     {FPL("/.."), false, false},
      {FPL("c:/"), true, false},     {FPL("c:/a"), true, false},
      {FPL("c:/."), true, false},    {FPL("c:/.."), true, false},
      {FPL("C:/a"), true, false},    {FPL("d:/a"), true, false},
#else   // FILE_PATH_USES_DRIVE_LETTERS
      {FPL("/"), true, false},
      {FPL("/a"), true, false},
      {FPL("/."), true, false},
      {FPL("/.."), true, false},
      {FPL("c:/"), false, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("a\\b"), false, false},   {FPL("\\\\"), true, true},
      {FPL("\\\\a"), true, true},    {FPL("a\\b"), false, false},
      {FPL("\\\\"), true, true},     {FPL("//a"), true, true},
      {FPL("c:a\\b"), false, false}, {FPL("?:\\a"), false, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("\\"), false, false},     {FPL("\\a"), false, false},
      {FPL("\\."), false, false},    {FPL("\\.."), false, false},
      {FPL("c:\\"), true, false},    {FPL("c:\\"), true, false},
      {FPL("c:\\a"), true, false},   {FPL("c:\\."), true, false},
      {FPL("c:\\.."), true, false},  {FPL("C:\\a"), true, false},
      {FPL("d:\\a"), true, false},
#else   // FILE_PATH_USES_DRIVE_LETTERS
      {FPL("\\"), true, false},      {FPL("\\a"), true, false},
      {FPL("\\."), true, false},     {FPL("\\.."), true, false},
      {FPL("c:\\"), false, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    bool observed_is_absolute = input.IsAbsolute();
    EXPECT_EQ(cases[i].expected_is_absolute, observed_is_absolute)
        << "i: " << i << ", input: " << input.value();
    bool observed_is_network = input.IsNetwork();
    EXPECT_EQ(cases[i].expected_is_network, observed_is_network)
        << "i: " << i << ", input: " << input.value();
  }
}

TEST_F(FilePathTest, PathComponentsTest) {
  const auto cases = std::to_array<UnaryTestData>({
      {FPL("//foo/bar/baz/"), FPL("|//|foo|bar|baz")},
      {FPL("///"), FPL("|/")},
      {FPL("///foo//bar/baz"), FPL("|/|foo|bar|baz")},
      {FPL("/foo//bar//baz/"), FPL("|/|foo|bar|baz")},
      {FPL("/foo/bar/baz/"), FPL("|/|foo|bar|baz")},
      {FPL("/foo/bar/baz//"), FPL("|/|foo|bar|baz")},
      {FPL("/foo/bar/baz///"), FPL("|/|foo|bar|baz")},
      {FPL("/foo/bar/baz"), FPL("|/|foo|bar|baz")},
      {FPL("/foo/bar.bot/baz.txt"), FPL("|/|foo|bar.bot|baz.txt")},
      {FPL("//foo//bar/baz"), FPL("|//|foo|bar|baz")},
      {FPL("/"), FPL("|/")},
      {FPL("foo"), FPL("|foo")},
      {FPL(""), FPL("")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {FPL("e:/foo"), FPL("|e:|/|foo")},
      {FPL("e:/"), FPL("|e:|/")},
      {FPL("e:foo"), FPL("|e:|foo")},
      {FPL("e:"), FPL("|e:")},
      {FPL("e://foo"), FPL("|e:|//|foo")},
      {FPL("e://"), FPL("|e:|//")},
      {FPL("e:///foo"), FPL("|e:|/|foo")},
      {FPL("e:///"), FPL("|e:|/")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("../foo"), FPL("|..|foo")},
      {FPL("./foo"), FPL("|foo")},
      {FPL("../foo/bar/"), FPL("|..|foo|bar")},
      {FPL("\\\\foo\\bar\\baz\\"), FPL("|\\\\|foo|bar|baz")},
      {FPL("\\\\\\"), FPL("|\\")},
      {FPL("\\foo\\\\bar\\\\baz\\"), FPL("|\\|foo|bar|baz")},
      {FPL("\\foo\\bar\\baz\\"), FPL("|\\|foo|bar|baz")},
      {FPL("\\foo\\bar\\baz\\\\"), FPL("|\\|foo|bar|baz")},
      {FPL("\\foo\\bar\\baz\\\\\\"), FPL("|\\|foo|bar|baz")},
      {FPL("\\foo\\bar\\baz"), FPL("|\\|foo|bar|baz")},
      {FPL("\\foo\\bar/baz\\\\\\"), FPL("|\\|foo|bar|baz")},
      {FPL("/foo\\bar\\baz"), FPL("|/|foo|bar|baz")},
      {FPL("\\foo\\bar.bot\\baz.txt"), FPL("|\\|foo|bar.bot|baz.txt")},
      {FPL("\\\\foo\\\\bar\\baz"), FPL("|\\\\|foo|bar|baz")},
      {FPL("\\"), FPL("|\\")},
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    std::vector<FilePath::StringType> comps = input.GetComponents();

    FilePath::StringType observed;
    for (const auto& j : comps) {
      observed.append(FILE_PATH_LITERAL("|"), 1);
      observed.append(j);
    }
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed)
        << "i: " << i << ", input: " << input.value();
  }
}

void IsParentTest(bool is_fast) {
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
  auto swap_separators = [](const FilePath& path) {
    FilePath::StringType new_path(path.value());
    for (auto& character : new_path) {
      if (character == '/') {
        character = '\\';
      } else if (character == '\\') {
        character = '/';
      }
    }
    return new_path;
  };
#endif  // defined(FILE_PATH_USES_WIN_SEPARATORS)

  const auto cases = std::to_array<BinaryBooleanTestData>({
      {{FPL(""), FPL("")}, false},
      {{FPL(""), FPL(".")}, false},
      {{FPL(""), FPL("foo")}, false},
      {{FPL("."), FPL("")}, false},
      {{FPL("."), FPL(".")}, false},
      // While the 2 cases below are incorrect, they verify that an old behavior
      // of IsParent() is preserved. We should consider modifying the code so
      // that the result is `true` for the 2 cases below.
      {{FPL("."), FPL("./foo")}, false},
      {{FPL("./"), FPL("./foo")}, false},
      {{FPL("./"), FPL("/")}, false},
      {{FPL("./"), FPL("//")}, false},
      {{FPL("./"), FPL("/foo")}, false},
      {{FPL("./"), FPL("//foo")}, false},
      {{FPL("./foo"), FPL("foo/bar")}, true},
      {{FPL("./foo"), FPL("./foo/bar")}, true},
      {{FPL("./foo"), FPL("/foo/bar")}, false},
      {{FPL("./foo"), FPL("//foo/bar")}, false},
      {{FPL("/"), FPL("/")}, false},
      {{FPL("/"), FPL("//foo")}, false},
      {{FPL("/"), FPL("///foo")}, true},
      {{FPL("/"), FPL("/foo/bar/baz")}, true},
      {{FPL("//////host/bar/"), FPL("//////HOST/bar/baz")}, true},
      {{FPL("//host/bar/"), FPL("//host/bar/baz")}, true},
      {{FPL("//host/bar/"), FPL("//host2/bar/baz")}, false},
      {{FPL("//host/bar/"), FPL("/HOST/bar/baz")}, false},
      {{FPL("//host/bar/"), FPL("/host/bar/baz")}, false},
      {{FPL("//HOST/foo"), FPL("//host/foo/bar")}, true},
      {{FPL("/foo"), FPL("/foo/bar/baz")}, true},
      {{FPL("/foo/bar.txt"), FPL("/foo/bar/baz")}, false},
      {{FPL("/foo/bar"), FPL("/foo//bar/baz")}, true},
      {{FPL("/foo/bar"), FPL("/FOO//bar/baz/")}, false},
      {{FPL("/foo/bar"), FPL("/foo//bar/baz/")}, true},
      {{FPL("/foo/bar"), FPL("/foo/bar")}, false},
      {{FPL("/foo/bar"), FPL("/foo/bar/baz")}, true},
      {{FPL("/foo/bar"), FPL("/foo/bar2/baz")}, false},
      {{FPL("/foo/bar"), FPL("/foo2/bar/baz")}, false},
      {{FPL("/foo/bar/"), FPL("/foo/bar/baz")}, true},
      {{FPL("/foo/bar/baz"), FPL("/foo/bar")}, false},
      {{FPL("foo"), FPL("./foo/bar/baz")}, true},
      {{FPL("foo"), FPL("")}, false},
      {{FPL("foo"), FPL(".")}, false},
      {{FPL("foo"), FPL(".")}, false},
      {{FPL("foo"), FPL("boo/bar/baz")}, false},
      {{FPL("foo"), FPL("foo/bar/baz")}, true},
      {{FPL("foo/b:"), FPL("foo/B:/baz")}, false},
      {{FPL("foo/bar"), FPL("/foo/bar/baz")}, false},
      {{FPL("foo/bar"), FPL("foo/bar/baz")}, true},
      {{FPL("foo/bar"), FPL("foo/bar2/baz")}, false},
      {{FPL("foo/bar"), FPL("foo2/bar/baz")}, false},
      {{FPL("foo/bar/../baz"), FPL("foo/baz/aaa")}, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("./c:"), FPL("C:foo")}, true},
      {{FPL("c:"), FPL("*:/foo/bar/baz")}, false},
      {{FPL("c:"), FPL("C:/foo")}, true},
      {{FPL("c:"), FPL("c:/foo/bar/baz")}, true},
      {{FPL("c:"), FPL("C:foo")}, true},
      {{FPL("c:/"), FPL("C:/foo")}, true},
      {{FPL("c:/"), FPL("c:/foo/bar/baz")}, true},
      // IsParentSlow() splits "c:/" -> ["c:", "/"], "C:foo" -> ["C:", "foo"].
      // Since the components of "c:/" aren't a prefix of the components of
      // "C:foo", it concludes that it isn't a parent.
      {{FPL("c:/"), FPL("C:foo")}, false},
      {{FPL("c:/foo"), FPL("C:foo/bar")}, false},
      {{FPL("c:/foo/bar"), FPL("c:///foo/bar/baz")}, true},
      {{FPL("c:/foo/bar"), FPL("c://foo/bar/baz")}, false},
      {{FPL("c:/foo/bar"), FPL("c:/foo/bar/baz")}, true},
      {{FPL("c:/foo/bar"), FPL("c:/foo/bar2/baz")}, false},
      {{FPL("c:/foo/bar"), FPL("c:/foo2/bar/baz")}, false},
      {{FPL("c:/foo/bar"), FPL("d:/foo/bar/baz")}, false},
      {{FPL("c:/foo/bar"), FPL("D:/foo/bar/baz")}, false},
      {{FPL("C:/foo/bar"), FPL("d:/foo/bar/baz")}, false},
      {{FPL("c:foo"), FPL("C:/foo/bar")}, false},
      {{FPL("c:foo"), FPL("C:foo/bar")}, true},
      {{FPL("c:foo/bar"), FPL("C:foo/bar/baz")}, true},
      {{FPL("E:/Foo/bar"), FPL("e:/foo/bar/baz")}, false},
      {{FPL("E:/foo/bar"), FPL("e:/foo/bar/baz")}, true},
      {{FPL("e:/foo/bar"), FPL("E:/foo2/bar/baz")}, false},
      {{FPL("f:/foo/bar"), FPL("F:/foo/Bar/baz")}, false},
      {{FPL("f:/foo/bar"), FPL("F:/foo/bar/baz")}, true},
      {{FPL("F:/foo/bar"), FPL("f:/foo2/bar/baz")}, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      // Mix different types of separators in the same test case.
      {{FPL("\\foo/bar"), FPL("\\foo/bar/baz")}, true},
      {{FPL("\\foo/bar"), FPL("\\foo\\bar/baz")}, true},
      {{FPL("\\foo/bar"), FPL("\\foo\\/\\/bar/baz")}, true},
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath parent(cases[i].inputs[0]);
    FilePath child(cases[i].inputs[1]);

    EXPECT_EQ(parent.IsParent(child), cases[i].expected)
        << "i: " << i << ", parent: " << parent.value()
        << ", child: " << child.value();

#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    // Using different separators should not affect the result.
    FilePath parent_swapped(swap_separators(parent));
    FilePath child_swapped(swap_separators(child));

    EXPECT_EQ(parent_swapped.IsParent(child_swapped), cases[i].expected)
        << "i (swapped): " << i << ", parent: " << parent_swapped.value()
        << ", child: " << child_swapped.value();
#endif  // defined(FILE_PATH_USES_WIN_SEPARATORS)
  }
}

#if 0
TEST_F(FilePathTest, IsParentFastTest) {
  {
    test::ScopedFeatureList feature_list;
    feature_list.InitAndEnableFeature(features::kFastFilePathIsParent);
    FilePath::InitializeFeatures();
    IsParentTest(/*is_fast=*/true);
  }
  // Reset feature state.
  FilePath::InitializeFeatures();
}
#endif  // 0

TEST_F(FilePathTest, IsParentSlowTest) {
  {
#if 0
    test::ScopedFeatureList feature_list;
    feature_list.InitAndDisableFeature(features::kFastFilePathIsParent);
    FilePath::InitializeFeatures();
#endif  // 0
    IsParentTest(/*is_fast=*/false);
  }
#if 0
  // Reset feature state.
  FilePath::InitializeFeatures();
#endif  // 0
}

TEST_F(FilePathTest, AppendRelativePathTest) {
  const auto cases = std::to_array<BinaryTestData>({
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("/"), FPL("/foo/bar/baz")}, FPL("foo\\bar\\baz")},
#else   // FILE_PATH_USES_WIN_SEPARATORS
      {{FPL("/"), FPL("/foo/bar/baz")}, FPL("foo/bar/baz")},
#endif  // FILE_PATH_USES_WIN_SEPARATORS
      {{FPL("/foo/bar"), FPL("/foo/bar/baz")}, FPL("baz")},
      {{FPL("/foo/bar/"), FPL("/foo/bar/baz")}, FPL("baz")},
      {{FPL("//foo/bar/"), FPL("//foo/bar/baz")}, FPL("baz")},
      {{FPL("/foo/bar"), FPL("/foo2/bar/baz")}, FPL("")},
      {{FPL("/foo/bar.txt"), FPL("/foo/bar/baz")}, FPL("")},
      {{FPL("/foo/bar"), FPL("/foo/bar2/baz")}, FPL("")},
      {{FPL("/foo/bar"), FPL("/foo/bar")}, FPL("")},
      {{FPL("/foo/bar/baz"), FPL("/foo/bar")}, FPL("")},
      {{FPL("foo/bar"), FPL("foo/bar/baz")}, FPL("baz")},
      {{FPL("foo/bar"), FPL("foo2/bar/baz")}, FPL("")},
      {{FPL("foo/bar"), FPL("foo/bar2/baz")}, FPL("")},
      {{FPL(""), FPL("foo")}, FPL("")},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:/foo/bar"), FPL("c:/foo/bar/baz")}, FPL("baz")},
      {{FPL("E:/foo/bar"), FPL("e:/foo/bar/baz")}, FPL("baz")},
      {{FPL("f:/foo/bar"), FPL("F:/foo/bar/baz")}, FPL("baz")},
      {{FPL("E:/Foo/bar"), FPL("e:/foo/bar/baz")}, FPL("")},
      {{FPL("f:/foo/bar"), FPL("F:/foo/Bar/baz")}, FPL("")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("c:/"), FPL("c:/foo/bar/baz")}, FPL("foo\\bar\\baz")},
  // TODO(akalin): Figure out how to handle the corner case in the
  // commented-out test case below.  Appending to an empty path gives
  // /foo\bar\baz but appending to a nonempty path "blah" gives
  // blah\foo\bar\baz.
  // { { FPL("c:"),            FPL("c:/foo/bar/baz") }, FPL("foo\\bar\\baz")},
#endif  // FILE_PATH_USES_WIN_SEPARATORS
      {{FPL("c:/foo/bar"), FPL("d:/foo/bar/baz")}, FPL("")},
      {{FPL("c:/foo/bar"), FPL("D:/foo/bar/baz")}, FPL("")},
      {{FPL("C:/foo/bar"), FPL("d:/foo/bar/baz")}, FPL("")},
      {{FPL("c:/foo/bar"), FPL("c:/foo2/bar/baz")}, FPL("")},
      {{FPL("e:/foo/bar"), FPL("E:/foo2/bar/baz")}, FPL("")},
      {{FPL("F:/foo/bar"), FPL("f:/foo2/bar/baz")}, FPL("")},
      {{FPL("c:/foo/bar"), FPL("c:/foo/bar2/baz")}, FPL("")},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("\\foo\\bar"), FPL("\\foo\\bar\\baz")}, FPL("baz")},
      {{FPL("\\foo/bar"), FPL("\\foo\\bar\\baz")}, FPL("baz")},
      {{FPL("\\foo/bar"), FPL("\\foo/bar/baz")}, FPL("baz")},
      {{FPL("\\"), FPL("\\foo\\bar\\baz")}, FPL("foo\\bar\\baz")},
      {{FPL(""), FPL("\\foo\\bar\\baz")}, FPL("")},
      {{FPL("\\foo\\bar"), FPL("\\foo2\\bar\\baz")}, FPL("")},
      {{FPL("\\foo\\bar"), FPL("\\foo\\bar2\\baz")}, FPL("")},
#endif  // FILE_PATH_USES_WIN_SEPARATORS

      // For network paths, the hosts are compared ignoring case, while the rest
      // of the path is compared using case.
      {{FPL("//FOO/bar/"), FPL("//foo/bar/baz")}, FPL("baz")},
      {{FPL("//foo/BAR/"), FPL("//foo/bar/baz")}, FPL("")},
      // For non-network paths, the first component is not a host and should be
      // compared using case.
      {{FPL("/FOO/bar/"), FPL("/foo/bar/baz")}, FPL("")},
      // Degenerate case when parent has no hostname.
      {{FPL("//"), FPL("//foo")}, FPL("foo")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      // Network path tests but using Windows path separators.
      {{FPL("\\\\FOO\\bar"), FPL("\\\\foo\\bar\\baz")}, FPL("baz")},
      {{FPL("\\\\fOO\\Bar"), FPL("\\\\foo\\bar\\baz")}, FPL("")},
      {{FPL("\\FOO\\bar"), FPL("\\foo\\bar\\baz")}, FPL("")},
      {{FPL("\\\\"), FPL("\\\\foo")}, FPL("foo")},
#endif  // FILE_PATH_USES_WIN_SEPARATORS

  });

  const FilePath base(FPL("blah"));

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath parent(cases[i].inputs[0]);
    FilePath child(cases[i].inputs[1]);
    {
      FilePath result;
      bool success = parent.AppendRelativePath(child, &result);
      EXPECT_EQ(!cases[i].expected.empty(), success)
          << "i: " << i << ", parent: " << parent.value()
          << ", child: " << child.value();
      EXPECT_EQ(cases[i].expected, result.value())
          << "i: " << i << ", parent: " << parent.value()
          << ", child: " << child.value();
    }
    {
      FilePath result(base);
      bool success = parent.AppendRelativePath(child, &result);
      EXPECT_EQ(!cases[i].expected.empty(), success)
          << "i: " << i << ", parent: " << parent.value()
          << ", child: " << child.value();
      EXPECT_EQ(base.Append(cases[i].expected).value(), result.value())
          << "i: " << i << ", parent: " << parent.value()
          << ", child: " << child.value();
    }
  }
}

TEST_F(FilePathTest, EqualityTest) {
  const auto cases = std::to_array<BinaryBooleanTestData>({
      {{FPL("/foo/bar/baz"), FPL("/foo/bar/baz")}, true},
      {{FPL("/foo/bar"), FPL("/foo/bar/baz")}, false},
      {{FPL("/foo/bar/baz"), FPL("/foo/bar")}, false},
      {{FPL("//foo/bar/"), FPL("//foo/bar/")}, true},
      {{FPL("/foo/bar"), FPL("/foo2/bar")}, false},
      {{FPL("/foo/bar.txt"), FPL("/foo/bar")}, false},
      {{FPL("foo/bar"), FPL("foo/bar")}, true},
      {{FPL("foo/bar"), FPL("foo/bar/baz")}, false},
      {{FPL(""), FPL("foo")}, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:/foo/bar"), FPL("c:/foo/bar")}, true},
      {{FPL("E:/foo/bar"), FPL("e:/foo/bar")}, true},
      {{FPL("f:/foo/bar"), FPL("F:/foo/bar")}, true},
      {{FPL("E:/Foo/bar"), FPL("e:/foo/bar")}, false},
      {{FPL("f:/foo/bar"), FPL("F:/foo/Bar")}, false},
      {{FPL("c:/"), FPL("c:/")}, true},
      {{FPL("c:"), FPL("c:")}, true},
      {{FPL("c:/foo/bar"), FPL("d:/foo/bar")}, false},
      {{FPL("c:/foo/bar"), FPL("D:/foo/bar")}, false},
      {{FPL("C:/foo/bar"), FPL("d:/foo/bar")}, false},
      {{FPL("c:/foo/bar"), FPL("c:/foo2/bar")}, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("\\foo\\bar"), FPL("\\foo\\bar")}, true},
      {{FPL("\\foo/bar"), FPL("\\foo/bar")}, true},
      {{FPL("\\foo/bar"), FPL("\\foo\\bar")}, false},
      {{FPL("\\"), FPL("\\")}, true},
      {{FPL("\\"), FPL("/")}, false},
      {{FPL(""), FPL("\\")}, false},
      {{FPL("\\foo\\bar"), FPL("\\foo2\\bar")}, false},
      {{FPL("\\foo\\bar"), FPL("\\foo\\bar2")}, false},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:\\foo\\bar"), FPL("c:\\foo\\bar")}, true},
      {{FPL("E:\\foo\\bar"), FPL("e:\\foo\\bar")}, true},
      {{FPL("f:\\foo\\bar"), FPL("F:\\foo/bar")}, false},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#endif  // FILE_PATH_USES_WIN_SEPARATORS
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath a(cases[i].inputs[0]);
    FilePath b(cases[i].inputs[1]);

    EXPECT_EQ(a == b, cases[i].expected)
        << "equality i: " << i << ", a: " << a.value() << ", b: " << b.value();
  }

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath a(cases[i].inputs[0]);
    FilePath b(cases[i].inputs[1]);

    EXPECT_EQ(a != b, !cases[i].expected)
        << "inequality i: " << i << ", a: " << a.value()
        << ", b: " << b.value();
  }
}

TEST_F(FilePathTest, MacroExpansion) {
  EXPECT_EQ(FILE_PATH_LITERAL(TEST_FILE), FILE_PATH_LITERAL("TestFile"));
}

TEST_F(FilePathTest, Extension) {
  FilePath base_dir(FILE_PATH_LITERAL("base_dir"));

  FilePath jpg = base_dir.Append(FILE_PATH_LITERAL("foo.jpg"));
  EXPECT_EQ(FILE_PATH_LITERAL(".jpg"), jpg.Extension());
  EXPECT_EQ(FILE_PATH_LITERAL(".jpg"), jpg.FinalExtension());

  FilePath base = jpg.BaseName().RemoveExtension();
  EXPECT_EQ(FILE_PATH_LITERAL("foo"), base.value());

  FilePath path_no_ext = base_dir.Append(base);
  EXPECT_EQ(path_no_ext.value(), jpg.RemoveExtension().value());

  EXPECT_EQ(path_no_ext.value(), path_no_ext.RemoveExtension().value());
  EXPECT_EQ(FILE_PATH_LITERAL(""), path_no_ext.Extension());
  EXPECT_EQ(FILE_PATH_LITERAL(""), path_no_ext.FinalExtension());
}

TEST_F(FilePathTest, Extension2) {
  // clang-format off
  const auto cases = std::to_array<UnaryTestData>({
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
    { FPL("C:\\a\\b\\c.ext"),        FPL(".ext") },
    { FPL("C:\\a\\b\\c."),           FPL(".") },
    { FPL("C:\\a\\b\\c"),            FPL("") },
    { FPL("C:\\a\\b\\"),             FPL("") },
    { FPL("C:\\a\\b.\\"),            FPL(".") },
    { FPL("C:\\a\\b\\c.ext1.ext2"),  FPL(".ext2") },
    { FPL("C:\\foo.bar\\\\\\"),      FPL(".bar") },
    { FPL("C:\\foo.bar\\.."),        FPL("") },
    { FPL("C:\\foo.bar\\..\\\\"),    FPL("") },
#endif
    { FPL("/foo/bar/baz.EXT"),       FPL(".EXT") },
    { FPL("/foo/bar/baz.Ext"),       FPL(".Ext") },
    { FPL("/foo/bar/baz.ext"),       FPL(".ext") },
    { FPL("/foo/bar/baz."),          FPL(".") },
    { FPL("/foo/bar/baz.."),         FPL(".") },
    { FPL("/foo/bar/baz"),           FPL("") },
    { FPL("/foo/bar/"),              FPL("") },
    { FPL("/foo/bar./"),             FPL(".") },
    { FPL("/foo/bar/baz.ext1.ext2"), FPL(".ext2") },
    { FPL("/subversion-1.6.12.zip"), FPL(".zip") },
    { FPL("/foo.12345.gz"),          FPL(".gz") },
    { FPL("/foo..gz"),               FPL(".gz") },
    { FPL("."),                      FPL("") },
    { FPL(".."),                     FPL("") },
    { FPL("./foo"),                  FPL("") },
    { FPL("./foo.ext"),              FPL(".ext") },
    { FPL("/foo.ext1/bar.ext2"),     FPL(".ext2") },
    { FPL("/foo.bar////"),           FPL(".bar") },
    { FPL("/foo.bar/.."),            FPL("") },
    { FPL("/foo.bar/..////"),        FPL("") },
    { FPL("/foo.1234.luser.js"),     FPL(".js") },
    { FPL("/user.js"),               FPL(".js") },
  });
  const auto double_extension_cases = std::to_array<UnaryTestData>({
    // `kCommonDoubleExtensionSuffixes` cases. Blah is not on that allow-list.
    // Membership is (ASCII) case-insensitive: both ".Z" and ".z" match.
    { FPL("/foo.TAR.bz2"),           FPL(".TAR.bz2") },
    { FPL("/foo.tar.Z"),             FPL(".tar.Z") },
    { FPL("/foo.tar.blah"),          FPL(".blah") },
    { FPL("/foo.tar.bz"),            FPL(".tar.bz") },
    { FPL("/foo.tar.bz2"),           FPL(".tar.bz2") },
    { FPL("/foo.tar.gz"),            FPL(".tar.gz") },
    { FPL("/foo.tar.lz"),            FPL(".tar.lz") },
    { FPL("/foo.tar.lzma"),          FPL(".tar.lzma") },
    { FPL("/foo.tar.lzo"),           FPL(".tar.lzo") },
    { FPL("/foo.tar.xz"),            FPL(".tar.xz") },
    { FPL("/foo.tar.z"),             FPL(".tar.z") },
    { FPL("/foo.tar.zst"),           FPL(".tar.zst") },
    // `kCommonDoubleExtensions` cases.
    { FPL("/foo.1234.user.js"),      FPL(".user.js") },
    { FPL("foo.user.js"),            FPL(".user.js") },
    // Other cases.
    { FPL("/foo.1234.gz"),           FPL(".1234.gz") },
    { FPL("/foo.1234.gz."),          FPL(".") },
    { FPL("/foo.1234.tar.gz"),       FPL(".tar.gz") },
    { FPL("/foo.tar.tar.gz"),        FPL(".tar.gz") },
    { FPL("/foo.tar.gz.gz"),         FPL(".gz.gz") },
  });
  // clang-format on

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].input);
    FilePath::StringType extension = path.Extension();
    FilePath::StringType final_extension = path.FinalExtension();
    EXPECT_EQ(cases[i].expected, extension)
        << "i: " << i << ", path: " << path.value();
    EXPECT_EQ(cases[i].expected, final_extension)
        << "i: " << i << ", path: " << path.value();
  }

  for (size_t i = 0; i < std::size(double_extension_cases); ++i) {
    FilePath path(double_extension_cases[i].input);
    FilePath::StringType extension = path.Extension();
    EXPECT_EQ(double_extension_cases[i].expected, extension)
        << "i: " << i << ", path: " << path.value();
  }
}

TEST_F(FilePathTest, InsertBeforeExtension) {
  const auto cases = std::to_array<BinaryTestData>({
      {{FPL(""), FPL("")}, FPL("")},
      {{FPL(""), FPL("txt")}, FPL("")},
      {{FPL("."), FPL("txt")}, FPL("")},
      {{FPL(".."), FPL("txt")}, FPL("")},
      {{FPL("foo.dll"), FPL("txt")}, FPL("footxt.dll")},
      {{FPL("."), FPL("")}, FPL(".")},
      {{FPL("foo.dll"), FPL(".txt")}, FPL("foo.txt.dll")},
      {{FPL("foo"), FPL("txt")}, FPL("footxt")},
      {{FPL("foo"), FPL(".txt")}, FPL("foo.txt")},
      {{FPL("foo.baz.dll"), FPL("txt")}, FPL("foo.baztxt.dll")},
      {{FPL("foo.baz.dll"), FPL(".txt")}, FPL("foo.baz.txt.dll")},
      {{FPL("foo.dll"), FPL("")}, FPL("foo.dll")},
      {{FPL("foo.dll"), FPL(".")}, FPL("foo..dll")},
      {{FPL("foo"), FPL("")}, FPL("foo")},
      {{FPL("foo"), FPL(".")}, FPL("foo.")},
      {{FPL("foo.baz.dll"), FPL("")}, FPL("foo.baz.dll")},
      {{FPL("foo.baz.dll"), FPL(".")}, FPL("foo.baz..dll")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("\\"), FPL("")}, FPL("\\")},
      {{FPL("\\"), FPL("txt")}, FPL("\\txt")},
      {{FPL("\\."), FPL("txt")}, FPL("")},
      {{FPL("\\.."), FPL("txt")}, FPL("")},
      {{FPL("\\."), FPL("")}, FPL("\\.")},
      {{FPL("C:\\bar\\foo.dll"), FPL("txt")}, FPL("C:\\bar\\footxt.dll")},
      {{FPL("C:\\bar.baz\\foodll"), FPL("txt")}, FPL("C:\\bar.baz\\foodlltxt")},
      {{FPL("C:\\bar.baz\\foo.dll"), FPL("txt")},
       FPL("C:\\bar.baz\\footxt.dll")},
      {{FPL("C:\\bar.baz\\foo.dll.exe"), FPL("txt")},
       FPL("C:\\bar.baz\\foo.dlltxt.exe")},
      {{FPL("C:\\bar.baz\\foo"), FPL("")}, FPL("C:\\bar.baz\\foo")},
      {{FPL("C:\\bar.baz\\foo.exe"), FPL("")}, FPL("C:\\bar.baz\\foo.exe")},
      {{FPL("C:\\bar.baz\\foo.dll.exe"), FPL("")},
       FPL("C:\\bar.baz\\foo.dll.exe")},
      {{FPL("C:\\bar\\baz\\foo.exe"), FPL(" (1)")},
       FPL("C:\\bar\\baz\\foo (1).exe")},
      {{FPL("C:\\foo.baz\\\\"), FPL(" (1)")}, FPL("C:\\foo (1).baz")},
      {{FPL("C:\\foo.baz\\..\\"), FPL(" (1)")}, FPL("")},
#endif
      {{FPL("/"), FPL("")}, FPL("/")},
      {{FPL("/"), FPL("txt")}, FPL("/txt")},
      {{FPL("/."), FPL("txt")}, FPL("")},
      {{FPL("/.."), FPL("txt")}, FPL("")},
      {{FPL("/."), FPL("")}, FPL("/.")},
      {{FPL("/bar/foo.dll"), FPL("txt")}, FPL("/bar/footxt.dll")},
      {{FPL("/bar.baz/foodll"), FPL("txt")}, FPL("/bar.baz/foodlltxt")},
      {{FPL("/bar.baz/foo.dll"), FPL("txt")}, FPL("/bar.baz/footxt.dll")},
      {{FPL("/bar.baz/foo.dll.exe"), FPL("txt")},
       FPL("/bar.baz/foo.dlltxt.exe")},
      {{FPL("/bar.baz/foo"), FPL("")}, FPL("/bar.baz/foo")},
      {{FPL("/bar.baz/foo.exe"), FPL("")}, FPL("/bar.baz/foo.exe")},
      {{FPL("/bar.baz/foo.dll.exe"), FPL("")}, FPL("/bar.baz/foo.dll.exe")},
      {{FPL("/bar/baz/foo.exe"), FPL(" (1)")}, FPL("/bar/baz/foo (1).exe")},
      {{FPL("/bar/baz/..////"), FPL(" (1)")}, FPL("")},
  });
  for (unsigned int i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath result = path.InsertBeforeExtension(cases[i].inputs[1]);
    EXPECT_EQ(cases[i].expected, result.value())
        << "i: " << i << ", path: " << path.value()
        << ", insert: " << cases[i].inputs[1];
  }
}

TEST_F(FilePathTest, RemoveExtension) {
  const auto cases = std::to_array<UnaryTestData>({
      {FPL(""), FPL("")},
      {FPL("."), FPL(".")},
      {FPL(".."), FPL("..")},
      {FPL("foo.dll"), FPL("foo")},
      {FPL("./foo.dll"), FPL("./foo")},
      {FPL("foo..dll"), FPL("foo.")},
      {FPL("foo"), FPL("foo")},
      {FPL("foo."), FPL("foo")},
      {FPL("foo.."), FPL("foo.")},
      {FPL("foo.baz.dll"), FPL("foo.baz")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {FPL("C:\\foo.bar\\foo"), FPL("C:\\foo.bar\\foo")},
      {FPL("C:\\foo.bar\\..\\\\"), FPL("C:\\foo.bar\\..\\\\")},
#endif
      {FPL("/foo.bar/foo"), FPL("/foo.bar/foo")},
      {FPL("/foo.bar/..////"), FPL("/foo.bar/..////")},
  });
  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].input);
    FilePath removed = path.RemoveExtension();
    FilePath removed_final = path.RemoveFinalExtension();
    EXPECT_EQ(cases[i].expected, removed.value())
        << "i: " << i << ", path: " << path.value();
    EXPECT_EQ(cases[i].expected, removed_final.value())
        << "i: " << i << ", path: " << path.value();
  }

  const FilePath::StringViewType tarballs[] = {
      FPL("foo.tar.gz"), FPL("foo.tar.xz"), FPL("foo.tar.bz2"),
      FPL("foo.tar.Z"), FPL("foo.tar.bz")};
  for (size_t i = 0; i < std::size(tarballs); ++i) {
    FilePath path(FPL("foo.tar.gz"));
    FilePath removed = path.RemoveExtension();
    FilePath removed_final = path.RemoveFinalExtension();
    EXPECT_EQ(FPL("foo"), removed.value())
        << "i: " << i << ", path: " << path.value();
    EXPECT_EQ(FPL("foo.tar"), removed_final.value())
        << "i: " << i << ", path: " << path.value();
  }
}

TEST_F(FilePathTest, ReplaceExtension) {
  const auto cases = std::to_array<BinaryTestData>({
      {{FPL(""), FPL("")}, FPL("")},
      {{FPL(""), FPL("txt")}, FPL("")},
      {{FPL("."), FPL("txt")}, FPL("")},
      {{FPL(".."), FPL("txt")}, FPL("")},
      {{FPL("."), FPL("")}, FPL("")},
      {{FPL("foo.dll"), FPL("txt")}, FPL("foo.txt")},
      {{FPL("./foo.dll"), FPL("txt")}, FPL("./foo.txt")},
      {{FPL("foo..dll"), FPL("txt")}, FPL("foo..txt")},
      {{FPL("foo.dll"), FPL(".txt")}, FPL("foo.txt")},
      {{FPL("foo"), FPL("txt")}, FPL("foo.txt")},
      {{FPL("foo."), FPL("txt")}, FPL("foo.txt")},
      {{FPL("foo.."), FPL("txt")}, FPL("foo..txt")},
      {{FPL("foo"), FPL(".txt")}, FPL("foo.txt")},
      {{FPL("foo.baz.dll"), FPL("txt")}, FPL("foo.baz.txt")},
      {{FPL("foo.baz.dll"), FPL(".txt")}, FPL("foo.baz.txt")},
      {{FPL("foo.dll"), FPL("")}, FPL("foo")},
      {{FPL("foo.dll"), FPL(".")}, FPL("foo")},
      {{FPL("foo"), FPL("")}, FPL("foo")},
      {{FPL("foo"), FPL(".")}, FPL("foo")},
      {{FPL("foo.baz.dll"), FPL("")}, FPL("foo.baz")},
      {{FPL("foo.baz.dll"), FPL(".")}, FPL("foo.baz")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("C:\\foo.bar\\foo"), FPL("baz")}, FPL("C:\\foo.bar\\foo.baz")},
      {{FPL("C:\\foo.bar\\..\\\\"), FPL("baz")}, FPL("")},
#endif
      {{FPL("/foo.bar/foo"), FPL("baz")}, FPL("/foo.bar/foo.baz")},
      {{FPL("/foo.bar/..////"), FPL("baz")}, FPL("")},
  });
  for (unsigned int i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath replaced = path.ReplaceExtension(cases[i].inputs[1]);
    EXPECT_EQ(cases[i].expected, replaced.value())
        << "i: " << i << ", path: " << path.value()
        << ", replace: " << cases[i].inputs[1];
  }
}

TEST_F(FilePathTest, AddExtension) {
  const auto cases = std::to_array<BinaryTestData>({
      {{FPL(""), FPL("")}, FPL("")},
      {{FPL(""), FPL("txt")}, FPL("")},
      {{FPL("."), FPL("txt")}, FPL("")},
      {{FPL(".."), FPL("txt")}, FPL("")},
      {{FPL("."), FPL("")}, FPL("")},
      {{FPL("foo.dll"), FPL("txt")}, FPL("foo.dll.txt")},
      {{FPL("./foo.dll"), FPL("txt")}, FPL("./foo.dll.txt")},
      {{FPL("foo..dll"), FPL("txt")}, FPL("foo..dll.txt")},
      {{FPL("foo.dll"), FPL(".txt")}, FPL("foo.dll.txt")},
      {{FPL("foo"), FPL("txt")}, FPL("foo.txt")},
      {{FPL("foo."), FPL("txt")}, FPL("foo.txt")},
      {{FPL("foo.."), FPL("txt")}, FPL("foo..txt")},
      {{FPL("foo"), FPL(".txt")}, FPL("foo.txt")},
      {{FPL("foo.baz.dll"), FPL("txt")}, FPL("foo.baz.dll.txt")},
      {{FPL("foo.baz.dll"), FPL(".txt")}, FPL("foo.baz.dll.txt")},
      {{FPL("foo.dll"), FPL("")}, FPL("foo.dll")},
      {{FPL("foo.dll"), FPL(".")}, FPL("foo.dll")},
      {{FPL("foo"), FPL("")}, FPL("foo")},
      {{FPL("foo"), FPL(".")}, FPL("foo")},
      {{FPL("foo.baz.dll"), FPL("")}, FPL("foo.baz.dll")},
      {{FPL("foo.baz.dll"), FPL(".")}, FPL("foo.baz.dll")},
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("C:\\foo.bar\\foo"), FPL("baz")}, FPL("C:\\foo.bar\\foo.baz")},
      {{FPL("C:\\foo.bar\\..\\\\"), FPL("baz")}, FPL("")},
#endif
      {{FPL("/foo.bar/foo"), FPL("baz")}, FPL("/foo.bar/foo.baz")},
      {{FPL("/foo.bar/..////"), FPL("baz")}, FPL("")},
  });
  for (unsigned int i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath added = path.AddExtension(cases[i].inputs[1]);
    EXPECT_EQ(cases[i].expected, added.value())
        << "i: " << i << ", path: " << path.value()
        << ", add: " << cases[i].inputs[1];
  }
}

TEST_F(FilePathTest, MatchesExtension) {
  const auto cases = std::to_array<BinaryBooleanTestData>({
      {{FPL("foo"), FPL("")}, true},
      {{FPL("foo"), FPL(".")}, false},
      {{FPL("foo."), FPL("")}, false},
      {{FPL("foo."), FPL(".")}, true},
      {{FPL("foo.txt"), FPL(".dll")}, false},
      {{FPL("foo.txt"), FPL(".txt")}, true},
      {{FPL("foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("foo.txt.dll"), FPL(".dll")}, true},
      {{FPL("foo.tar.gz"), FPL(".gz")}, false},
      {{FPL("foo.tar.lzma"), FPL(".tar.lzma")}, true},
      {{FPL("foo.TXT"), FPL(".txt")}, true},
      {{FPL("foo.txt"), FPL(".TXT")}, true},
      {{FPL("foo.tXt"), FPL(".txt")}, true},
      {{FPL("foo.txt"), FPL(".tXt")}, true},
      {{FPL("foo.tXt"), FPL(".TXT")}, true},
      {{FPL("foo.tXt"), FPL(".tXt")}, true},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:/foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("c:/foo.txt"), FPL(".txt")}, true},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("c:\\bar\\foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("c:\\bar\\foo.txt"), FPL(".txt")}, true},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
      {{FPL("/bar/foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("/bar/foo.txt"), FPL(".txt")}, true},
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_APPLE)
      // Umlauts A, O, U: direct comparison, and upper case vs. lower case
      {{FPL("foo.\u00E4\u00F6\u00FC"), FPL(".\u00E4\u00F6\u00FC")}, true},
      {{FPL("foo.\u00C4\u00D6\u00DC"), FPL(".\u00E4\u00F6\u00FC")}, true},
      // C with circumflex: direct comparison, and upper case vs. lower case
      {{FPL("foo.\u0109"), FPL(".\u0109")}, true},
      {{FPL("foo.\u0108"), FPL(".\u0109")}, true},
#endif
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath::StringType ext(cases[i].inputs[1]);

    EXPECT_EQ(cases[i].expected, path.MatchesExtension(ext))
        << "i: " << i << ", path: " << path.value() << ", ext: " << ext;
  }
}

TEST_F(FilePathTest, MatchesFinalExtension) {
  const auto cases = std::to_array<BinaryBooleanTestData>({
      {{FPL("foo"), FPL("")}, true},
      {{FPL("foo"), FPL(".")}, false},
      {{FPL("foo."), FPL("")}, false},
      {{FPL("foo."), FPL(".")}, true},
      {{FPL("foo.txt"), FPL(".dll")}, false},
      {{FPL("foo.txt"), FPL(".txt")}, true},
      {{FPL("foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("foo.txt.dll"), FPL(".dll")}, true},
      {{FPL("foo.tar.gz"), FPL(".gz")}, true},
      {{FPL("foo.tar.lzma"), FPL(".lzma")}, true},
      {{FPL("foo.tar.lzma"), FPL(".tar.lzma")}, false},
      {{FPL("foo.tlzma"), FPL(".tlzma")}, true},
      {{FPL("foo.TXT"), FPL(".txt")}, true},
      {{FPL("foo.txt"), FPL(".TXT")}, true},
      {{FPL("foo.tXt"), FPL(".txt")}, true},
      {{FPL("foo.txt"), FPL(".tXt")}, true},
      {{FPL("foo.tXt"), FPL(".TXT")}, true},
      {{FPL("foo.tXt"), FPL(".tXt")}, true},
#if defined(FILE_PATH_USES_DRIVE_LETTERS)
      {{FPL("c:/foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("c:/foo.txt"), FPL(".txt")}, true},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
      {{FPL("c:\\bar\\foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("c:\\bar\\foo.txt"), FPL(".txt")}, true},
#endif  // FILE_PATH_USES_DRIVE_LETTERS
      {{FPL("/bar/foo.txt.dll"), FPL(".txt")}, false},
      {{FPL("/bar/foo.txt"), FPL(".txt")}, true},
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_APPLE)
      // Umlauts A, O, U: direct comparison, and upper case vs. lower case
      {{FPL("foo.\u00E4\u00F6\u00FC"), FPL(".\u00E4\u00F6\u00FC")}, true},
      {{FPL("foo.\u00C4\u00D6\u00DC"), FPL(".\u00E4\u00F6\u00FC")}, true},
      // C with circumflex: direct comparison, and upper case vs. lower case
      {{FPL("foo.\u0109"), FPL(".\u0109")}, true},
      {{FPL("foo.\u0108"), FPL(".\u0109")}, true},
#endif
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath path(cases[i].inputs[0]);
    FilePath::StringType ext(cases[i].inputs[1]);

    EXPECT_EQ(cases[i].expected, path.MatchesFinalExtension(ext))
        << "i: " << i << ", path: " << path.value() << ", ext: " << ext;
  }
}

TEST_F(FilePathTest, CompareIgnoreCase) {
  const auto cases = std::to_array<BinaryIntTestData>({
      {{FPL("foo"), FPL("foo")}, 0},
      {{FPL("FOO"), FPL("foo")}, 0},
      {{FPL("foo.ext"), FPL("foo.ext")}, 0},
      {{FPL("FOO.EXT"), FPL("foo.ext")}, 0},
      {{FPL("Foo.Ext"), FPL("foo.ext")}, 0},
      {{FPL("foO"), FPL("foo")}, 0},
      {{FPL("foo"), FPL("foO")}, 0},
      {{FPL("fOo"), FPL("foo")}, 0},
      {{FPL("foo"), FPL("fOo")}, 0},
      {{FPL("bar"), FPL("foo")}, -1},
      {{FPL("foo"), FPL("bar")}, 1},
      {{FPL("BAR"), FPL("foo")}, -1},
      {{FPL("FOO"), FPL("bar")}, 1},
      {{FPL("bar"), FPL("FOO")}, -1},
      {{FPL("foo"), FPL("BAR")}, 1},
      {{FPL("BAR"), FPL("FOO")}, -1},
      {{FPL("FOO"), FPL("BAR")}, 1},
      // German "Eszett" (lower case and the new-fangled upper case)
      // Note that uc(<lowercase eszett>) => "SS", NOT <uppercase eszett>!
      // However, neither Windows nor Mac OSX converts these.
      // (or even have glyphs for <uppercase eszett>)
      {{FPL("\u00DF"), FPL("\u00DF")}, 0},
      {{FPL("\u1E9E"), FPL("\u1E9E")}, 0},
      {{FPL("\u00DF"), FPL("\u1E9E")}, -1},
      {{FPL("SS"), FPL("\u00DF")}, -1},
      {{FPL("SS"), FPL("\u1E9E")}, -1},
#if BUILDFLAG(IS_WIN) || BUILDFLAG(IS_APPLE)
      // Umlauts A, O, U: direct comparison, and upper case vs. lower case
      {{FPL("\u00E4\u00F6\u00FC"), FPL("\u00E4\u00F6\u00FC")}, 0},
      {{FPL("\u00C4\u00D6\u00DC"), FPL("\u00E4\u00F6\u00FC")}, 0},
      // C with circumflex: direct comparison, and upper case vs. lower case
      {{FPL("\u0109"), FPL("\u0109")}, 0},
      {{FPL("\u0108"), FPL("\u0109")}, 0},
      // Cyrillic letter SHA: direct comparison, and upper case vs. lower case
      {{FPL("\u0428"), FPL("\u0428")}, 0},
      {{FPL("\u0428"), FPL("\u0448")}, 0},
      // Greek letter DELTA: direct comparison, and upper case vs. lower case
      {{FPL("\u0394"), FPL("\u0394")}, 0},
      {{FPL("\u0394"), FPL("\u03B4")}, 0},
      // Japanese full-width A: direct comparison, and upper case vs. lower case
      // Note that full-width and standard characters are considered different.
      {{FPL("\uFF21"), FPL("\uFF21")}, 0},
      {{FPL("\uFF21"), FPL("\uFF41")}, 0},
      {{FPL("A"), FPL("\uFF21")}, -1},
      {{FPL("A"), FPL("\uFF41")}, -1},
      {{FPL("a"), FPL("\uFF21")}, -1},
      {{FPL("a"), FPL("\uFF41")}, -1},
#endif
#if BUILDFLAG(IS_APPLE)
      // Codepoints > 0x1000
      // Georgian letter DON: direct comparison, and upper case vs. lower case
      {{FPL("\u10A3"), FPL("\u10A3")}, 0},
      {{FPL("\u10A3"), FPL("\u10D3")}, 0},
      // Combining characters vs. pre-composed characters, upper and lower case
      {{FPL("k\u0301u\u032Do\u0304\u0301n"), FPL("\u1E31\u1E77\u1E53n")}, 0},
      {{FPL("k\u0301u\u032Do\u0304\u0301n"), FPL("kuon")}, 1},
      {{FPL("kuon"), FPL("k\u0301u\u032Do\u0304\u0301n")}, -1},
      {{FPL("K\u0301U\u032DO\u0304\u0301N"), FPL("KUON")}, 1},
      {{FPL("KUON"), FPL("K\u0301U\u032DO\u0304\u0301N")}, -1},
      {{FPL("k\u0301u\u032Do\u0304\u0301n"), FPL("KUON")}, 1},
      {{FPL("K\u0301U\u032DO\u0304\u0301N"), FPL("\u1E31\u1E77\u1E53n")}, 0},
      {{FPL("k\u0301u\u032Do\u0304\u0301n"), FPL("\u1E30\u1E76\u1E52n")}, 0},
      {{FPL("k\u0301u\u032Do\u0304\u0302n"), FPL("\u1E30\u1E76\u1E52n")}, 1},

      // Codepoints > 0xFFFF
      // Here, we compare the `Adlam Letter Shu` in its capital and small
      // version.
      {{FPL("\U0001E921"), FPL("\U0001E943")}, -1},
      {{FPL("\U0001E943"), FPL("\U0001E921")}, 1},
      {{FPL("\U0001E921"), FPL("\U0001E921")}, 0},
      {{FPL("\U0001E943"), FPL("\U0001E943")}, 0},
#endif
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath::StringType s1(cases[i].inputs[0]);
    FilePath::StringType s2(cases[i].inputs[1]);
    int result = FilePath::CompareIgnoreCase(s1, s2);
    EXPECT_EQ(cases[i].expected, result)
        << "i: " << i << ", s1: " << s1 << ", s2: " << s2;
  }
}

TEST_F(FilePathTest, ReferencesParent) {
  // clang-format off
  const auto cases = std::to_array<UnaryBooleanTestData>({
    { FPL("."),        false },
    { FPL(".."),       true },
#if BUILDFLAG(IS_WIN)
    { FPL(".. "),      true },
    { FPL(" .."),      true },
    { FPL("..."),      true },
#else
    { FPL(".. "),      false },
    { FPL(" .."),      false },
    { FPL("..."),      false },
#endif
    { FPL("a.."),      false },
    { FPL("..a"),      false },
    { FPL("../"),      true },
    { FPL("/.."),      true },
    { FPL("/../"),     true },
    { FPL("/a../"),    false },
    { FPL("/..a/"),    false },
    { FPL("//.."),     true },
    { FPL("..//"),     true },
    { FPL("//..//"),   true },
    { FPL("a//..//c"), true },
    { FPL("../b/c"),   true },
    { FPL("/../b/c"),  true },
    { FPL("a/b/.."),   true },
    { FPL("a/b/../"),  true },
    { FPL("a/../c"),   true },
    { FPL("a/b/c"),    false },
  });
  // clang-format on

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    bool observed = input.ReferencesParent();
    EXPECT_EQ(cases[i].expected, observed)
        << "i: " << i << ", input: " << input.value();
  }
}

TEST_F(FilePathTest, FromASCII) {
  const auto cases = std::to_array<UTF8TestData>({
      {FPL("foo.txt"), "foo.txt"},
      {FPL("!#$%&'()"), "!#$%&'()"},
  });

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath from_ascii = FilePath::FromASCII(cases[i].utf8);
    EXPECT_EQ(FilePath::StringType(cases[i].native), from_ascii.value())
        << "i: " << i << ", input: " << cases[i].utf8;
  }
}

TEST_F(FilePathTest, FromUTF8Unsafe_And_AsUTF8Unsafe) {
  const auto cases = std::to_array<UTF8TestData>({
      {FPL("foo.txt"), "foo.txt"},
      // "aeo" with accents. Use http://0xcc.net/jsescape/ to decode them.
      {FPL("\u00E0\u00E8\u00F2.txt"), "\xC3\xA0\xC3\xA8\xC3\xB2.txt"},
      // Full-width "ABC".
      {FPL("\uFF21\uFF22\uFF23.txt"),
       "\xEF\xBC\xA1\xEF\xBC\xA2\xEF\xBC\xA3.txt"},
  });

#if !defined(SYSTEM_NATIVE_UTF8) && \
    (BUILDFLAG(IS_LINUX) || BUILDFLAG(IS_CHROMEOS))
  ScopedLocale locale("en_US.UTF-8");
#endif

  for (size_t i = 0; i < std::size(cases); ++i) {
    // Test FromUTF8Unsafe() works.
    FilePath from_utf8 = FilePath::FromUTF8Unsafe(cases[i].utf8);
    EXPECT_EQ(cases[i].native, from_utf8.value())
        << "i: " << i << ", input: " << cases[i].native;
    // Test AsUTF8Unsafe() works.
    FilePath from_native = FilePath(cases[i].native);
    EXPECT_EQ(cases[i].utf8, from_native.AsUTF8Unsafe())
        << "i: " << i << ", input: " << cases[i].native;
    // Test the two file paths are identical.
    EXPECT_EQ(from_utf8.value(), from_native.value());
  }
}

TEST_F(FilePathTest, ConstructWithNUL) {
  // Assert FPS() works.
  ASSERT_EQ(3U, FPS("a\0b").length());

  // Test constructor strips '\0'
  FilePath path(FPS("a\0b"));
  EXPECT_EQ(1U, path.value().length());
  EXPECT_EQ(FPL("a"), path.value());
}

TEST_F(FilePathTest, AppendWithNUL) {
  // Assert FPS() works.
  ASSERT_EQ(3U, FPS("b\0b").length());

  // Test Append() strips '\0'
  FilePath path(FPL("a"));
  path = path.Append(FPS("b\0b"));
  EXPECT_EQ(3U, path.value().length());
#if defined(FILE_PATH_USES_WIN_SEPARATORS)
  EXPECT_EQ(FPL("a\\b"), path.value());
#else
  EXPECT_EQ(FPL("a/b"), path.value());
#endif
}

TEST_F(FilePathTest, AppendBaseName) {
  FilePath dir(FPL("foo"));
  auto file(SafeBaseName::Create(FPL("bar.txt")));
  EXPECT_TRUE(file);

#if defined(FILE_PATH_USES_WIN_SEPARATORS)
  EXPECT_EQ(dir.Append(*file), FilePath(FPL("foo\\bar.txt")));
#else
  EXPECT_EQ(dir.Append(*file), FilePath(FPL("foo/bar.txt")));
#endif
}

TEST_F(FilePathTest, ReferencesParentWithNUL) {
  // Assert FPS() works.
  ASSERT_EQ(3U, FPS("..\0").length());

  // Test ReferencesParent() doesn't break with "..\0"
  FilePath path(FPS("..\0"));
  EXPECT_TRUE(path.ReferencesParent());
}

#if defined(FILE_PATH_USES_WIN_SEPARATORS)
TEST_F(FilePathTest, NormalizePathSeparators) {
  const struct UnaryTestData cases[] = {
      {FPL("foo/bar"), FPL("foo\\bar")},
      {FPL("foo/bar\\betz"), FPL("foo\\bar\\betz")},
      {FPL("foo\\bar"), FPL("foo\\bar")},
      {FPL("foo\\bar/betz"), FPL("foo\\bar\\betz")},
      {FPL("foo"), FPL("foo")},
      // Trailing slashes don't automatically get stripped.  That's what
      // StripTrailingSeparators() is for.
      {FPL("foo\\"), FPL("foo\\")},
      {FPL("foo/"), FPL("foo\\")},
      {FPL("foo/bar\\"), FPL("foo\\bar\\")},
      {FPL("foo\\bar/"), FPL("foo\\bar\\")},
      {FPL("foo/bar/"), FPL("foo\\bar\\")},
      {FPL("foo\\bar\\"), FPL("foo\\bar\\")},
      {FPL("\\foo/bar"), FPL("\\foo\\bar")},
      {FPL("/foo\\bar"), FPL("\\foo\\bar")},
      {FPL("c:/foo/bar/"), FPL("c:\\foo\\bar\\")},
      {FPL("/foo/bar/"), FPL("\\foo\\bar\\")},
      {FPL("\\foo\\bar\\"), FPL("\\foo\\bar\\")},
      {FPL("c:\\foo/bar"), FPL("c:\\foo\\bar")},
      {FPL("//foo\\bar\\"), FPL("\\\\foo\\bar\\")},
      {FPL("\\\\foo\\bar\\"), FPL("\\\\foo\\bar\\")},
      {FPL("//foo\\bar\\"), FPL("\\\\foo\\bar\\")},
      // This method does not normalize the number of path separators.
      {FPL("foo\\\\bar"), FPL("foo\\\\bar")},
      {FPL("foo//bar"), FPL("foo\\\\bar")},
      {FPL("foo/\\bar"), FPL("foo\\\\bar")},
      {FPL("foo\\/bar"), FPL("foo\\\\bar")},
      {FPL("///foo\\\\bar"), FPL("\\\\\\foo\\\\bar")},
      {FPL("foo//bar///"), FPL("foo\\\\bar\\\\\\")},
      {FPL("foo/\\bar/\\"), FPL("foo\\\\bar\\\\")},
      {FPL("/\\foo\\/bar"), FPL("\\\\foo\\\\bar")},
  };
  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    FilePath observed = input.NormalizePathSeparators();
    EXPECT_EQ(FilePath::StringType(cases[i].expected), observed.value())
        << "i: " << i << ", input: " << input.value();
  }
}
#endif

TEST_F(FilePathTest, EndsWithSeparator) {
  const UnaryBooleanTestData cases[] = {
      {FPL(""), false},    {FPL("/"), true},         {FPL("foo/"), true},
      {FPL("bar"), false}, {FPL("/foo/bar"), false},
  };
  for (const auto& i : cases) {
    FilePath input = FilePath(i.input).NormalizePathSeparators();
    EXPECT_EQ(i.expected, input.EndsWithSeparator());
  }
}

TEST_F(FilePathTest, AsEndingWithSeparator) {
  const UnaryTestData cases[] = {{FPL(""), FPL("")},
                                 {FPL("/"), FPL("/")},
                                 {FPL("foo"), FPL("foo/")},
                                 {FPL("foo/"), FPL("foo/")}};
  for (const auto& i : cases) {
    FilePath input = FilePath(i.input).NormalizePathSeparators();
    FilePath expected = FilePath(i.expected).NormalizePathSeparators();
    EXPECT_EQ(expected.value(), input.AsEndingWithSeparator().value());
  }
}

#if BUILDFLAG(IS_ANDROID)
TEST_F(FilePathTest, ContentUriTest) {
  const struct UnaryBooleanTestData cases[] = {
      {FPL("content://foo.bar"), true},
      {FPL("content://foo.bar/"), true},
      {FPL("content://foo/bar"), true},
      {FPL("CoNTenT://foo.bar"), true},
      {FPL("content://"), true},
      {FPL("content:///foo.bar"), true},
      {FPL("content://3foo/bar"), true},
      {FPL("content://_foo/bar"), true},
      {FPL(".. "), false},
      {FPL("foo.bar"), false},
      {FPL("content:foo.bar"), false},
      {FPL("content:/foo.ba"), false},
      {FPL("content:/dir/foo.bar"), false},
      {FPL("content: //foo.bar"), false},
      {FPL("content%2a%2f%2f"), false},
  };

  for (size_t i = 0; i < std::size(cases); ++i) {
    FilePath input(cases[i].input);
    bool observed = input.IsContentUri();
    EXPECT_EQ(cases[i].expected, observed)
        << "i: " << i << ", input: " << input.value();
  }
}
#endif

// Test the operator<<(ostream, FilePath).
TEST_F(FilePathTest, PrintToOstream) {
  std::stringstream ss;
  FilePath fp(FPL("foo"));
  ss << fp;
  EXPECT_EQ("foo", ss.str());
}

#if 0
TEST_F(FilePathTest, TracedValueSupport) {
  EXPECT_EQ(perfetto::TracedValueToString(FilePath(FPL("foo"))), "foo");
}
#endif  // 0

// Test GetHFSDecomposedForm should return empty result for invalid UTF-8
// strings.
#if BUILDFLAG(IS_APPLE)
TEST_F(FilePathTest, GetHFSDecomposedFormWithInvalidInput) {
  const FilePath::CharType* cases[] = {
      FPL("\xc3\x28"),         FPL("\xe2\x82\x28"),     FPL("\xe2\x28\xa1"),
      FPL("\xf0\x28\x8c\xbc"), FPL("\xf0\x28\x8c\x28"),
  };
  for (auto* invalid_input : cases) {
    FilePath::StringType observed =
        FilePath::GetHFSDecomposedForm(invalid_input);
    EXPECT_TRUE(observed.empty());
  }
}

TEST_F(FilePathTest, CompareIgnoreCaseWithInvalidInput) {
  const FilePath::CharType* cases[] = {
      FPL("\xc3\x28"),         FPL("\xe2\x82\x28"),     FPL("\xe2\x28\xa1"),
      FPL("\xf0\x28\x8c\xbc"), FPL("\xf0\x28\x8c\x28"),
  };
  for (auto* invalid_input : cases) {
    // All example inputs will be greater than the string "fixed".
    EXPECT_EQ(FilePath::CompareIgnoreCase(invalid_input, FPL("fixed")), 1);
  }
}
#endif

}  // namespace base

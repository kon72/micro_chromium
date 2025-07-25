// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/strings/string_split.h"

#include <stddef.h>

#include <string>
#include <string_view>
#include <vector>

#include "base/strings/string_util.h"
#include "base/strings/string_view_util.h"
#include "base/strings/utf_string_conversions.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::ElementsAre;
using ::testing::Optional;
using ::testing::Pair;

namespace base {

template <typename StringPairsType>
class SplitStringIntoKeyValuePairsTest;

template <>
class SplitStringIntoKeyValuePairsTest<StringPairs> : public ::testing::Test {
 protected:
  static std::string EmptyString() { return std::string(); }

  static bool SplitStringIntoKeyValuePairs(std::string_view input,
                                           char key_value_delimiter,
                                           char key_value_pair_delimiter,
                                           StringPairs* key_value_pairs) {
    return base::SplitStringIntoKeyValuePairs(
        input, key_value_delimiter, key_value_pair_delimiter, key_value_pairs);
  }

  static bool SplitStringIntoKeyValuePairsUsingSubstr(
      std::string_view input,
      char key_value_delimiter,
      std::string_view key_value_pair_delimiter,
      StringPairs* key_value_pairs) {
    return base::SplitStringIntoKeyValuePairsUsingSubstr(
        input, key_value_delimiter, key_value_pair_delimiter, key_value_pairs);
  }
};

template <>
class SplitStringIntoKeyValuePairsTest<StringViewPairs>
    : public ::testing::Test {
 protected:
  static std::string_view EmptyString() { return std::string_view(); }

  static bool SplitStringIntoKeyValuePairs(std::string_view input,
                                           char key_value_delimiter,
                                           char key_value_pair_delimiter,
                                           StringViewPairs* key_value_pairs) {
    return SplitStringIntoKeyValueViewPairs(
        input, key_value_delimiter, key_value_pair_delimiter, key_value_pairs);
  }

  static bool SplitStringIntoKeyValuePairsUsingSubstr(
      std::string_view input,
      char key_value_delimiter,
      std::string_view key_value_pair_delimiter,
      StringViewPairs* key_value_pairs) {
    return SplitStringIntoKeyValueViewPairsUsingSubstr(
        input, key_value_delimiter, key_value_pair_delimiter, key_value_pairs);
  }
};

template <typename StringPairsType>
using SplitStringIntoKeyValuePairsUsingSubstrTest =
    SplitStringIntoKeyValuePairsTest<StringPairsType>;

using AllStringPairsTypes = ::testing::Types<StringPairs, StringViewPairs>;
TYPED_TEST_SUITE(SplitStringIntoKeyValuePairsTest, AllStringPairsTypes);
TYPED_TEST_SUITE(SplitStringIntoKeyValuePairsUsingSubstrTest,
                 AllStringPairsTypes);

TYPED_TEST(SplitStringIntoKeyValuePairsUsingSubstrTest, EmptyString) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairsUsingSubstr(
      TestFixture::EmptyString(),
      ':',  // Key-value delimiter
      ",",  // Key-value pair delimiter
      &kv_pairs));
  EXPECT_TRUE(kv_pairs.empty());
}

TYPED_TEST(SplitStringIntoKeyValuePairsUsingSubstrTest,
           MissingKeyValueDelimiter) {
  TypeParam kv_pairs;
  EXPECT_FALSE(TestFixture::SplitStringIntoKeyValuePairsUsingSubstr(
      "key1,,key2:value2",
      ':',   // Key-value delimiter
      ",,",  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_TRUE(kv_pairs[0].first.empty());
  EXPECT_TRUE(kv_pairs[0].second.empty());
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsUsingSubstrTest,
           MissingKeyValuePairDelimiter) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairsUsingSubstr(
      "key1:value1,,key3:value3",
      ':',    // Key-value delimiter
      ",,,",  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(1U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("value1,,key3:value3", kv_pairs[0].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsUsingSubstrTest, UntrimmedWhitespace) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairsUsingSubstr(
      "key1 : value1",
      ':',  // Key-value delimiter
      ",",  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(1U, kv_pairs.size());
  EXPECT_EQ("key1 ", kv_pairs[0].first);
  EXPECT_EQ(" value1", kv_pairs[0].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsUsingSubstrTest,
           OnlySplitAtGivenSeparator) {
  TypeParam kv_pairs;
  std::string a("a ?!@#$%^&*()_+:/{}\\\t\nb");
  std::string b(a + "X" + a + "XY" + a + "YX" + a);
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairsUsingSubstr(
      b,
      'X',   // Key-value delimiter
      "XY",  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ(a, kv_pairs[0].first);
  EXPECT_EQ(a, kv_pairs[0].second);
  EXPECT_EQ(a + 'Y', kv_pairs[1].first);
  EXPECT_EQ(a, kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, EmptyString) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      TestFixture::EmptyString(),
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  EXPECT_TRUE(kv_pairs.empty());
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, MissingKeyValueDelimiter) {
  TypeParam kv_pairs;
  EXPECT_FALSE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1,key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_TRUE(kv_pairs[0].first.empty());
  EXPECT_TRUE(kv_pairs[0].second.empty());
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, EmptyKeyWithKeyValueDelimiter) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      ":value1,key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_TRUE(kv_pairs[0].first.empty());
  EXPECT_EQ("value1", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, TrailingAndLeadingPairDelimiter) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      ",key1:value1,key2:value2,",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("value1", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, EmptyPair) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1:value1,,key3:value3",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("value1", kv_pairs[0].second);
  EXPECT_EQ("key3", kv_pairs[1].first);
  EXPECT_EQ("value3", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, EmptyValue) {
  TypeParam kv_pairs;
  EXPECT_FALSE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1:,key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, UntrimmedWhitespace) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1 : value1",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(1U, kv_pairs.size());
  EXPECT_EQ("key1 ", kv_pairs[0].first);
  EXPECT_EQ(" value1", kv_pairs[0].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, TrimmedWhitespace) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1:value1 , key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("value1", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, MultipleKeyValueDelimiters) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1:::value1,key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("value1", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, OnlySplitAtGivenSeparator) {
  TypeParam kv_pairs;
  std::string a("a ?!@#$%^&*()_+:/{}\\\t\nb");
  std::string b(a + "X" + a + "Y" + a + "X" + a);
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      b,
      'X',  // Key-value delimiter
      'Y',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ(a, kv_pairs[0].first);
  EXPECT_EQ(a, kv_pairs[0].second);
  EXPECT_EQ(a, kv_pairs[1].first);
  EXPECT_EQ(a, kv_pairs[1].second);
}

TYPED_TEST(SplitStringIntoKeyValuePairsTest, DelimiterInValue) {
  TypeParam kv_pairs;
  EXPECT_TRUE(TestFixture::SplitStringIntoKeyValuePairs(
      "key1:va:ue1,key2:value2",
      ':',  // Key-value delimiter
      ',',  // Key-value pair delimiter
      &kv_pairs));
  ASSERT_EQ(2U, kv_pairs.size());
  EXPECT_EQ("key1", kv_pairs[0].first);
  EXPECT_EQ("va:ue1", kv_pairs[0].second);
  EXPECT_EQ("key2", kv_pairs[1].first);
  EXPECT_EQ("value2", kv_pairs[1].second);
}

TEST(SplitStringUsingSubstrTest, EmptyString) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      std::string(), "DELIMITER", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(1u, results.size());
  EXPECT_THAT(results, ElementsAre(""));
}

TEST(SplitStringUsingSubstrTest, EmptyDelimiter) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      "TEST", std::string(), TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(1u, results.size());
  EXPECT_THAT(results, ElementsAre("TEST"));
}

TEST(StringUtilTest, SplitString_Basics) {
  std::vector<std::string> r;

  r = SplitString(std::string(), ",:;", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  EXPECT_TRUE(r.empty());

  // Empty separator list
  r = SplitString("hello, world", "", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(1u, r.size());
  EXPECT_EQ("hello, world", r[0]);

  // Should split on any of the separators.
  r = SplitString("::,,;;", ",:;", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(7u, r.size());
  for (const auto& str : r) {
    ASSERT_TRUE(str.empty());
  }

  r = SplitString("red, green; blue:", ",:;", TRIM_WHITESPACE,
                  SPLIT_WANT_NONEMPTY);
  ASSERT_EQ(3u, r.size());
  EXPECT_EQ("red", r[0]);
  EXPECT_EQ("green", r[1]);
  EXPECT_EQ("blue", r[2]);

  // Want to split a string along whitespace sequences.
  r = SplitString("  red green   \tblue\n", " \t\n", TRIM_WHITESPACE,
                  SPLIT_WANT_NONEMPTY);
  ASSERT_EQ(3u, r.size());
  EXPECT_EQ("red", r[0]);
  EXPECT_EQ("green", r[1]);
  EXPECT_EQ("blue", r[2]);

  // Weird case of splitting on spaces but not trimming.
  r = SplitString(" red ", " ", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(3u, r.size());
  EXPECT_EQ("", r[0]);  // Before the first space.
  EXPECT_EQ("red", r[1]);
  EXPECT_EQ("", r[2]);  // After the last space.
}

TEST(StringUtilTest, SplitString_WhitespaceAndResultType) {
  std::vector<std::string> r;

  // Empty input handling.
  r = SplitString(std::string(), ",", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  EXPECT_TRUE(r.empty());
  r = SplitString(std::string(), ",", KEEP_WHITESPACE, SPLIT_WANT_NONEMPTY);
  EXPECT_TRUE(r.empty());

  // Input string is space and we're trimming.
  r = SplitString(" ", ",", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(1u, r.size());
  EXPECT_EQ("", r[0]);
  r = SplitString(" ", ",", TRIM_WHITESPACE, SPLIT_WANT_NONEMPTY);
  EXPECT_TRUE(r.empty());

  // Test all 4 combinations of flags on ", ,".
  r = SplitString(", ,", ",", KEEP_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(3u, r.size());
  EXPECT_EQ("", r[0]);
  EXPECT_EQ(" ", r[1]);
  EXPECT_EQ("", r[2]);
  r = SplitString(", ,", ",", KEEP_WHITESPACE, SPLIT_WANT_NONEMPTY);
  ASSERT_EQ(1u, r.size());
  ASSERT_EQ(" ", r[0]);
  r = SplitString(", ,", ",", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(3u, r.size());
  EXPECT_EQ("", r[0]);
  EXPECT_EQ("", r[1]);
  EXPECT_EQ("", r[2]);
  r = SplitString(", ,", ",", TRIM_WHITESPACE, SPLIT_WANT_NONEMPTY);
  ASSERT_TRUE(r.empty());
}

TEST(SplitStringUsingSubstrTest, StringWithNoDelimiter) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      "alongwordwithnodelimiter", "DELIMITER", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(1u, results.size());
  EXPECT_THAT(results, ElementsAre("alongwordwithnodelimiter"));
}

TEST(SplitStringUsingSubstrTest, LeadingDelimitersSkipped) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      "DELIMITERDELIMITERDELIMITERoneDELIMITERtwoDELIMITERthree", "DELIMITER",
      TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(6u, results.size());
  EXPECT_THAT(results, ElementsAre("", "", "", "one", "two", "three"));
}

TEST(SplitStringUsingSubstrTest, ConsecutiveDelimitersSkipped) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      "unoDELIMITERDELIMITERDELIMITERdosDELIMITERtresDELIMITERDELIMITERcuatro",
      "DELIMITER", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(7u, results.size());
  EXPECT_THAT(results, ElementsAre("uno", "", "", "dos", "tres", "", "cuatro"));
}

TEST(SplitStringUsingSubstrTest, TrailingDelimitersSkipped) {
  std::vector<std::string> results = SplitStringUsingSubstr(
      "unDELIMITERdeuxDELIMITERtroisDELIMITERquatreDELIMITERDELIMITERDELIMITER",
      "DELIMITER", TRIM_WHITESPACE, SPLIT_WANT_ALL);
  ASSERT_EQ(7u, results.size());
  EXPECT_THAT(results,
              ElementsAre("un", "deux", "trois", "quatre", "", "", ""));
}

TEST(SplitStringPieceUsingSubstrTest, StringWithNoDelimiter) {
  std::vector<std::string_view> results =
      SplitStringPieceUsingSubstr("alongwordwithnodelimiter", "DELIMITER",
                                  base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(1u, results.size());
  EXPECT_THAT(results, ElementsAre("alongwordwithnodelimiter"));
}

TEST(SplitStringPieceUsingSubstrTest, LeadingDelimitersSkipped) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "DELIMITERDELIMITERDELIMITERoneDELIMITERtwoDELIMITERthree", "DELIMITER",
      base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(6u, results.size());
  EXPECT_THAT(results, ElementsAre("", "", "", "one", "two", "three"));
}

TEST(SplitStringPieceUsingSubstrTest, ConsecutiveDelimitersSkipped) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "unoDELIMITERDELIMITERDELIMITERdosDELIMITERtresDELIMITERDELIMITERcuatro",
      "DELIMITER", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(7u, results.size());
  EXPECT_THAT(results, ElementsAre("uno", "", "", "dos", "tres", "", "cuatro"));
}

TEST(SplitStringPieceUsingSubstrTest, TrailingDelimitersSkipped) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "unDELIMITERdeuxDELIMITERtroisDELIMITERquatreDELIMITERDELIMITERDELIMITER",
      "DELIMITER", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(7u, results.size());
  EXPECT_THAT(results,
              ElementsAre("un", "deux", "trois", "quatre", "", "", ""));
}

TEST(SplitStringPieceUsingSubstrTest, KeepWhitespace) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "un DELIMITERdeux\tDELIMITERtrois\nDELIMITERquatre", "DELIMITER",
      base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(4u, results.size());
  EXPECT_THAT(results, ElementsAre("un ", "deux\t", "trois\n", "quatre"));
}

TEST(SplitStringPieceUsingSubstrTest, TrimWhitespace) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "un DELIMITERdeux\tDELIMITERtrois\nDELIMITERquatre", "DELIMITER",
      base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(4u, results.size());
  EXPECT_THAT(results, ElementsAre("un", "deux", "trois", "quatre"));
}

TEST(SplitStringPieceUsingSubstrTest, SplitWantAll) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "unDELIMITERdeuxDELIMITERtroisDELIMITERDELIMITER", "DELIMITER",
      base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(5u, results.size());
  EXPECT_THAT(results, ElementsAre("un", "deux", "trois", "", ""));
}

TEST(SplitStringPieceUsingSubstrTest, SplitWantNonEmpty) {
  std::vector<std::string_view> results = SplitStringPieceUsingSubstr(
      "unDELIMITERdeuxDELIMITERtroisDELIMITERDELIMITER", "DELIMITER",
      base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  ASSERT_EQ(3u, results.size());
  EXPECT_THAT(results, ElementsAre("un", "deux", "trois"));
}

TEST(StringSplitTest, SplitStringOnce) {
  // None of the separators are in the input, so should always be std::nullopt.
  EXPECT_EQ(std::nullopt, SplitStringOnce("", ""));
  EXPECT_EQ(std::nullopt, SplitStringOnce("a", ""));
  EXPECT_EQ(std::nullopt, SplitStringOnce("ab", ""));

  EXPECT_THAT(SplitStringOnce("a:b:c", ':'), Optional(Pair("a", "b:c")));
  EXPECT_THAT(SplitStringOnce("a:", ':'), Optional(Pair("a", "")));
  EXPECT_THAT(SplitStringOnce(":b", ':'), Optional(Pair("", "b")));

  // Now the same using the multiple separators overload, but with only one
  // separator specified.
  EXPECT_THAT(SplitStringOnce("a:b:c", ":"), Optional(Pair("a", "b:c")));
  EXPECT_THAT(SplitStringOnce("a:", ":"), Optional(Pair("a", "")));
  EXPECT_THAT(SplitStringOnce(":b", ":"), Optional(Pair("", "b")));

  // Multiple separators overload, but only the first one present.
  EXPECT_THAT(SplitStringOnce("a:b:c", ":="), Optional(Pair("a", "b:c")));
  EXPECT_THAT(SplitStringOnce("a:", ":="), Optional(Pair("a", "")));
  EXPECT_THAT(SplitStringOnce(":b", ":="), Optional(Pair("", "b")));

  // Multiple separators overload, but only the second one present.
  EXPECT_THAT(SplitStringOnce("a:b:c", "=:"), Optional(Pair("a", "b:c")));
  EXPECT_THAT(SplitStringOnce("a:", "=:"), Optional(Pair("a", "")));
  EXPECT_THAT(SplitStringOnce(":b", "=:"), Optional(Pair("", "b")));

  // Multiple separators overload, both present. The separator that comes first
  // in the input string (not separators string) should win.
  EXPECT_THAT(SplitStringOnce("a:b=c", ":="), Optional(Pair("a", "b=c")));
  EXPECT_THAT(SplitStringOnce("a:b=c", "=:"), Optional(Pair("a", "b=c")));
  EXPECT_THAT(SplitStringOnce("a=b:c", ":="), Optional(Pair("a", "b:c")));
  EXPECT_THAT(SplitStringOnce("a=b:c", "=:"), Optional(Pair("a", "b:c")));
}

TEST(StringSplitTest, RSplitStringOnce) {
  // None of the separators are in the input, so should always be std::nullopt.
  EXPECT_EQ(std::nullopt, RSplitStringOnce("", ""));
  EXPECT_EQ(std::nullopt, RSplitStringOnce("a", ""));
  EXPECT_EQ(std::nullopt, RSplitStringOnce("ab", ""));

  EXPECT_THAT(RSplitStringOnce("a:b:c", ':'), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a:", ':'), Optional(Pair("a", "")));
  EXPECT_THAT(RSplitStringOnce(":b", ':'), Optional(Pair("", "b")));

  // Now the same using the multiple separators overload, but with only one
  // separator specified.
  EXPECT_THAT(RSplitStringOnce("a:b:c", ":"), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a:", ":"), Optional(Pair("a", "")));
  EXPECT_THAT(RSplitStringOnce(":b", ":"), Optional(Pair("", "b")));

  // Multiple separators overload, but only the first one present.
  EXPECT_THAT(RSplitStringOnce("a:b:c", ":="), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a:", ":="), Optional(Pair("a", "")));
  EXPECT_THAT(RSplitStringOnce(":b", ":="), Optional(Pair("", "b")));

  // Multiple separators overload, but only the second one present.
  EXPECT_THAT(RSplitStringOnce("a:b:c", "=:"), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a:", "=:"), Optional(Pair("a", "")));
  EXPECT_THAT(RSplitStringOnce(":b", "=:"), Optional(Pair("", "b")));

  // Multiple separators overload, both present. The separator that comes first
  // in the input string (not separators string) should win.
  EXPECT_THAT(RSplitStringOnce("a:b=c", ":="), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a:b=c", "=:"), Optional(Pair("a:b", "c")));
  EXPECT_THAT(RSplitStringOnce("a=b:c", ":="), Optional(Pair("a=b", "c")));
  EXPECT_THAT(RSplitStringOnce("a=b:c", "=:"), Optional(Pair("a=b", "c")));
}

TEST(StringSplitTest, StringSplitKeepWhitespace) {
  std::vector<std::string> r;

  r = SplitString("   ", "*", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
  ASSERT_EQ(1U, r.size());
  EXPECT_EQ(r[0], "   ");

  r = SplitString("\t  \ta\t ", "\t", base::KEEP_WHITESPACE,
                  base::SPLIT_WANT_ALL);
  ASSERT_EQ(4U, r.size());
  EXPECT_EQ(r[0], "");
  EXPECT_EQ(r[1], "  ");
  EXPECT_EQ(r[2], "a");
  EXPECT_EQ(r[3], " ");

  r = SplitString("\ta\t\nb\tcc", "\n", base::KEEP_WHITESPACE,
                  base::SPLIT_WANT_ALL);
  ASSERT_EQ(2U, r.size());
  EXPECT_EQ(r[0], "\ta\t");
  EXPECT_EQ(r[1], "b\tcc");
}

TEST(StringSplitTest, SplitStringAlongWhitespace) {
  struct TestData {
    const char* input;
    const size_t expected_result_count;
    const char* output1;
    const char* output2;
  } data[] = {
      {"a", 1, "a", ""},         {" ", 0, "", ""},
      {" a", 1, "a", ""},        {" ab ", 1, "ab", ""},
      {" ab c", 2, "ab", "c"},   {" ab c ", 2, "ab", "c"},
      {" ab cd", 2, "ab", "cd"}, {" ab cd ", 2, "ab", "cd"},
      {" \ta\t", 1, "a", ""},    {" b\ta\t", 2, "b", "a"},
      {" b\tat", 2, "b", "at"},  {"b\tat", 2, "b", "at"},
      {"b\t at", 2, "b", "at"},
  };
  for (const TestData& i : data) {
    std::vector<std::string> results =
        base::SplitString(i.input, kWhitespaceASCII, base::KEEP_WHITESPACE,
                          base::SPLIT_WANT_NONEMPTY);
    ASSERT_EQ(i.expected_result_count, results.size());
    if (i.expected_result_count > 0) {
      ASSERT_EQ(i.output1, results[0]);
    }
    if (i.expected_result_count > 1) {
      ASSERT_EQ(i.output2, results[1]);
    }
  }
}

TEST(StringSplitTest, NullSeparatorWantNonEmpty) {
  struct TestData {
    std::string_view test_case;
    std::vector<std::string_view> expected;
  } data[] = {
      {base::MakeStringViewWithNulChars("a"), {"a"}},
      {base::MakeStringViewWithNulChars("a\0"), {"a"}},
      {base::MakeStringViewWithNulChars("a\0a"), {"a", "a"}},
      {base::MakeStringViewWithNulChars("\0a\0\0a\0"), {"a", "a"}},
  };

  for (const auto& i : data) {
    std::vector<std::string_view> results = base::SplitStringPiece(
        i.test_case, base::MakeStringViewWithNulChars("\0"),
        base::KEEP_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
    EXPECT_EQ(results, i.expected);
  }
}

TEST(StringSplitTest, NullSeparatorWantAll) {
  struct TestData {
    std::string_view test_case;
    std::vector<std::string_view> expected;
  } data[] = {
      {base::MakeStringViewWithNulChars("a"), {"a"}},
      {base::MakeStringViewWithNulChars("a\0"), {"a", ""}},
      {base::MakeStringViewWithNulChars("a\0a"), {"a", "a"}},
      {base::MakeStringViewWithNulChars("\0a\0\0a\0"), {"", "a", "", "a", ""}},
  };

  for (const TestData& i : data) {
    std::vector<std::string_view> results = base::SplitStringPiece(
        i.test_case, base::MakeStringViewWithNulChars("\0"),
        base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
    EXPECT_EQ(results, i.expected);
  }
}

}  // namespace base

// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MEMORY_RAW_PTR_H_
#define BASE_MEMORY_RAW_PTR_H_

#include <base/compiler_specific.h>

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace partition_alloc::internal {

template <typename EnumType, typename T = EnumType>
using IfEnum = std::enable_if_t<
    std::is_enum_v<EnumType> &&
        !std::is_convertible_v<EnumType, std::underlying_type_t<EnumType>>,
    T>;

template <typename EnumType>
constexpr inline EnumType kAllFlags = static_cast<IfEnum<EnumType>>(
    (static_cast<std::underlying_type_t<EnumType>>(EnumType::kMaxValue) << 1) -
    1);

#define PA_DEFINE_OPERATORS_FOR_FLAGS(EnumType)                              \
  [[maybe_unused]] [[nodiscard]] inline constexpr EnumType operator&(        \
      const EnumType& lhs, const EnumType& rhs) {                            \
    return static_cast<EnumType>(                                            \
        static_cast<std::underlying_type_t<EnumType>>(lhs) &                 \
        static_cast<std::underlying_type_t<EnumType>>(rhs));                 \
  }                                                                          \
  [[maybe_unused]] inline constexpr EnumType& operator&=(                    \
      EnumType& lhs, const EnumType& rhs) {                                  \
    lhs = lhs & rhs;                                                         \
    return lhs;                                                              \
  }                                                                          \
  [[maybe_unused]] [[nodiscard]] inline constexpr EnumType operator|(        \
      const EnumType& lhs, const EnumType& rhs) {                            \
    return static_cast<EnumType>(                                            \
        static_cast<std::underlying_type_t<EnumType>>(lhs) |                 \
        static_cast<std::underlying_type_t<EnumType>>(rhs));                 \
  }                                                                          \
  [[maybe_unused]] inline constexpr EnumType& operator|=(                    \
      EnumType& lhs, const EnumType& rhs) {                                  \
    lhs = lhs | rhs;                                                         \
    return lhs;                                                              \
  }                                                                          \
  [[maybe_unused]] [[nodiscard]] inline constexpr EnumType operator^(        \
      const EnumType& lhs, const EnumType& rhs) {                            \
    return static_cast<EnumType>(                                            \
        static_cast<std::underlying_type_t<EnumType>>(lhs) ^                 \
        static_cast<std::underlying_type_t<EnumType>>(rhs));                 \
  }                                                                          \
  [[maybe_unused]] inline constexpr EnumType& operator^=(                    \
      EnumType& lhs, const EnumType& rhs) {                                  \
    lhs = lhs ^ rhs;                                                         \
    return lhs;                                                              \
  }                                                                          \
  [[maybe_unused]] [[nodiscard]] inline constexpr EnumType operator~(        \
      const EnumType& val) {                                                 \
    return static_cast<EnumType>(                                            \
        static_cast<std::underlying_type_t<EnumType>>(kAllFlags<EnumType>) & \
        ~static_cast<std::underlying_type_t<EnumType>>(val));                \
  }                                                                          \
  static_assert(true) /* semicolon here */

template <typename Z>
static constexpr bool is_offset_type =
    std::is_integral_v<Z> && sizeof(Z) <= sizeof(ptrdiff_t);

enum class RawPtrTraits : unsigned {
  kEmpty = 0,
  kMayDangle = (1 << 0),
  kDisableHooks = (1 << 2),
  kAllowPtrArithmetic = (1 << 3),
  kDisableBRP = (1 << 4),
  kAllowUninitialized = (1 << 5),
  kUseCountingImplForTest = (1 << 10),
  kDummyForTest = (1 << 11),
  kAllMask = kMayDangle | kDisableHooks | kAllowPtrArithmetic | kDisableBRP |
             kAllowUninitialized | kUseCountingImplForTest | kDummyForTest,
};
template <>
constexpr inline RawPtrTraits kAllFlags<RawPtrTraits> = RawPtrTraits::kAllMask;

PA_DEFINE_OPERATORS_FOR_FLAGS(RawPtrTraits);

}  // namespace partition_alloc::internal

namespace base {
using partition_alloc::internal::RawPtrTraits;

template <typename T, RawPtrTraits PointerTraits = RawPtrTraits::kEmpty>
using raw_ptr = T*;

}  // namespace base

constexpr inline auto DisableDanglingPtrDetection =
    base::RawPtrTraits::kMayDangle;
constexpr inline auto DanglingUntriaged = base::RawPtrTraits::kMayDangle;
constexpr inline auto FlakyDanglingUntriaged = base::RawPtrTraits::kMayDangle;
constexpr inline auto AcrossTasksDanglingUntriaged =
    base::RawPtrTraits::kMayDangle;
constexpr inline auto AllowPtrArithmetic =
    base::RawPtrTraits::kAllowPtrArithmetic;
constexpr inline auto AllowUninitialized =
    base::RawPtrTraits::kAllowUninitialized;
constexpr inline auto LeakedDanglingUntriaged = base::RawPtrTraits::kMayDangle;
constexpr inline auto VectorExperimental = base::RawPtrTraits::kMayDangle;
constexpr inline auto SetExperimental = base::RawPtrTraits::kMayDangle;
constexpr inline auto CtnExperimental = base::RawPtrTraits::kMayDangle;

template <typename T, base::RawPtrTraits Traits = base::RawPtrTraits::kEmpty>
using MayBeDangling = base::raw_ptr<T, Traits | base::RawPtrTraits::kMayDangle>;

#endif  // BASE_MEMORY_RAW_PTR_H_

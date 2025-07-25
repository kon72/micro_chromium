// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PARTITION_ALLOC_PARTITION_ALLOC_BASE_NUMERICS_SAFE_MATH_SHARED_IMPL_H_
#define PARTITION_ALLOC_PARTITION_ALLOC_BASE_NUMERICS_SAFE_MATH_SHARED_IMPL_H_

#include <cassert>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <type_traits>

#include "partition_alloc/build_config.h"
#include "partition_alloc/partition_alloc_base/numerics/safe_conversions.h"

#if PA_BUILDFLAG(IS_ASMJS)
// Optimized safe math instructions are incompatible with asmjs.
#define PA_BASE_HAS_OPTIMIZED_SAFE_MATH (0)
// Where available use builtin math overflow support on Clang and GCC.
#elif (defined(__clang__) &&                                \
       ((__clang_major__ > 3) ||                            \
        (__clang_major__ == 3 && __clang_minor__ >= 4))) || \
    (defined(__GNUC__) && __GNUC__ >= 5)
#include "partition_alloc/partition_alloc_base/numerics/safe_math_clang_gcc_impl.h"
#define PA_BASE_HAS_OPTIMIZED_SAFE_MATH (1)
#else
#define PA_BASE_HAS_OPTIMIZED_SAFE_MATH (0)
#endif

namespace partition_alloc::internal::base::internal {

// These are the non-functioning boilerplate implementations of the optimized
// safe math routines.
#if !PA_BASE_HAS_OPTIMIZED_SAFE_MATH
template <typename T, typename U>
struct CheckedAddFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr bool Do(T, U, V*) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<bool>();
  }
};

template <typename T, typename U>
struct CheckedSubFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr bool Do(T, U, V*) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<bool>();
  }
};

template <typename T, typename U>
struct CheckedMulFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr bool Do(T, U, V*) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<bool>();
  }
};

template <typename T, typename U>
struct ClampedAddFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr V Do(T, U) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<V>();
  }
};

template <typename T, typename U>
struct ClampedSubFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr V Do(T, U) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<V>();
  }
};

template <typename T, typename U>
struct ClampedMulFastOp {
  static const bool is_supported = false;
  template <typename V>
  static constexpr V Do(T, U) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<V>();
  }
};

template <typename T>
struct ClampedNegFastOp {
  static const bool is_supported = false;
  static constexpr T Do(T) {
    // Force a compile failure if instantiated.
    return CheckOnFailure::template HandleFailure<T>();
  }
};
#endif  // PA_BASE_HAS_OPTIMIZED_SAFE_MATH
#undef PA_BASE_HAS_OPTIMIZED_SAFE_MATH

// This is used for UnsignedAbs, where we need to support floating-point
// template instantiations even though we don't actually support the operations.
// However, there is no corresponding implementation of e.g. SafeUnsignedAbs,
// so the float versions will not compile.
template <typename Numeric,
          bool IsInteger = std::is_integral_v<Numeric>,
          bool IsFloat = std::is_floating_point_v<Numeric>>
struct UnsignedOrFloatForSize;

template <typename Numeric>
struct UnsignedOrFloatForSize<Numeric, true, false> {
  using type = typename std::make_unsigned<Numeric>::type;
};

template <typename Numeric>
struct UnsignedOrFloatForSize<Numeric, false, true> {
  using type = Numeric;
};

// Wrap the unary operations to allow SFINAE when instantiating integrals versus
// floating points. These don't perform any overflow checking. Rather, they
// exhibit well-defined overflow semantics and rely on the caller to detect
// if an overflow occurred.

template <typename T,
          typename std::enable_if<std::is_integral_v<T>>::type* = nullptr>
constexpr T NegateWrapper(T value) {
  using UnsignedT = typename std::make_unsigned<T>::type;
  // This will compile to a NEG on Intel, and is normal negation on ARM.
  return static_cast<T>(UnsignedT(0) - static_cast<UnsignedT>(value));
}

template <typename T,
          typename std::enable_if<std::is_floating_point_v<T>>::type* = nullptr>
constexpr T NegateWrapper(T value) {
  return -value;
}

template <typename T,
          typename std::enable_if<std::is_integral_v<T>>::type* = nullptr>
constexpr typename std::make_unsigned<T>::type InvertWrapper(T value) {
  return ~value;
}

template <typename T,
          typename std::enable_if<std::is_integral_v<T>>::type* = nullptr>
constexpr T AbsWrapper(T value) {
  return static_cast<T>(SafeUnsignedAbs(value));
}

template <typename T,
          typename std::enable_if<std::is_floating_point_v<T>>::type* = nullptr>
constexpr T AbsWrapper(T value) {
  return value < 0 ? -value : value;
}

template <template <typename, typename, typename> class M,
          typename L,
          typename R>
struct MathWrapper {
  using math = M<typename UnderlyingType<L>::type,
                 typename UnderlyingType<R>::type,
                 void>;
  using type = typename math::result_type;
};

// The following macros are just boilerplate for the standard arithmetic
// operator overloads and variadic function templates. A macro isn't the nicest
// solution, but it beats rewriting these over and over again.
#define PA_BASE_NUMERIC_ARITHMETIC_VARIADIC(CLASS, CL_ABBR, OP_NAME)    \
  template <typename L, typename R, typename... Args>                   \
  constexpr auto CL_ABBR##OP_NAME(const L lhs, const R rhs,             \
                                  const Args... args) {                 \
    return CL_ABBR##MathOp<CLASS##OP_NAME##Op, L, R, Args...>(lhs, rhs, \
                                                              args...); \
  }

#define PA_BASE_NUMERIC_ARITHMETIC_OPERATORS(CLASS, CL_ABBR, OP_NAME, OP,  \
                                             CMP_OP)                       \
  /* Binary arithmetic operator for all CLASS##Numeric operations. */      \
  template <typename L, typename R,                                        \
            typename std::enable_if<Is##CLASS##Op<L, R>::value>::type* =   \
                nullptr>                                                   \
  constexpr CLASS##Numeric<                                                \
      typename MathWrapper<CLASS##OP_NAME##Op, L, R>::type>                \
  operator OP(const L lhs, const R rhs) {                                  \
    return decltype(lhs OP rhs)::template MathOp<CLASS##OP_NAME##Op>(lhs,  \
                                                                     rhs); \
  }                                                                        \
  /* Assignment arithmetic operator implementation from CLASS##Numeric. */ \
  template <typename L>                                                    \
  template <typename R>                                                    \
  constexpr CLASS##Numeric<L>& CLASS##Numeric<L>::operator CMP_OP(         \
      const R rhs) {                                                       \
    return MathOp<CLASS##OP_NAME##Op>(rhs);                                \
  }                                                                        \
  /* Variadic arithmetic functions that return CLASS##Numeric. */          \
  PA_BASE_NUMERIC_ARITHMETIC_VARIADIC(CLASS, CL_ABBR, OP_NAME)

}  // namespace partition_alloc::internal::base::internal

#endif  // PARTITION_ALLOC_PARTITION_ALLOC_BASE_NUMERICS_SAFE_MATH_SHARED_IMPL_H_

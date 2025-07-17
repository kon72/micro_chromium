// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#include "absl/base/internal/raw_logging.h"
#include "absl/log/absl_log.h"
#include "base/check.h"
#include "build/build_config.h"

namespace logging {

#define DLOG(severity) ABSL_LOG_INTERNAL_DLOG_IMPL(_##severity)
#define DLOG_IF(severity, condition) \
  ABSL_LOG_INTERNAL_DLOG_IF_IMPL(_##severity, condition)

#ifndef NDEBUG
#define DPLOG(severity) ABSL_LOG_INTERNAL_PLOG_IMPL(_##severity)
#define DPLOG_IF(severity, condition) \
  ABSL_LOG_INTERNAL_PLOG_IF_IMPL(_##severity, condition)
#else
#define DPLOG(severity) ABSL_LOG_INTERNAL_PLOG_IF_IMPL(_##severity, false)
#define DPLOG_IF(severity, condition) \
  ABSL_LOG_INTERNAL_PLOG_IF_IMPL(_##severity, false && (condition))
#endif  // NDEBUG

#define DVLOG(verbose_level) ABSL_LOG_INTERNAL_DVLOG_IMPL(verbose_level)

#if BUILDFLAG(IS_WIN)
typedef unsigned long SystemErrorCode;
#elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
typedef int SystemErrorCode;
#endif

#define RAW_LOG(level, message)                                                \
  do {                                                                         \
    constexpr const char* absl_raw_log_internal_basename =                     \
        ::absl::raw_log_internal::Basename(__FILE__, sizeof(__FILE__) - 1);    \
    ::absl::raw_log_internal::RawLog(ABSL_RAW_LOG_INTERNAL_##level,            \
                                     absl_raw_log_internal_basename, __LINE__, \
                                     message);                                 \
    ABSL_RAW_LOG_INTERNAL_MAYBE_UNREACHABLE_##level;                           \
  } while (0)

}  // namespace logging

#endif  // BASE_LOGGING_H_

// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_APPLE_MACH_LOGGING_H_
#define BASE_APPLE_MACH_LOGGING_H_

#include <mach/mach.h>

#include <string>

#include "absl/log/absl_check.h"
#include "absl/log/absl_log.h"
#include "base/strings/stringprintf.h"

namespace logging {

std::string FormatMachErrorNumber(mach_error_t mach_err) {
  // For the os/kern subsystem, give the error number in decimal as in
  // <mach/kern_return.h>. Otherwise, give it in hexadecimal to make it easier
  // to visualize the various bits. See <mach/error.h>.
  if (mach_err >= 0 && mach_err < KERN_RETURN_MAX) {
    return base::StringPrintf(" (%d)", mach_err);
  }
  return base::StringPrintf(" (0x%08x)", mach_err);
}

}  // namespace logging

#define MACH_LOG(severity, mach_err)         \
  ABSL_LOG_INTERNAL_LOG_IMPL(_##severity)    \
      << ": " << mach_error_string(mach_err) \
      << ::logging::FormatMachErrorNumber(mach_err)

#define MACH_LOG_IF(severity, condition, mach_err)      \
  ABSL_LOG_INTERNAL_LOG_IF_IMPL(_##severity, condition) \
      << ": " << mach_error_string(mach_err)            \
      << ::logging::FormatMachErrorNumber(mach_err)

#define MACH_DLOG(severity, mach_err)        \
  ABSL_LOG_INTERNAL_DLOG_IMPL(_##severity)   \
      << ": " << mach_error_string(mach_err) \
      << ::logging::FormatMachErrorNumber(mach_err)

#define MACH_DLOG_IF(severity, condition, mach_err)      \
  ABSL_LOG_INTERNAL_DLOG_IF_IMPL(_##severity, condition) \
      << ": " << mach_error_string(mach_err)             \
      << ::logging::FormatMachErrorNumber(mach_err)

#define MACH_DVLOG(verbose_level, mach_err)   \
  ABSL_LOG_INTERNAL_DVLOG_IMPL(verbose_level) \
      << ": " << mach_error_string(mach_err)  \
      << ::logging::FormatMachErrorNumber(mach_err)

#ifndef NDEBUG
#define ABSL_LOG_INTERNAL_DVLOG_IF_IMPL(verbose_level, condition)            \
  switch (const int absl_logging_internal_verbose_level = (verbose_level))   \
  case 0:                                                                    \
  default:                                                                   \
    ABSL_LOG_INTERNAL_DLOG_IF_IMPL(                                          \
        _INFO,                                                               \
        (condition) && ABSL_VLOG_IS_ON(absl_logging_internal_verbose_level)) \
        .WithVerbosity(absl_logging_internal_verbose_level)
#else
#define ABSL_LOG_INTERNAL_DVLOG_IF_IMPL(verbose_level, condition)          \
  switch (const int absl_logging_internal_verbose_level = (verbose_level)) \
  case 0:                                                                  \
  default:                                                                 \
    ABSL_LOG_INTERNAL_DLOG_IF_IMPL(                                        \
        _INFO, false && (condition) &&                                     \
                   ABSL_VLOG_IS_ON(absl_logging_internal_verbose_level))   \
        .WithVerbosity(absl_logging_internal_verbose_level)
#endif
#define MACH_DVLOG_IF(verbose_level, condition, mach_err)   \
  ABSL_LOG_INTERNAL_DVLOG_IF_IMPL(verbose_level, condition) \
      << ": " << mach_error_string(mach_err)                \
      << ::logging::FormatMachErrorNumber(mach_err)

#define MACH_DCHECK(condition, mach_err)                        \
  ABSL_DCHECK(condition) << ": " << mach_error_string(mach_err) \
                         << ::logging::FormatMachErrorNumber(mach_err)

#endif  // BASE_APPLE_MACH_LOGGING_H_

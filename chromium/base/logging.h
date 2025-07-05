// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_LOGGING_H_
#define BASE_LOGGING_H_

#include "absl/log/absl_log.h"
#include "base/check.h"
#include "build/build_config.h"

namespace logging {

#define DLOG(severity) ABSL_LOG_INTERNAL_DLOG_IMPL(_##severity)
#define DLOG_IF(severity, condition) \
  ABSL_LOG_INTERNAL_DLOG_IF_IMPL(_##severity, condition)

#ifndef NDEBUG
#define DPLOG(severity) ABSL_LOG_INTERNAL_PLOG_IMPL(_##severity)
#else
#define DPLOG(severity) ABSL_LOG_INTERNAL_PLOG_IF_IMPL(_##severity, condition)
#endif  // NDEBUG

#define DVLOG(verbose_level) ABSL_LOG_INTERNAL_DVLOG_IMPL(verbose_level)

#if BUILDFLAG(IS_WIN)
typedef unsigned long SystemErrorCode;
#elif BUILDFLAG(IS_POSIX) || BUILDFLAG(IS_FUCHSIA)
typedef int SystemErrorCode;
#endif

}  // namespace logging

#endif  // BASE_LOGGING_H_

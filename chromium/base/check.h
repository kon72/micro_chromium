// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CHECK_H_
#define BASE_CHECK_H_

#include <iosfwd>

#include "absl/log/absl_check.h"
#include "base/macros/uniquify.h"

namespace logging {

// Class used to explicitly ignore an ostream, and optionally a boolean value.
class VoidifyStream {
 public:
  VoidifyStream() = default;
  explicit VoidifyStream(bool) {}

  // Binary & has lower precedence than << but higher than ?:
  void operator&(std::ostream&) {}
};

// Macro which uses but does not evaluate expr and any stream parameters.
#define EAT_CHECK_STREAM_PARAMS(expr) \
  true ? (void)0                      \
       : ::logging::VoidifyStream(expr) & (*::logging::g_swallow_stream)
extern std::ostream* g_swallow_stream;

#define CHECK(cond, ...) ABSL_CHECK(cond)

// absl::log_internal::LogMessageFatal::~LogMessageFatal() is marked as
// [[noreturn]].  Use switch statement to suppress unreachable-code warnings.
#define DCHECK(cond)                              \
  switch (int BASE_UNIQUIFY(check_internal_) = 0) \
  case 0:                                         \
  default:                                        \
    ABSL_LOG_INTERNAL_DCHECK_IMPL(                \
        BASE_UNIQUIFY(check_internal_) != 0 || (cond), #cond)

}  // namespace logging

#endif  // BASE_CHECK_H_

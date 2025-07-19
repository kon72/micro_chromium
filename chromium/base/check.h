// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CHECK_H_
#define BASE_CHECK_H_

#include <iosfwd>

#include "absl/log/absl_check.h"

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

#define CHECK(cond) ABSL_CHECK(cond)

#define DCHECK(cond) ABSL_DCHECK(cond)

}  // namespace logging

#endif  // BASE_CHECK_H_

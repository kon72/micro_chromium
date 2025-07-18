// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NOTREACHED_H_
#define BASE_NOTREACHED_H_

#include "absl/log/absl_log.h"
#include "base/macros/uniquify.h"

namespace logging {

#define NOTREACHED() ABSL_LOG(FATAL) << "NOTREACHED hit. "

// ABSL_LOG(FATAL) is marked as [[noreturn]].  Use switch statement to suppress
// unreachable-code warnings.
#define DUMP_WILL_BE_NOTREACHED()                                  \
  switch (int BASE_UNIQUIFY(notreached_internal_) = 0)             \
  case 0:                                                          \
  default:                                                         \
    ABSL_LOG_IF(FATAL, (BASE_UNIQUIFY(notreached_internal_) == 0)) \
        << "NOTREACHED hit. "

}  // namespace logging

#endif  // BASE_NOTREACHED_H_

// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NOTREACHED_H_
#define BASE_NOTREACHED_H_

#include "absl/log/absl_log.h"

namespace logging {

// ABSL_LOG(FATAL) is marked as [[noreturn]].  Use ABSL_LOG_FIRST_N to suppress
// unreachable-code warnings.

#define NOTREACHED() ABSL_LOG_FIRST_N(FATAL, 1) << "NOTREACED hit. "

#define DUMP_WILL_BE_NOTREACHED() \
  ABSL_LOG_FIRST_N(FATAL, 1) << "NOTREACHED hit. "

}  // namespace logging

#endif  // BASE_NOTREACHED_H_

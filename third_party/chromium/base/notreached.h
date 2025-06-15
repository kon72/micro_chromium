// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NOTREACHED_H_
#define BASE_NOTREACHED_H_

#include "absl/log/absl_log.h"
#include "base/compiler_specific.h"
#include "base/immediate_crash.h"

namespace logging {

// This function is used to be able to detect NOTREACHED() failures in stack
// traces where this symbol is preserved (even if inlined). Its implementation
// matches logging::CheckFailure() but intentionally uses a different signature.
[[noreturn]] NOMERGE IMMEDIATE_CRASH_ALWAYS_INLINE void NotReachedFailure() {
  base::ImmediateCrash();
}

#define NOTREACHED()                \
  do {                              \
    ::logging::NotReachedFailure(); \
  } while (false)

#define DUMP_WILL_BE_NOTREACHED() ABSL_LOG(FATAL) << "NOTREACHED hit. "

}  // namespace logging

#endif  // BASE_NOTREACHED_H_

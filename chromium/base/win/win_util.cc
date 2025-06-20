// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/win_util.h"

#include <objbase.h>

namespace base {
namespace win {

bool IsUser32AndGdi32Available() {
  static const bool is_user32_and_gdi32_available = [] {
    // If win32k syscalls aren't disabled, then user32 and gdi32 are available.
    PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY policy = {};
    if (::GetProcessMitigationPolicy(GetCurrentProcess(),
                                     ProcessSystemCallDisablePolicy, &policy,
                                     sizeof(policy))) {
      return policy.DisallowWin32kSystemCalls == 0;
    }

    return true;
  }();
  return is_user32_and_gdi32_available;
}

}  // namespace win
}  // namespace base

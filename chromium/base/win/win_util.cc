// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/win_util.h"

#include <objbase.h>

#include <strsafe.h>

#include "base/check.h"

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

std::wstring WStringFromGUID(const ::GUID& rguid) {
  // This constant counts the number of characters in the formatted string,
  // including the null termination character.
  constexpr int kGuidStringCharacters =
      1 + 8 + 1 + 4 + 1 + 4 + 1 + 4 + 1 + 12 + 1 + 1;
  wchar_t guid_string[kGuidStringCharacters];
  CHECK(SUCCEEDED(StringCchPrintfW(
      guid_string, kGuidStringCharacters,
      L"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", rguid.Data1,
      rguid.Data2, rguid.Data3, rguid.Data4[0], rguid.Data4[1], rguid.Data4[2],
      rguid.Data4[3], rguid.Data4[4], rguid.Data4[5], rguid.Data4[6],
      rguid.Data4[7])));
  return std::wstring(guid_string, kGuidStringCharacters - 1);
}

}  // namespace win
}  // namespace base

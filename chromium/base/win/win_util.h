// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_WIN_WIN_UTIL_H_
#define BASE_WIN_WIN_UTIL_H_

#include "base/base_export.h"

namespace base {
namespace win {

// Returns true if the current process can make USER32 or GDI32 calls such as
// CreateWindow and CreateDC. Windows 8 and above allow the kernel component
// of these calls to be disabled (also known as win32k lockdown) which can
// cause undefined behaviour such as crashes. This function can be used to
// guard areas of code using these calls and provide a fallback path if
// necessary.
// Because they are not always needed (and not needed at all in processes that
// have the win32k lockdown), USER32 and GDI32 are delayloaded. Attempts to
// load them in those processes will cause a crash. Any code which uses USER32
// or GDI32 and may run in a locked-down process MUST be guarded using this
// method. Before the dlls were delayloaded, method calls into USER32 and GDI32
// did not work, so adding calls to this method to guard them simply avoids
// unnecessary method calls.
BASE_EXPORT bool IsUser32AndGdi32Available();

}  // namespace win
}  // namespace base

#endif  // BASE_WIN_WIN_UTIL_H_

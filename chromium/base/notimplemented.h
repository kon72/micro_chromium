// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_NOTIMPLEMENTED_H_
#define BASE_NOTIMPLEMENTED_H_

#include "absl/log/absl_log.h"

#define NOTIMPLEMENTED() \
  ABSL_DLOG(ERROR) << "Not implemented reached in " << __PRETTY_FUNCTION__;

#endif  // BASE_NOTIMPLEMENTED_H_

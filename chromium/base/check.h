// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CHECK_H_
#define BASE_CHECK_H_

#include "absl/log/absl_check.h"

#define CHECK(cond) ABSL_CHECK(cond)

#define DCHECK(cond) ABSL_DCHECK(cond)

#endif  // BASE_CHECK_H_

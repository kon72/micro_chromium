// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CHECK_OP_H_
#define BASE_CHECK_OP_H_

#include "absl/log/absl_check.h"

#define CHECK_EQ(val1, val2) ABSL_CHECK_EQ(val1, val2)
#define CHECK_NE(val1, val2) ABSL_CHECK_NE(val1, val2)
#define CHECK_LE(val1, val2) ABSL_CHECK_LE(val1, val2)
#define CHECK_LT(val1, val2) ABSL_CHECK_LT(val1, val2)
#define CHECK_GE(val1, val2) ABSL_CHECK_GE(val1, val2)
#define CHECK_GT(val1, val2) ABSL_CHECK_GT(val1, val2)

#define DCHECK_EQ(val1, val2) ABSL_DCHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2) ABSL_DCHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2) ABSL_DCHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2) ABSL_DCHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2) ABSL_DCHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2) ABSL_DCHECK_GT(val1, val2)

#endif  // BASE_CHECK_OP_H_

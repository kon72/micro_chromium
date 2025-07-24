// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_METRICS_HISTOGRAM_MACROS_H_
#define BASE_METRICS_HISTOGRAM_MACROS_H_

#define SCOPED_UMA_HISTOGRAM_TIMER_MICROS(name) \
  do {                                          \
    (void)name;                                 \
  } while (0)

#endif  // BASE_METRICS_HISTOGRAM_MACROS_H_

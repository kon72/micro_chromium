load("@rules_cc//cc:cc_library.bzl", "cc_library")

def chromium_cc_library(
        name,
        testonly = False,
        srcs = [],
        hdrs = [],
        visibility = None,
        deps = []):
    cc_library(
        name = name,
        testonly = testonly,
        srcs = srcs,
        hdrs = hdrs,
        visibility = visibility,
        deps = deps + ["//third_party/chromium:defs"],
    )

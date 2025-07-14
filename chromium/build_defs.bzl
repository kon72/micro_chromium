load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:objc_library.bzl", "objc_library")

def chromium_cc_opts():
    return select({
        "@platforms//os:windows": [
            "-DWINVER=0x0A00",
            "-D_WIN32_WINNT=0x0A00",
        ],
        "//conditions:default": ["-fno-exceptions"],
    }) + select({
        "@rules_cc//cc/compiler:clang-cl": ["-Wno-macro-redefined"],
        "//conditions:default": [],
    })

def chromium_cc_library(
        name,
        testonly = False,
        srcs = [],
        hdrs = [],
        copts = [],
        linkopts = [],
        target_compatible_with = [],
        textual_hdrs = [],
        visibility = None,
        deps = []):
    cc_library(
        name = name,
        testonly = testonly,
        srcs = srcs,
        hdrs = hdrs,
        copts = chromium_cc_opts() + copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,
        textual_hdrs = textual_hdrs,
        visibility = visibility,
        deps = deps + ["//chromium:defs"],
    )

def chromium_objc_library(
        name,
        testonly = False,
        srcs = [],
        hdrs = [],
        copts = [],
        linkopts = [],
        target_compatible_with = [],
        textual_hdrs = [],
        visibility = None,
        deps = []):
    objc_library(
        name = name,
        testonly = testonly,
        srcs = srcs,
        hdrs = hdrs,
        copts = chromium_cc_opts() + copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,
        textual_hdrs = textual_hdrs,
        visibility = visibility,
        deps = deps + ["//chromium:defs"],
    )

def chromium_cc_test(
        name,
        size = "medium",
        srcs = [],
        copts = [],
        linkopts = [],
        target_compatible_with = [],
        visibility = None,
        deps = []):
    cc_test(
        name = name,
        size = size,
        srcs = srcs,
        copts = chromium_cc_opts() + copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,
        visibility = visibility,
        deps = deps + ["//chromium:defs"],
    )

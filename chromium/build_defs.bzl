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

def chromium_cc_or_objc_library(
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
    cc_lib_name = "%s_cc" % name
    objc_lib_name = "%s_objc" % name

    chromium_cc_library(
        name = cc_lib_name,
        testonly = testonly,
        srcs = srcs,
        hdrs = hdrs,
        copts = copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with + select({
            "@platforms//os:macos": ["@platforms//:incompatible"],
            "@platforms//os:ios": ["@platforms//:incompatible"],
            "//conditions:default": [],
        }),
        textual_hdrs = textual_hdrs,
        visibility = ["//visibility:private"],
        deps = deps,
    )

    chromium_objc_library(
        name = objc_lib_name,
        testonly = testonly,
        srcs = srcs,
        hdrs = hdrs,
        copts = copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with + select({
            "@platforms//os:macos": [],
            "@platforms//os:ios": [],
            "//conditions:default": ["@platforms//:incompatible"],
        }),
        textual_hdrs = textual_hdrs,
        visibility = ["//visibility:private"],
        deps = deps,
    )

    native.alias(
        name = name,
        testonly = testonly,
        actual = select({
            "@platforms//os:macos": objc_lib_name,
            "@platforms//os:ios": objc_lib_name,
            "//conditions:default": cc_lib_name,
        }),
        target_compatible_with = target_compatible_with,
        visibility = visibility,
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

def chromium_objc_test(
        name,
        size = "medium",
        srcs = [],
        copts = [],
        linkopts = [],
        target_compatible_with = [],
        visibility = None,
        deps = []):
    test_lib_name = "%s_lib" % name

    chromium_objc_library(
        name = test_lib_name,
        testonly = True,
        srcs = srcs,
        copts = copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,
        visibility = ["//visibility:private"],
        deps = deps,
    )

    chromium_cc_test(
        name = name,
        size = size,
        target_compatible_with = target_compatible_with,
        visibility = visibility,
        deps = [test_lib_name],
    )

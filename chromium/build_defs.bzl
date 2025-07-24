load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_cc//cc:objc_library.bzl", "objc_library")

def chromium_cc_copts():
    copts = select({
        "@platforms//os:chromiumos": ["-DSYSTEM_NATIVE_UTF8"],
        "@platforms//os:fuchsia": ["-DSYSTEM_NATIVE_UTF8"],
        "@platforms//os:macos": ["-DSYSTEM_NATIVE_UTF8"],
        "@platforms//os:ios": ["-DSYSTEM_NATIVE_UTF8"],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:linux": ["-DUSE_SYMBOLIZE"],
        "@platforms//os:chromiumos": ["-DUSE_SYMBOLIZE"],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:windows": [
            "/W4",
            "-D_HAS_NODISCARD",
        ],
        "//conditions:default": [
            "-Wall",
            "-Wextra",
            "-D__STDC_CONSTANT_MACROS",
            "-D__STDC_FORMAT_MACROS",
        ],
    }) + select({
        "//chromium/bazel/config:is_clang": [
            "-Wimplicit-fallthrough",
            "-Wextra-semi",
            "-Wunreachable-code-aggressive",
            "-Wgnu",
            "-Wno-gnu-statement-expression",
            "-Wno-zero-length-array",
            "-Wthread-safety",
        ],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:windows": ["-D_CRT_NONSTDC_NO_WARNINGS"],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:android": ["-Wunguarded-availability"],
        "@platforms//os:macos": ["-Wunguarded-availability"],
        "@platforms//os:ios": [
            "-Wunguarded-availability",
            "-Wundeclared-selector",
        ],
        "//conditions:default": [],
    }) + [
        "-Wno-missing-field-initializers",
        "-Wno-unused-parameter",
    ] + select({
        "//chromium/bazel/config:is_clang": [
            "-Wloop-analysis",
            "-Wno-unneeded-internal-declaration",
            "-Wenum-compare-conditional",
            "-Wshadow",
        ],
        "//conditions:default": [],
    }) + ["-fno-ident"] + select({
        "@platforms//os:windows": ["/clang:-fno-math-errno"],
        "//conditions:default": ["-fno-math-errno"],
    }) + select({
        "@platforms//os:windows": [],
        "//conditions:default": ["-fno-strict-aliasing"],
    }) + select({
        "@platforms//os:android": [],
        "@platforms//os:macos": [],
        "@platforms//os:ios": [],
        "@platforms//os:windows": [],
        "//conditions:default": [
            "-D_FILE_OFFSET_BITS=64",
            "-D_LARGEFILE_SOURCE",
            "-D_LARGEFILE64_SOURCE",
        ],
    }) + select({
        "@platforms//os:linux": ["-D_GNU_SOURCE"],
        "@platforms//os:android": ["-D_GNU_SOURCE"],
        "@platforms//os:chromiumos": ["-D_GNU_SOURCE"],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:windows": [
            "/bigobj",
            "/utf-8",
            "-fmsc-version=1934",
        ],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:windows": [
            # Defines that set up the CRT.
            "-D__STD_C",
            "-D_CRT_RAND_S",
            "-D_CRT_SECURE_NO_DEPRECATE",
            "-D_SCL_SECURE_NO_DEPRECATE",
            # Defines that set up the Windows SDK.
            "-D_ATL_NO_OPENGL",
            "-D_WINDOWS",
            "-DCERT_CHAIN_PARA_HAS_EXTRA_FIELDS",
            "-DPSAPI_VERSION=2",
            "-DWIN32",
            "-D_SECURE_ATL",
        ],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:linux": [
            "-DUSE_UDEV",
            "-DUSE_AURA=1",
            "-DUSE_GLIB=1",
            "-DUSE_OZONE=1",
            "-DGLIB_VERSION_MAX_ALLOWED=GLIB_VERSION_2_56",
            "-DGLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_56",
        ],
        "@platforms//os:fuchsia": [
            "-DUSE_AURA=1",
            "-DUSE_OZONE=1",
        ],
        "@platforms//os:chromiumos": [
            "-DUSE_UDEV=1",
            "-DUSE_AURA=1",
            "-DUSE_OZONE=1",
        ],
        "@platforms//os:windows": ["-DUSE_AURA=1"],
        "//conditions:default": [],
    }) + select({
        "@platforms//os:windows": [
            "-DWIN32_LEAN_AND_MEAN",
            "-DNOMINMAX",
            "-D_UNICODE",
            "-DUNICODE",
            "-DNTDDI_VERSION=NTDDI_WIN11_GE",
            "-U_WIN32_WINNT",
            "-D_WIN32_WINNT=0x0A00",
            "-DWINVER=0x0A00",
        ],
        "//conditions:default": [],
    }) + select({
        "//chromium/bazel/config:is_clang": [
            "-Wheader-hygiene",
            "-Wstring-conversion",
            "-Wtautological-overlap-compare",
        ],
        "//conditions:default": [],
    }) + select({
        "//chromium/bazel/config:is_clang": [
            "-Wimplicit-int-conversion",
            "-Wshorten-64-to-32",
            "-Wsign-compare",
            "-Wsign-conversion",
            "-Wtautological-unsigned-zero-compare",
            "-Wexit-time-destructors",
            "-Wglobal-constructors",
        ],
        "//conditions:default": [],
    })

    return copts

def chromium_cc_cxxopts():
    return select({
        "@platforms//os:windows": [
            "/EHs-c-",
            "-D_HAS_EXCEPTIONS=0",
            "/GR-",
        ],
        "//conditions:default": [
            "-fno-exceptions",
            "-fno-rtti",
        ],
    })

def chromium_objc_copts():
    return chromium_cc_copts() + [
        "-fobjc-arc",
        "-fno-objc-arc-exceptions",
    ] + select({
        "@platforms//os:macos": [
            "-Wimplicit-retain-self",
            "-Wobjc-missing-property-synthesis",
            "-Wobjc-property-assign-on-object-type",
        ],
        "@platforms//os:ios": [
            "-Wimplicit-retain-self",
            "-Widiomatic-parentheses",
        ],
        "//conditions:default": [],
    })

def chromium_objc_cxxopts():
    return chromium_cc_cxxopts()

def chromium_cc_library(
        name,
        testonly = False,
        srcs = [],
        hdrs = [],
        copts = [],
        cxxopts = [],
        defines = [],
        features = [],
        implementation_deps = [],
        includes = [],
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
        copts = chromium_cc_copts() + copts,
        cxxopts = chromium_cc_cxxopts() + cxxopts,
        defines = defines,
        features = features,
        implementation_deps = implementation_deps,
        includes = includes,
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
        cxxopts = [],
        defines = [],
        features = [],
        implementation_deps = [],
        includes = [],
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
        copts = chromium_objc_copts() + copts,
        cxxopts = chromium_objc_cxxopts() + cxxopts,
        defines = defines,
        features = features,
        implementation_deps = implementation_deps,
        includes = includes,
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
        cxxopts = [],
        defines = [],
        features = [],
        implementation_deps = [],
        includes = [],
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
        cxxopts = cxxopts,
        defines = defines,
        features = features,
        implementation_deps = implementation_deps,
        includes = includes,
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
        cxxopts = cxxopts,
        defines = defines,
        features = features,
        implementation_deps = implementation_deps,
        includes = includes,
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
        cxxopts = [],
        linkopts = [],
        target_compatible_with = [],
        visibility = None,
        deps = []):
    cc_test(
        name = name,
        size = size,
        srcs = srcs,
        copts = chromium_cc_copts() + select({
            "//chromium/bazel/config:is_clang": [
                "-Wno-implicit-int-conversion",
                "-Wno-shorten-64-to-32",
                "-Wno-sign-compare",
                "-Wno-sign-conversion",
                "-Wno-exit-time-destructors",
                "-Wno-global-constructors",
            ],
            "//conditions:default": [],
        }) + copts,
        cxxopts = chromium_cc_cxxopts() + cxxopts,
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
        cxxopts = [],
        linkopts = [],
        target_compatible_with = [],
        visibility = None,
        deps = []):
    test_lib_name = "%s_lib" % name

    chromium_objc_library(
        name = test_lib_name,
        testonly = True,
        srcs = srcs,
        copts = select({
            "//chromium/bazel/config:is_clang": [
                "-Wno-implicit-int-conversion",
                "-Wno-shorten-64-to-32",
                "-Wno-sign-compare",
                "-Wno-sign-conversion",
                "-Wno-exit-time-destructors",
                "-Wno-global-constructors",
            ],
            "//conditions:default": [],
        }) + copts,
        cxxopts = cxxopts,
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

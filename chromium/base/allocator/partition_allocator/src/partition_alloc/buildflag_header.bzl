load("//chromium:build_defs.bzl", "chromium_cc_library")
load("//chromium/bazel:gen_buildflag_header.bzl", "gen_buildflag_header")

def pa_buildflag_header(
        name,
        flags = {},
        visibility = []):
    out_h = "%s.h" % name

    gen_buildflag_header(
        name = "%s_header" % name,
        out = out_h,
        flags = flags,
        buildflag_macro_hdr = "partition_alloc/buildflag.h",
        flag_prefix = "PA_BUILDFLAG_INTERNAL_",
    )

    chromium_cc_library(
        name = name,
        hdrs = [out_h],
        visibility = visibility,
        deps = [
            "//chromium/base/allocator/partition_allocator/src/partition_alloc:defs",
            "//chromium/base/allocator/partition_allocator/src/partition_alloc:buildflag_macro",
        ],
    )

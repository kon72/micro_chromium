#!/usr/bin/env bash

set -euo pipefail

upstream_url='https://chromium.googlesource.com/chromium/src'
revision='ba5e91c3c31fb669ddb0eaaaf8d9c356f6cade70'
checkout_dirs=(
  'base'
  'build'
  'testing/gmock'
  'testing/gtest'
)
files=(
  '.clang-format'
  'base/base_export.h'
  'base/bits.h'
  'base/compiler_specific.h'
  'base/containers/contains.h'
  # 'base/containers/contains_nocompile.nc'
  # 'base/containers/contains_unittest.cc'
  'base/containers/checked_iterators.h'
  # 'base/containers/checked_iterators_nocompile.nc'
  # 'base/containers/checked_iterators_unittest.cc'
  'base/containers/span.h'
  'base/containers/span_forward_internal.h'
  # 'base/containers/span_nocompile.nc'
  # 'base/containers/span_unittest.cc'
  'base/memory/raw_ptr_exclusion.h'
  'base/no_destructor.h'
  # 'base/no_destructor_nocompile.nc'
  # 'base/no_destructor_unittest.cc'
  'base/numerics/integral_constant_like.h'
  'base/numerics/safe_conversions.h'
  'base/numerics/safe_conversions_arm_impl.h'
  'base/numerics/safe_conversions_impl.h'
  'base/scoped_generic.h'
  'base/scoped_generic_unittest.cc'
  'base/strings/strcat.cc'
  'base/strings/strcat.h'
  'base/strings/strcat_internal.h'
  'base/strings/strcat_unittest.cc'
  'base/strings/strcat_win.cc'
  'base/strings/strcat_win.h'
  'base/strings/string_util.cc'
  'base/strings/string_util.h'
  'base/strings/string_util_constants.cc'
  'base/strings/string_util_impl_helpers.h'
  'base/strings/string_util_internal.h'
  # 'base/strings/string_util_perftest.cc'
  'base/strings/string_util_posix.h'
  'base/strings/string_util_unittest.cc'
  'base/strings/string_util_win.cc'
  'base/strings/string_util_win.h'
  'base/strings/string_view_util.h'
  # 'base/strings/string_view_util_nocompile.nc'
  'base/strings/string_view_util_unittest.cc'
  'base/strings/to_string.cc'
  'base/strings/to_string.h'
  # 'base/strings/to_string_unittest.cc'
  'base/strings/utf_ostream_operators.cc'
  'base/strings/utf_ostream_operators.h'
  'base/strings/utf_string_conversion_utils.cc'
  'base/strings/utf_string_conversion_utils.h'
  'base/strings/utf_string_conversion_utils_unittest.cc'
  'base/strings/utf_string_conversions.cc'
  'base/strings/utf_string_conversions.h'
  # 'base/strings/utf_string_conversions_fuzzer.cc'
  'base/strings/utf_string_conversions_unittest.cc'
  'base/third_party/icu/icu_utf.h'
  'base/types/always_false.h'
  'base/types/supports_ostream_operator.h'
  'base/types/supports_ostream_operator_unittest.cc'
  'base/types/supports_to_string.h'
  'base/types/to_address.h'
  'base/types/to_address_unittest.cc'
  'build/build_config.h'
  'build/buildflag.h'
  'testing/gmock/include/gmock/gmock.h'
  'testing/gtest/include/gtest/gtest.h'
  'LICENSE'
)

upstream_dir='third_party/chromium-upstream'
out_dir='third_party/chromium'

git -C "${upstream_dir}" rev-parse --is-inside-work-tree > /dev/null 2>&1 || {
  git clone --sparse --depth=1 --revision="${revision}" \
    "${upstream_url}" "${upstream_dir}"
  git -C "${upstream_dir}" sparse-checkout set "${checkout_dirs[@]}"
}
git -C "${upstream_dir}" checkout "${revision}" || {
  echo "Failed to checkout revision ${revision} in ${upstream_dir}" >&2
  exit 1
}
if ! git -C "${upstream_dir}" diff --quiet; then
  echo "The repository ${upstream_dir} is dirty" >&2
  exit 1
fi

echo "Syncing upstream files to ${out_dir}..."
fd --unrestricted --type=file \
  --exclude='**/BUILD.bazel' \
  --exclude='build_defs.bzl' \
  --exec rm -f {} \; \
  . "${out_dir}"
find "${out_dir}" -type d -empty -delete
for file in "${files[@]}"; do
  src="${upstream_dir}/${file}"
  dst="${out_dir}/${file}"
  mkdir -p "$(dirname "${dst}")"
  if [[ -f ${src} ]]; then
    cp "${src}" "${dst}"
  else
    echo "File ${src} does not exist, skipping." >&2
  fi
done

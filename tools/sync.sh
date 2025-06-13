#!/usr/bin/env bash

set -euo pipefail

upstream_url='https://chromium.googlesource.com/chromium/src'
revision='ba5e91c3c31fb669ddb0eaaaf8d9c356f6cade70'
checkout_dirs=(
  'base'
  'build'
  'testing/gtest'
)
files=(
  '.clang-format'
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
  'base/numerics/integral_constant_like.h'
  'base/numerics/safe_conversions.h'
  'base/numerics/safe_conversions_arm_impl.h'
  'base/numerics/safe_conversions_impl.h'
  'base/scoped_generic.h'
  'base/scoped_generic_unittest.cc'
  'base/strings/string_view_util.h'
  # 'base/strings/string_view_util_nocompile.nc'
  'base/strings/string_view_util_unittest.cc'
  'base/types/to_address.h'
  'base/types/to_address_unittest.cc'
  'build/build_config.h'
  'build/buildflag.h'
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

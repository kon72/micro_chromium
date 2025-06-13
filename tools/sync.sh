#!/usr/bin/env bash

set -euo pipefail

download_file() {
  local repo_url="$1"
  local revision="$2"
  local file="$3"
  local out_dir="$4"

  echo "- ${file}" >&2
  local blob_url="${repo_url}/+/${revision}/${file}?format=text"
  local out_file="${out_dir}/${file}"
  mkdir -p "$(dirname "${out_file}")"
  curl --silent --show-error --fail "${blob_url}" \
    | base64 --decode > "${out_file}"
}

repo_url='https://chromium.googlesource.com/chromium/src'
revision='ba5e91c3c31fb669ddb0eaaaf8d9c356f6cade70'
files=(
  '.clang-format'
  'base/compiler_specific.h'
  'base/containers/contains.h'
  'base/numerics/integral_constant_like.h'
  'base/numerics/safe_conversions.h'
  'base/numerics/safe_conversions_arm_impl.h'
  'base/numerics/safe_conversions_impl.h'
  'base/scoped_generic.h'
  'base/scoped_generic_unittest.cc'
  'base/types/to_address.h'
  'base/types/to_address_unittest.cc'
  'build/build_config.h'
  'build/buildflag.h'
  'testing/gtest/include/gtest/gtest.h'
  'LICENSE'
)

temp_dir="$(mktemp -d)"
trap 'rm -rf "${temp_dir}"' EXIT
mkdir "${temp_dir}/chromium"

echo "Downloading Chromium files into ${temp_dir}/chromium" >&2
for file in "${files[@]}"; do
  download_file "${repo_url}" "${revision}" "${file}" "${temp_dir}/chromium"
done

echo "Syncing downloaded files to third_party directory" >&2
rsync -aP --delete \
  --exclude="BUILD.bazel" \
  --exclude="build_defs.bzl" \
  "${temp_dir}/chromium" third_party

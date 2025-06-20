# Adopted from https://github.com/ungoogled-software/ungoogled-chromium/blob/47bc6ecca740b3a700865e0c1418a1802af4bba2/devutils/set_quilt_vars.sh

# Sets quilt variables for updating the patches
# Make sure to run this with the shell command "source" in order to inherit the variables into the interactive environment

quilt() {
  if [[ "$(pwd -P || true)" != */chromium ]]; then
    echo "Error: This script must be run from the 'chromium' directory." >&2
    return 1
  fi

  # There is some problem with the absolute paths in QUILT_PATCHES and QUILT_SERIES breaking quilt
  # (refresh and diff don't read QUILT_*_ARGS, and series displays absolute paths instead of relative)
  # Specifying a quiltrc file fixes this, so "--quiltrc -" fixes this too.
  # One side effect of '--quiltrc -' is that we lose default settings from /etc/quilt.quiltrc, so they are redefined below.
  command quilt --quiltrc - "$@"
}

# Assume this script lives within the repository
REPO_ROOT=..

export QUILT_PATCHES="${REPO_ROOT}/patches"
#export QUILT_SERIES=$(readlink -f "$REPO_ROOT/patches/series")

# Options below borrowed from Debian and default quilt options (from /etc/quilt.quiltrc on Debian)
export QUILT_PUSH_ARGS="--color=auto"
export QUILT_DIFF_OPTS="--show-c-function"
export QUILT_PATCH_OPTS="--unified --reject-format=unified"
export QUILT_DIFF_ARGS="-p ab --no-timestamps --no-index --color=auto --sort"
export QUILT_REFRESH_ARGS="-p ab --no-timestamps --no-index --sort --strip-trailing-whitespace"
export QUILT_COLORS="diff_hdr=1;32:diff_add=1;34:diff_rem=1;31:diff_hunk=1;33:diff_ctx=35:diff_cctx=33"
export QUILT_SERIES_ARGS="--color=auto"
export QUILT_PATCHES_ARGS="--color=auto"

export LC_ALL=C
# When non-default less options are used, add the -R option so that less outputs
# ANSI color escape codes "raw".
[[ -n ${LESS-} && -z ${QUILT_PAGER+x} ]] && export QUILT_PAGER="less -FRX"

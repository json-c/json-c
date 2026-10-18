#!/bin/bash
#
# Step 2 of creating a release:
#  We've decided a release is worth doing
#   Update automatic bits
#   Collect information needed to populate changelog, etc...

set -e -o pipefail

RUNDIR=$(dirname "$0")

cd "${RUNDIR}"

./gen_issues_closed.sh

../abi-check.sh

./update_authors.sh

./check_symbols.sh || true


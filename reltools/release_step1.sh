#!/bin/bash
#
# Step 1 of creating a release:
#  Determine whether a release is worth doing

set -e -o pipefail

RUNDIR=$(dirname "$0")

. "${RUNDIR}/reltools.subr"
set_rel_vars

cat <<EOF
RELTAG=${RELTAG}
RELVER=${RELVER}
RELBRANCH=${RELBRANCH}
RELBASE=${RELBASE}
NEXT_RELVER=${NEXT_RELVER}

Analyze previous release branch to see if anything should have been
 applied to master:

git log -r "${RELBASE}..${RELBRANCH}" | less

Check commit logs between previous branch point and now for notable changes.

git log --reverse -r "${RELBASE}"..master | less

Starting shell:
EOF

$SHELL

echo "$0: done"

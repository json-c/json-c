#!/bin/bash

set -e -o pipefail

RUNDIR=$(dirname "$0")
. "${RUNDIR}/reltools.subr"

NEXT_VER=$(get_next_relver)

MOST_RECENT=$(ls ${RUNDIR}/../issues_closed*md | sort -V | tail -1)
PREV=$(grep 'NOW=' "${MOST_RECENT}" | sed -e's/.*=//')
NOW=$(date +%Y-%m-%d)
set -x -v
curl -v "https://api.github.com/search/issues?q=repo%3Ajson-c%2Fjson-c+closed%3A>${PREV}+created%3A<${NOW}&sort=created&order=asc&per_page=100&page=1" > issues1.out

set -o noclobber
NEW_FILE=issues_closed_for_${NEXT_VER}.md
cat <<EOF > "${NEW_FILE}"
This list was created with:
\`\`\`
# PREV=${PREV}
# NOW=${NOW}
./reltools/gen_issues_closed.sh
\`\`\`

EOF

jq -r '.items[] | "[" + .title + "](" + .url + ")" | tostring' issues1.out >> "${NEW_FILE}"
sed -e's,^\[ *\(.*\)\](https://api.github.com/.*/\([0-9].*\)),* [Issue #\2](https://github.com/json-c/json-c/issues/\2) - \1,' -i "${NEW_FILE}"
echo "${NEW_FILE}"


#!/bin/bash

set -e -o pipefail

RUNDIR=$(dirname "$0")
TOP=${RUNDIR}/..
. "${RUNDIR}/reltools.subr"

update_authors()
{
	PREV=$(get_most_recent_reltag)
	NEW_AUTHORS=$( git log -r ${PREV}..HEAD | grep Author: | sed -e's/Author: //' ; cat "${TOP}/AUTHORS" )
	echo "${NEW_AUTHORS}" | sort -u > "${TOP}/AUTHORS"
}

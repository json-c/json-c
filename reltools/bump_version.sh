#!/bin/bash
#

set -e -o pipefail
trap 'echo "ERROR: Unexpected exit in $0"' EXIT

RUNDIR=$(dirname "$0")
RUNDIR=$(realpath "${RUNDIR}")
. "${RUNDIR}/reltools.subr"

usage()
{
	local exitval="$1" ; shift
	local errmsg="$1"
	if [ "${exitval}" -ne 0 ] ; then
		exec 1>&2
	fi
	if [ -n "${errmsg}" ] ; then
		echo "ERROR: ${errmsg}" 1>&2
	fi
	cat <<EOF
Usage: $0 --help|--dev|--release

Bump version in all necessary locations, to either a release or dev version

Run this script from the top-level directory of a json-c clone.

EOF
	trap - EXIT
	exit "${exitval}"
}


bump_release_ver()
{
	echo "ERROR: bump_release_ver() not yet implemented in $0" 1>&2
	echo "See RELEASE_CHECKLIST.txt for steps to take" 1>&2
	trap - EXIT
	exit 1
}

bump_dev_ver()
{
	if [ -n "$(git status --porcelain | grep -v reltools/bump_version.sh)" ] ; then
		usage 1 "local changes in working directory, aborting"
	fi

	RELVER=$(get_most_recent_relver)
	DEVVER=${RELVER}.99
	NEXTVER=$(get_next_relver)

	if ! grep -q "^${NEXTVER}" ChangeLog ; then
		# Add new section to ChangeLog for ${release}+1
		cat <<EOF > ChangeLog.new
${NEXTVER} (future release)
========================================
Deprecated and removed features:
--------------------------------
* none yet

New features
------------
* none yet

Significant changes and bug fixes
---------------------------------
* none yet

EOF
		cat ChangeLog >> ChangeLog.new
		mv ChangeLog.new ChangeLog
	else
		echo "Warning: ChangeLog already contains a ${NEXTVER} section" 1>&2
	fi


	# Use ${release}.99 to indicate a version "newer" than anything on the branch:
	if ! grep -q ${RELVER}.99 json_c_version.h ; then
		local MAJOR MINOR
		MAJOR=${RELVER%%.*}
		MINOR=${RELVER#*.}
		echo "Updating json_c_version.h to ${RELVER}.99"
		sed -i \
			-e "s/#define JSON_C_MAJOR_VERSION .*/#define JSON_C_MAJOR_VERSION ${MAJOR}/" \
			-e "s/#define JSON_C_MINOR_VERSION .*/#define JSON_C_MINOR_VERSION ${MINOR}/" \
			-e "s/#define JSON_C_MICRO_VERSION .*/#define JSON_C_MICRO_VERSION 99/"       \
			-e "s/#define JSON_C_VERSION \".*\"/#define JSON_C_VERSION \"${RELVER}.99\"/" \
			json_c_version.h
	else
		echo "Warning: json_c_version.h already contains ${RELVER}.99" 1>&2
	fi

	if ! grep -q "${RELVER}.99" CMakeLists.txt ; then
		echo "Updating CMakeLists.txt to ${RELVER}.99"
		sed -i \
			-e "s/project(\(.*\) VERSION .*)/project(\1 VERSION ${RELVER}.99)/" \
			CMakeLists.txt
	else
		echo "Warning: CMakeLists.txt already contains ${RELVER}.99" 1>&2
	fi

	local RELTAG
	RELTAG=$(get_most_recent_reltag)
	echo "Extracting VERSION, SOVERSION from ${RELTAG}:CMakeLists.txt"

	local REL_SOVER REL_SOVERNUM
	REL_SOVER=$(git show ${RELTAG}:CMakeLists.txt | awk '/set_target_prop.*PROJECT_NAME/,/)/')
	REL_VERNUM=$(echo "${REL_SOVER}" | grep " VERSION " | sed -e's/  *VERSION  *//' )
	REL_SOVERNUM=$(echo "${REL_SOVER}" | grep " SOVERSION " | sed -e's/  *SOVERSION  *//' -e's/)//' )
	echo "  VERSION=${REL_VERNUM}"
	echo "  SOVERSION=${REL_SOVERNUM}"
	if ! grep -q " VERSION ${REL_VERNUM}" CMakeLists.txt ; then
		echo "Updating VERSION=${REL_VERNUM}, SOVERSION=${REL_VERNUM} in CMakeLists.txt to match release branch"
		# Paste in the whole block from the releaes copy of CMakeLists.txt:
		awk -v REL_SOVER="${REL_SOVER}"  \
			'/set_target_prop.*PROJECT_NAME/,/)/ {
				if (REL_SOVER) print REL_SOVER;
				REL_SOVER=null;
				next;
			}
			{ print; }' \
			CMakeLists.txt > CMakeLists.txt.new
		mv -f CMakeLists.txt.new CMakeLists.txt 
	else
		echo "Warning: CMakeLists.txt already contains VERSION ${REL_VERNUM}" 1>&2
	fi

	if ! grep -q "${RELVER}.99" meson.build ; then
		echo "Updating project version in meson.build to ${RELVER}.99"
		sed -i -e"s/project(\(.*\)version:.*/project(\1version: '${RELVER}.99',/" meson.build
	else
		echo "Warning: meson.build already contains ${RELVER}.99" 1>&2
	fi

	if ! grep -q "version: *'${REL_SOVERNUM}'" meson.build ; then
		echo "Updating soversion in meson.build to ${REL_VERNUM}, ${REL_SOVERNUM}"
		awk -v version="'${REL_VERNUM}'" -v soversion="'${REL_SOVERNUM}'" \
			'/library\(.json-c/,/\)/ {
				if ($1 == "version:") { gsub("'\''.*", version ","); }
				if ($1 == "soversion:") { gsub("'\''.*", soversion ","); }
			} 
			{ print; }' < meson.build > meson.build.new
		mv -f meson.build.new meson.build
	else
		echo "Warning: meson.build already contains soversion ${REL_VERNUM}, ${REL_SOVERNUM}" 1>&2
	fi

	if ! grep -q "JSONC_${NEXTVER}" json-c.sym; then
		echo "Add a new empty section to the json-c.sym file, for ${NEXTVER}"
		cat <<EOF >> json-c.sym

JSONC_${NEXTVER} {
#  global:
} JSONC_${RELVER};
EOF
	else
		echo "Warning: json-c-sym already has JSONC_${NEXTVER}" 1>&2
	fi

	git commit -a -m "Update the master branch to version ${release}.99"
}

while [ $# -gt 0 ] ; do
	case "$1" in
	--dev)
		bump_dev_ver
		trap - EXIT
		exit 0
	;;
	--release)
		bump_release_ver
		trap - EXIT
		exit 0
		;;
	-h|--help)
		usage 0
		;;
	*)
		usage 1 "Unknown args: $*"
		exit 1
	;;
	esac
done

usage 1 "Need at least one argument"

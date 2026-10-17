#!/bin/bash

set -e -o pipefail

RUNDIR=$(dirname "$0")
RUNDIR=$(realpath "${RUNDIR}")

. "${RUNDIR}/reltools/reltools.subr"
# The 0.17 release is broken
#prev=0.18
#release=0.19

prev=$(get_most_recent_relver)
prevtag=$(get_most_recent_reltag "${prev}")
release=$(get_ver_descr)

ACCDIR=${RUNDIR}/../abi-compliance-checker
ACC=${ACCDIR}/abi-compliance-checker.pl
REPORTDIR=${ACCDIR}
INSTBASE=${HOME}/json-c-installs

usage()
{
	local exitval="$1" ; shift
	local errmsg="$1"
	if [ $exitval -ne 0 ] ; then
		exec 1>&2
	fi
	if [ -n "$errmsg" ] ; then
		echo "ERROR: $errmsg" 1>&2
	fi
	cat <<EOF
Usage: $0 [--help] [--rebuild-old] [--rebuild-new] [--regen]
          [--buildold] [--prev <tag>]
          [-R <reportdir>] [-I <instdir>]
  Build two versions of json-c and compare them with abi-compliance-checker

  --rebuild-old - Re-build the previous version
  --rebuild-new - Re-build the current version
  --regen       - Always re-generate the ABI dumps
  --buildold - Build the previous version if not yet done
               By default, an old build must already exist.
  --prev     - Specify an alternate old tag to compare against
  --clean    - Rebuild and regen everything
  -R         - Where to place output reports (${REPORTDIR})
  -I         - Where to install json-c builds to (${INSTBASE})

  Previous release: ${prev} (${prevtag})
  Current version:  ${release}
EOF
	exit "$exitval"
}

REBUILD_NEW=0
REBUILD_OLD=0
BUILD_OLD=0
REGEN_ABI_DUMPS=0

while [ $# -gt 0 ] ; do
	case "$1" in
	--clean)
		BUILD_OLD=1
		REGEN_ABI_DUMPS=1
		REBUILD_NEW=1
		REBUILD_OLD=1
		;;
	--rebuild-new)
		REBUILD_NEW=1
		;;
	--rebuild-old)
		REBUILD_OLD=1
		;;
	--regen)
		REGEN_ABI_DUMPS=1
		;;
	--buildold)
		BUILD_OLD=1
		;;
	--prev)
		prev="$2"
		prevtag=$(get_most_recent_reltag "${prev}")
		shift
		;;
	-R)
		REPORTDIR="$2"
		shift
		;;
	-I)
		INSTBASE="$2"
		shift
		;;
	-h|--help)
		usage 0
		;;
	*)
		usage 1 "Unknown args: $*"
		;;
	esac
	shift
done

if [ ! -d "${ACCDIR}" ]; then
	echo "Cloning abi-compliance-checker into ${ACCDIR}"
	git clone https://github.com/lvc/abi-compliance-checker "${ACCDIR}"
fi

if ! type -P perl > /dev/null ; then
	echo "perl install needed: apt-get install perl, yum install perl, etc..." 1>&2
	exit 1
fi

# ===============================

build_new()
{
	local BUILD_DEST=${INSTBASE}/json-c-${release}
	if [ ! -d "${BUILD_DEST}" ] || [ "${REBUILD_NEW}" -eq 1 ] ; then
		echo "Building $(pwd) (${release}) into ${BUILD_DEST}"
		rm -rf build.abi-check
		mkdir -p build.abi-check/${release}
		(
			cd build.abi-check/${release}
			CFLAGS=-Og cmake -DCMAKE_INSTALL_PREFIX="${BUILD_DEST}" ../..
			make && make test
			rm -rf "${BUILD_DEST}"
			make install
		)
	fi
}

build_old()
{
	local BUILD_DEST=${INSTBASE}/json-c-${prev}
	if [ ! -d "${BUILD_DEST}" ] && [ "${BUILD_OLD}" -eq 0 ] ; then
		echo "ERROR: ${INSTBASE}/json-c-${prev} does not exist, pass --buildold to build it." 1>&2
		exit 1
	fi
	if [ ! -d "${BUILD_DEST}" ] || [ "${REBUILD_OLD}" -eq 1 ] ; then
		rm -rf "build.abi-check/${prev}"
		git clone "${RUNDIR}" "build.abi-check/${prev}"
		(
			cd "build.abi-check/${prev}"
			git -c advice.detachedHead=false checkout "${prevtag}"
			echo "Building $(pwd) (${prevtag}) into ${BUILD_DEST}"
			mkdir build.abi-check
			cd build.abi-check
			CFLAGS=-Og cmake -DCMAKE_INSTALL_PREFIX="${BUILD_DEST}" ..
			make && make test
			rm -rf "${BUILD_DEST}"
			make install
		)
	fi
}

mkxml()
{
	local INSTREL
	INSTREL=$(realpath --relative-to="${REPORTDIR}" "${INSTBASE}")
	ver="$1"
	if [ ! -e "${INSTBASE}/json-c-${ver}/lib64" ] ; then
		ln -s lib "${INSTBASE}/json-c-${ver}/lib64"
	fi
cat <<EOF > json-c-${ver}.xml
<foo>
<version>
   ${ver}
</version>

<headers>
${INSTREL}/json-c-${ver}/include/json-c
</headers>

<libs>
${INSTREL}/json-c-${ver}/lib64/libjson-c.so
</libs>
</foo>
EOF
}

abidump()
{
	local libver="$1" ; shift

	local dumppath="./ABI-${libver}.dump"
	if [ "${REGEN_ABI_DUMPS}" -eq 1 ] ; then
		rm -f "${dumppath}"
	fi
	if [ ! -e "${dumppath}" ] ; then
		perl "${ACC}" -lib json-c -dump json-c-${libver}.xml -dump-path "${dumppath}"
	else
		echo "ABI dump already exists at ${dumppath}"
	fi
}

# ===============================

mkdir -p "${REPORTDIR}"

build_old
build_new

cd "${REPORTDIR}"
mkxml ${release}
mkxml ${prev}

echo "=== Generating ABI dumps ==="
abidump ${prev}
abidump ${release}

echo "=== Comparing ABIs ==="
perl "${ACC}" -l json-c -old ABI-${prev}.dump -new ABI-${release}.dump

echo
echo "Look in $(pwd)/compat_reports/json-c/${prev}_to_${release}"

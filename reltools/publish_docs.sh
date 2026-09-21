#!/bin/bash
#
# Publish release docs on the gh-pages branch in ~/json-c-pages
# Does NOT push changes
#

set -e -o pipefail
trap 'echo "ERROR: Unexpected exit in $0"' EXIT

RUNDIR=$(dirname "$0")
. "${RUNDIR}/reltools.subr"

reltag="$1"
if [ -z "${reltag}" ] ; then
	echo "Usage: $0 <relver>" 1>&2
	trap - EXIT
	exit 1
fi
release=${reltag%-*}  # Strip off the date suffix
release=${release#json-c}

cd "${HOME}"

if [ ! -d "json-c-${release}" ] ; then
	git clone https://github.com/json-c/json-c "json-c-${release}"
fi

cd json-c-${release}
git checkout ${reltag}
if [ ! -e doc/html/index.html ] ; then
	echo "ERROR: unable to find doc/html/index.html on at version ${reltag} in $(pwd)" 1>&2
	exit 1
fi
cd ..


if [ ! -d json-c-pages ] ; then
	git clone -b gh-pages https://github.com/json-c/json-c json-c-pages
fi
cd json-c-pages
mkdir -p json-c-${release}
cp -R ../json-c-${release}/doc json-c-${release}/.
git add json-c-${release}
rm json-c-current-release
ln -s json-c-${release} json-c-current-release
git commit -a -m "Add the ${release} docs."

# TODO: automate changes to index.html
cat <<EOF
Steps to do:
 * Add section with links to new json-c-${release} release.
 * Update versions in the "Current release" section
EOF
read -p "Press <enter> to run vi index.html: "
vi index.html

git commit -a -m "Update the doc links to point at ${release}"

cat <<EOF
cd $(pwd)
git log -n1 --name-status
git log -n1 -p


# If all looks ok:
git push
EOF

trap - EXIT
exit 0

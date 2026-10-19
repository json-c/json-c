#!/bin/bash
#
# Analyze the ABI compliance report and pick out things that 
# may need attention when creating a new release.
#

RUNDIR=$(dirname "$0")

exitval=0

XMLREPORT="$1"
if [[ "${XMLREPORT}" = -* ]] || [ ! -r "${XMLREPORT}" ] ; then
	echo "Usage: $0 <abireport.xml>" 1>&2
	exit 1
fi

XSLTPROC=${XSLTPROC:-xsltproc}
if ! type -P "${XSLTPROC}" > /dev/null ; then
	# Fallback to xsltproc.exe from Strawberry perl if on WSL
	XSLTPROC=xsltproc.exe
fi
if ! type -P "${XSLTPROC}" > /dev/null ; then
	echo "ERROR: can't find xsltproc, check ABI report for added/removed symbols" 1>&2
	exit 1
fi

XMLREPORT=$HOME/abi-compliance-checker/compat_reports/json-c/0.19_to_0.19.99-50e3389/compat_report.xml
BIN_SYMBOLS="$("$XSLTPROC" --stringparam kind binary --stringparam elemname added_symbols "${RUNDIR}/acc_added_symbols.xsl" "${XMLREPORT}" | sort)"
SRC_SYMBOLS="$("$XSLTPROC" --stringparam kind source --stringparam elemname added_symbols "${RUNDIR}/acc_added_symbols.xsl" "${XMLREPORT}" | sort)"
echo "INFO: Added symbols, check if these are meant to be visible:"
echo "${SRC_SYMBOLS}" | sed -e's/^/  /'
echo

if ! cmp -s <(echo "${BIN_SYMBOLS}") <(echo "${SRC_SYMBOLS}") ; then
	echo "WARNING: mismatch between src vs. bin added symbols, check json-c.sym" 1>&2
	diff -u0 <(echo "${BIN_SYMBOLS}") <(echo "${SRC_SYMBOLS}") | \
		awk 'NR>3 && /^(-|\+)/ { printf("  %s\n", $0); }'
	echo
	exitval=1
fi

DEL_SYMBOLS="$("$XSLTPROC" --stringparam elemname removed_symbols "${RUNDIR}/acc_added_symbols.xsl" "${XMLREPORT}" | sort)"
if [ -z "${DEL_SYMBOLS}" ] ; then
	echo "OK: No removed symbols detected"
else
	echo "=========================" 1>&2
	echo "WARNING: major release may be needed!  Removed symbols:" 1>&2
	echo "${DEL_SYMBOLS}" 1>&2
	echo 1>&2
	exitval=1
fi

exit $exitval

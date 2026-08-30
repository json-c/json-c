#!/usr/bin/env bash
# Build every json-c variant for the requested Amiga systems and package them
# into dist/json-c.lha.
#
#   ./amiga/package-amiga.sh [OS ...]     # default: all three
#
# Archive layout (single directory at the root):
#     json-c/json-c.readme
#     json-c/AmigaOS3/...    per-OS libraries and headers
#     json-c/AmigaOS4/...
#     json-c/MorphOS/...
set -uo pipefail

AMIGA_DIR=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$AMIGA_DIR/.." && pwd)
cd "$ROOT"
DIST=$ROOT/dist
STAGE=$DIST/stage
OSES=("$@"); [[ ${#OSES[@]} -eq 0 ]] && OSES=(AmigaOS3 AmigaOS4 MorphOS)

command -v lha >/dev/null || { echo "error: lha not found on PATH" >&2; exit 1; }
# Ubuntu's `lha` package is lhasa, which can only extract.
_lha_probe=$(mktemp -d)
echo x > "$_lha_probe/f"
if ! ( cd "$_lha_probe" && lha -aq2 t.lha f >/dev/null 2>&1 && [[ -s t.lha ]] ); then
  echo "error: lha cannot create archives (is it lhasa?). Install LHa for UNIX: https://github.com/jca02266/lha" >&2
  rm -rf "$_lha_probe"
  exit 1
fi
rm -rf "$_lha_probe"

# Build straight into the staging tree by pointing AMIGA_SDK at it: the build
# script's SDK layout ($SDK/<target>) is the same shape we want per OS, so each
# OS gets staged under its own directory.
rm -rf "$DIST"; mkdir -p "$STAGE/json-c"
cp "$AMIGA_DIR/json-c.readme" "$STAGE/json-c/json-c.readme"

built=()
for os in "${OSES[@]}"; do
  case "$os" in
    AmigaOS3) sub=m68k-amigaos ;;
    AmigaOS4) sub=ppc-amigaos ;;
    MorphOS)  sub=ppc-morphos ;;
    *) echo "skip: unknown OS $os"; continue ;;
  esac
  tmp=$DIST/sdk-$os
  mkdir -p "$tmp/$sub"
  echo "=== $os ==="
  # Stage whatever landed even if some variants failed -- a single unsupported
  # multilib should not drop the whole OS from the archive.
  AMIGA_SDK="$tmp" "$AMIGA_DIR/build-amiga-sdk.sh" "$os" || echo "  ($os: some variants failed, see above)"
  if [[ -n $(find "$tmp/$sub" -name 'libjson-c.a' -print -quit 2>/dev/null) ]]; then
    mkdir -p "$STAGE/json-c/$os"
    cp -a "$tmp/$sub/." "$STAGE/json-c/$os/"
    built+=("$os")
  else
    echo "  $os: nothing built, omitted from the archive"
  fi
  rm -rf "$tmp"
done

[[ ${#built[@]} -eq 0 ]] && { echo "nothing built"; exit 1; }

# NOTE: build the archive from a clean staging tree with `lha -aq2`. Do NOT try
# to refresh a single entry (e.g. the readme) in an existing archive with
# `lha u` -- it APPENDS a second entry with the same name rather than replacing
# it, and extraction then yields the stale copy.
( cd "$STAGE" && lha -aq2 "$DIST/json-c.lha" * >/dev/null )
rm -rf "$STAGE"
echo
echo "packaged: ${built[*]}"
ls -la "$DIST"

#!/usr/bin/env bash
# Cross-build json-c for Amiga systems across every multilib slot and install
# the archives plus headers into an SDK tree.
#
#   ./build-amiga-sdk.sh [OS ...]        # default: AmigaOS3 AmigaOS4 MorphOS
#
# SDK location comes from $AMIGA_SDK (default /opt/amiga).
set -uo pipefail

SDK=${AMIGA_SDK:-/opt/amiga}
cd "$(dirname "$0")"
ROOT=$PWD
OSES=("$@"); [[ ${#OSES[@]} -eq 0 ]] && OSES=(AmigaOS3 AmigaOS4 MorphOS)

# os | image tag | sdk subdir
os_image() { case "$1" in
  AmigaOS3) echo "sacredbanana/amiga-compiler:m68k-amigaos m68k-amigaos" ;;
  AmigaOS4) echo "sacredbanana/amiga-compiler:ppc-amigaos ppc-amigaos" ;;
  MorphOS)  echo "sacredbanana/amiga-compiler:ppc-morphos ppc-morphos" ;;
  *) return 1 ;;
esac; }

# Where an AmigaSDK-gcc-style checkout keeps each system's target tree.
os_sdkdir() { case "$1" in
  AmigaOS3) echo "amigaos3/sdk" ;;
  AmigaOS4) echo "amigaos4/sdk" ;;
  MorphOS)  echo "morphos/sdk" ;;
  *) return 1 ;;
esac; }

# Per-OS variant table: "tree|slot|cmake args"
variants_AmigaOS3=(
  "libnix|.|-DM68K_CRT=nix20 -DM68K_CPU=68000 -DM68K_BASEREL=off"
  "libnix|libm020|-DM68K_CRT=nix20 -DM68K_CPU=68020 -DM68K_BASEREL=off"
  "libnix|libb|-DM68K_CRT=nix20 -DM68K_CPU=68000 -DM68K_BASEREL=baserel"
  "libnix|libb/libm020|-DM68K_CRT=nix20 -DM68K_CPU=68020 -DM68K_BASEREL=baserel"
  "libnix|libb32/libm020|-DM68K_CRT=nix20 -DM68K_CPU=68020 -DM68K_BASEREL=baserel32"
  ".|.|-DM68K_CRT=newlib -DM68K_CPU=68000 -DM68K_BASEREL=off"
  ".|libm020|-DM68K_CRT=newlib -DM68K_CPU=68020 -DM68K_BASEREL=off"
  ".|libb|-DM68K_CRT=newlib -DM68K_CPU=68000 -DM68K_BASEREL=baserel"
  ".|libb/libm020|-DM68K_CRT=newlib -DM68K_CPU=68020 -DM68K_BASEREL=baserel"
  ".|libb32/libm020|-DM68K_CRT=newlib -DM68K_CPU=68020 -DM68K_BASEREL=baserel32"
  "clib2|.|-DM68K_CRT=clib2 -DM68K_CPU=68000 -DM68K_BASEREL=off"
  "clib2|libm020|-DM68K_CRT=clib2 -DM68K_CPU=68020 -DM68K_BASEREL=off"
  "clib2|libb|-DM68K_CRT=clib2 -DM68K_CPU=68000 -DM68K_BASEREL=baserel"
  "clib2|libb/libm020|-DM68K_CRT=clib2 -DM68K_CPU=68020 -DM68K_BASEREL=baserel"
  "clib2|libb32/libm020|-DM68K_CRT=clib2 -DM68K_CPU=68020 -DM68K_BASEREL=baserel32"
)
# Unlike m68k, where each C runtime is its own tree (libnix/, clib2/), the PPC
# toolchains make the runtime a multilib SLOT under lib/, with headers coming
# from the shared <target>/include/. Verified with -print-search-dirs:
#   ppc-amigaos  -mcrt=clib2  -> <target>/lib/clib2/
#   ppc-amigaos  -mcrt=clib4  -> <target>/lib/clib4/
#   ppc-morphos  -mclib=libnix-> <target>/lib/libnix/
#   ppc-morphos  -mbaserel32  -> <target>/lib/libb32/
variants_AmigaOS4=(
  ".|.|-DPPC_CRT=newlib"
  ".|clib2|-DPPC_CRT=clib2"
  ".|clib4|-DPPC_CRT=clib4"
)
# MORPHOS_CLIB=libnix (-mclib=libnix) and NOIXEMUL (-noixemul) select the same
# multilib; it is the self-contained ABI, needing no ixemul.library on target.
variants_MorphOS=(
  ".|.|"
  ".|libb32|-DMORPHOS_BASEREL32=ON"
  ".|libnix|-DMORPHOS_CLIB=libnix"
  ".|libb32/libnix|-DMORPHOS_CLIB=libnix -DMORPHOS_BASEREL32=ON"
)

# $1 out-dir-for-libs  $2 out-dir-for-headers  $3.. OS list
ok=0; fail=0; failed=()
for os in "${OSES[@]}"; do
  read -r image sdksub <<<"$(os_image "$os")" || { echo "skip: unknown OS $os"; continue; }
  # /opt/amiga keeps the target tree directly under <target>/, while an
  # AmigaSDK-gcc checkout nests it under <os>/sdk/<target>. Accept either.
  osdir=$(os_sdkdir "$os")
  if   [[ -d $SDK/$sdksub ]];        then SDKROOT=$SDK/$sdksub
  elif [[ -d $SDK/$osdir/$sdksub ]]; then SDKROOT=$SDK/$osdir/$sdksub
  else echo "skip $os: neither $SDK/$sdksub nor $SDK/$osdir/$sdksub"; continue; fi

  eval "vars=(\"\${variants_$os[@]}\")"
  for v in "${vars[@]}"; do
    IFS='|' read -r tree slot cargs <<<"$v"
    bdir="bld-$os-$(echo "$tree$slot$cargs" | tr -c 'A-Za-z0-9' '-' | cut -c1-40)"
    printf '  %-9s %-7s %-16s ' "$os" "$tree" "$slot"
    if ! docker run --rm -v "$ROOT":/work -e USER="$(id -u)" -e GROUP="$(id -g)" "$image" sh -c "
        set -e
        rm -rf /work/$bdir && mkdir -p /work/$bdir && cd /work/$bdir
        cmake $cargs -DCMAKE_BUILD_TYPE=Release -DBUILD_APPS=NO \
              -DCMAKE_INSTALL_PREFIX=/work/$bdir/inst .. >cmake.log 2>&1
        make -j\$(nproc) json-c >make.log 2>&1
        make install >install.log 2>&1
      " 2>/dev/null; then
      echo "BUILD FAILED (logs in $bdir)"; fail=$((fail+1)); failed+=("$os/$tree/$slot"); continue
    fi
    lib=$bdir/libjson-c.a
    [[ -f $lib ]] || lib=$bdir/inst/lib/libjson-c.a
    [[ -f $lib ]] || { echo "no libjson-c.a"; fail=$((fail+1)); failed+=("$os/$tree/$slot"); continue; }

    base=$SDKROOT; [[ $tree != "." ]] && base=$SDKROOT/$tree
    dest=$base/lib; [[ $slot != "." ]] && dest=$base/lib/$slot
    mkdir -p "$dest" "$base/include"
    cp "$lib" "$dest/libjson-c.a"
    cp -a "$bdir/inst/include/json-c" "$base/include/"
    echo "ok"; ok=$((ok+1)); rm -rf "$bdir"
  done
done
echo
echo "built+installed: $ok   failed: $fail"
((fail)) && { printf 'failures: %s\n' "${failed[*]}"; exit 1; }
exit 0

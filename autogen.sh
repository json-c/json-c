#!/bin/sh
autoreconf -v --install || exit 1
if test -z "$NOCONFIGURE"; then
    exec ./configure  "$@"
fi

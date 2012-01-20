autoreconf -v --install || exit 1
exec ./configure  "$@"

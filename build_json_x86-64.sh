git clean -xfd
# autogen.sh
# ./configure --build x86_64-pc-linux-gnu --host arm-linux-gnueabi LDFLAGS="-static -pthread" --enable-mpers=check
cmake -DBUILD_SHARED_LIBS=OFF ../json-c
make


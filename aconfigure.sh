ANDROID_NDK_DIR=/opt/elear-solutions
LIBICONV_INSTALL_DIR=/opt/elear-solutions/android

declare -a COMPILE_ARCHITECTURES=("armv7" "arm64" "x86" "x86_64")
SAVED_PATH="${PATH}"
for ARCH in "${COMPILE_ARCHITECTURES[@]}"
do
    make distclean
    autoreconf -i
    COMPILER_DIR=""
    COMPILER_PREFIX=""
    case ${ARCH} in
        "armv7" )
            COMPILER_DIR=toolchain_armeabi_v7a_19
            ;;
        "arm64" )
            COMPILER_DIR=toolchain_aarch64_v8a_21
            ;;
        "x86" )
            COMPILER_DIR=toolchain_x86_19
            ;;
	"x86_64" )
            COMPILER_DIR=toolchain_x86_64_21
            ;;
    esac

    export ANDROID_NDK_ROOT="${ANDROID_NDK_DIR}/${COMPILER_DIR}"

    ANDROID_NDK_BIN="${ANDROID_NDK_ROOT}/bin"
    ANDROID_SYSROOT_DIR="${ANDROID_NDK_ROOT}/sysroot"

    export PATH="${ANDROID_NDK_BIN}:${SAVED_PATH}"

    export CFLAGS="--sysroot=${ANDROID_SYSROOT_DIR}"
    export CXXFLAGS="--sysroot=${ANDROID_SYSROOT_DIR}"
    case ${ARCH} in
	"armv7" )
            ABI_NAME=armv7
            COMPILER_PREFIX=arm-linux-androideabi
            ;;
        "arm64" )
	    ABI_NAME=arm64
            COMPILER_PREFIX=aarch64-linux-android
            ;;
        "x86" )
	    ABI_NAME=x86
            COMPILER_PREFIX=i686-linux-android
            ;;
	"x86_64" )
	    ABI_NAME=x86_64
            COMPILER_PREFIX=x86_64-linux-android
            ;;


    esac


    export CC=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-clang
    export CPP=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-cpp
    export CXX=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-clang++
    export LD=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-ld
    export AR=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-ar
    export RANLIB=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-ranlib
    export STRIP=${ANDROID_NDK_BIN}/${COMPILER_PREFIX}-strip

   echo "---- Compiling for ${ARCH}"
   ./configure --host="${COMPILER_PREFIX}" --prefix="${LIBICONV_INSTALL_DIR}/${ABI_NAME}"
   make clean
   make install
done

export PATH="${SAVED_PATH}"

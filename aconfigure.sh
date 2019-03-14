#! /usr/bin/env bash

## *****************************************************************************
##  
## Description : aconfigure is a script used for compiling autotools projects 
##               for android.
##
##               Prerequisites:
##               1. Setup android toolchains using README.md present in 
##                  cocosdk-java/jni 
##
## *****************************************************************************
##/*===============================================================================*/
##/* Copyright (c) 2018 Elear Solutions Tech Private Limited. All rights reserved. */
##/* To any person (the "Recipient") obtaining a copy of this software and         */
##/* associated documentation files (the "Software"):                              */
##/*                                                                               */
##/* All information contained in or disclosed by this software is confidential    */
##/* and proprietary information of Elear Solutions Tech Private Limited and all   */
##/* rights therein are expressly reserved. By accepting this material the         */
##/* recipient agrees that this material and the information contained therein is  */
##/* held in confidence and in trust and will NOT be used, copied, modified,       */
##/* merged, published, distributed, sublicensed, reproduced in whole or in part,  */
##/* nor its contents revealed in any manner to others without the express         */
##/* written permission of Elear Solutions Tech Private Limited.                   */
##/*===============================================================================*/

ANDROID_TOOLCHAINS_DIR="/opt/elear-solutions"

declare -a COMPILE_ARCHITECTURES=("armv7" "arm64" "x86" "x86_64")
for ARCH in "${COMPILE_ARCHITECTURES[@]}"
do
    make distclean
    autoreconf -fsi
    ANDROID_TOOLCHAIN_DIR=""
    COMPILER_PREFIX=""
    case ${ARCH} in
        "armv7" )
            ANDROID_TOOLCHAIN_DIR="toolchain_armeabi_v7a_19"
            COMPILER_PREFIX="arm-linux-androideabi"
            ;;
        "arm64" )
            ANDROID_TOOLCHAIN_DIR="toolchain_aarch64_v8a_21"
            COMPILER_PREFIX="aarch64-linux-android"
            ;;
        "x86" )
            ANDROID_TOOLCHAIN_DIR="toolchain_x86_19"
            COMPILER_PREFIX="i686-linux-android"
            ;;
        "x86_64" )
            ANDROID_TOOLCHAIN_DIR="toolchain_x86_64_21"
            COMPILER_PREFIX="x86_64-linux-android"
            ;;
    esac

    export ANDROID_TOOLCHAIN_ROOT="${ANDROID_TOOLCHAINS_DIR}/${ANDROID_TOOLCHAIN_DIR}"

    ANDROID_TOOLCHAIN_BIN="${ANDROID_TOOLCHAIN_ROOT}/bin"
    ANDROID_TOOLCHAIN_SYSROOT_DIR="${ANDROID_TOOLCHAIN_ROOT}/sysroot"
    ANDROID_TOOLCHAIN_LD_LIB="${ANDROID_TOOLCHAIN_SYSROOT_DIR}/usr/lib"


    export CFLAGS="--sysroot=${ANDROID_TOOLCHAIN_SYSROOT_DIR}"
    export CXXFLAGS="--sysroot=${ANDROID_TOOLCHAIN_SYSROOT_DIR}"
    export LDFLAGS="-L${ANDROID_TOOLCHAIN_ROOT}/${COMPILER_PREFIX}/lib"

    export CC=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-clang
    # export CPP=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-cpp
    export CXX=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-clang++
    export LD=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-ld
    export AR=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-ar
    export RANLIB=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-ranlib
    export STRIP=${ANDROID_TOOLCHAIN_BIN}/${COMPILER_PREFIX}-strip

    echo "---- Compiling for ${ARCH}"
    ./configure --host="${COMPILER_PREFIX}" --prefix="${ANDROID_TOOLCHAIN_SYSROOT_DIR}/usr" CFLAGS="-fPIC"
    make
    make install
done

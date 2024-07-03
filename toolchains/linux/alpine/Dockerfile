FROM alpine:3.9

RUN apk update && \
    apk add --no-cache \
    binutils \
    make \
    cmake \
    libgcc \
    musl-dev \
    gcc \
    g++ \
    autoconf \
    automake \
    libtool \
    linux-headers \
    lua \
    lua-dev \
    openssl \
    curl \
    python3 \
    pkgconfig \
    ncurses-dev \
    texinfo \
    postgresql-dev \
    git \
    curl-dev \
    openssl-dev \
    zlib-dev

RUN curl https://bootstrap.pypa.io/pip/3.6/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py && \
    pip3 install conan==1.60.1

RUN conan remote add conan_server http://conan.elear.solutions && \
    conan profile new --detect default

COPY ./profiles/ /root/.conan/profiles/

ENV CONAN_DEFAULT_PROFILE_PATH /root/.conan/profiles/alpine_x86_64

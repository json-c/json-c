FROM ubuntu:22.04

RUN apt update && \
    apt install -y \
    build-essential \
    make \
    cmake \
    autoconf \
    automake \
    libtool \
    curl \
    python3 \
    pkg-config \
    ncurses-dev \
    texinfo \
    ca-certificates-java \
    openjdk-8-jdk \
    libpq-dev \
    git \
    libcurl4-openssl-dev \
    libssl-dev \
    zlib1g-dev

RUN curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py && \
    pip3 install conan==1.60.1

RUN conan remote add conan_server http://conan.elear.solutions && \
    conan profile new --detect default

COPY ./profiles/ /root/.conan/profiles/

ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/
ENV CONAN_DEFAULT_PROFILE_PATH /root/.conan/profiles/ubuntu_x86_64

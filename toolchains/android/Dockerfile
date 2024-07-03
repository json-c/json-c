FROM ubuntu:22.04

ADD android-ndk-r25c-linux.tar.gz /opt/

RUN apt update && \
    apt install -y \
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
    git

RUN curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py && \
    pip3 install conan==1.60.1

RUN conan remote add conan_server http://conan.elear.solutions

COPY ./profiles/ /root/.conan/profiles/

ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/
ENV ANDROID_NDK /opt/android-ndk-r25c/
ENV CONAN_DEFAULT_PROFILE_PATH /root/.conan/profiles/android_armv8

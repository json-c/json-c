## Toolchains and use cases

- Apple toolchain is for iOS (arm, arm64, x86, x86_64) & macOS (x86_64)
- Android toolchain is for Android (arm, arm64, x86, x86_64)
- Linux toolchain is for general purpose linux machines (alpine & ubuntu)
- Goke toolchain is for building cameras
- OpenWrt toolchain is for gateways

## Apple toolchain info

- Apple toolchains are not actually toolchains
- The profiles provided are meant to be copied to ```$HOME/.conan/profiles``` folder
- Depending on the necessity, appropriate profile must be used

## Building
- Some toolchains need additional files to be added which are available to download [here](https://drive.google.com/drive/folders/1V0Ol2-PnoEkpx0Z7W4DyQtb5CKidXbL3)
- For building android toolchain, download the ```android-ndk-r25c-linux.tar.gz``` and place it in the ```android``` folder
- For building goke toolchain, download the ```toolchain_goke_armv6.tar.gz``` and place it in the ```linux/goke``` folder
- For building openwrt toolchain, download the ```toolchain_openwrt_mips.tar.gz``` and place it in the ```linux/openwrt``` folder
- Docker command for building the toolchains: ```docker build . -t docker-reg.elear.solutions/dev/tools/<toolchain_name>:<version>```
- Docker command for pushing to docker registry: ```docker push docker-reg.elear.solutions/dev/tools/<toolchain_name>:<version>```

toolchain | toolchain name
----------|--------------
android   | toolchain_android
ubuntu    | toolchain_ubuntu
alpine    | toolchain_alpine
goke      | toolchain_goke_armv6
openwrt   | toolchain_openwrt_mips

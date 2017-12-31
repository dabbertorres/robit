from debian:stretch-slim as builder

arg CT_NG_VERSION=1.23.0

run apt update && apt install -y \
    # deps f or downloading
    wget \
    xz-utils \
    # build deps
    g++ \
    gperf \
    bison \
    flex \
    texinfo \
    help2man \
    make \
    libncurses5-dev \
    python-dev \
    patch \
    gawk

# build crosstools-ng
run wget http://crosstool-ng.org/download/crosstool-ng/crosstool-ng-${CT_NG_VERSION}.tar.xz && \
    tar -xf crosstool-ng-${CT_NG_VERSION}.tar.xz && \
    cd crosstool-ng-${CT_NG_VERSION} && \
    ./configure --prefix=/usr && \
    make && \
    make install

# build our toolchain! (installs to /opt/gcc-rpi)
workdir /build
copy defconfig .
run ct-ng defconfig && \
    ct-ng build

# final image
from debian:stretch-slim
copy --from=builder /opt/gcc-rpi /opt/gcc-rpi
env PATH=$PATH:/opt/gcc-rpi/bin


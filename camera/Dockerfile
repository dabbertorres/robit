from dabbertorres/rpi3-gcc as builder

arg LIBJPEG_TURBO_VERSION=1.5.3

run apt update && apt install -y \
    git \
    m4

run git clone https://github.com/libjpeg-turbo/libjpeg-turbo && \
    cd libjpeg-turbo && git checkout 1.5.3 && \
    autoreconf -fiv && \
    mkdir build && cd build && \
    CC=`which rpi3-cc` sh ../configure --host arm-unknown-linux-musleabi CFLAGS='-O3' && \
    make && make install

from dabbertorres/rpi3-gcc
copy --from=builder /opt/libjpeg-turbo /opt/libjpeg-turbo

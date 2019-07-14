mkdir build-bin
cd build-bin
../binutils-*/configure \
--prefix=/usr/local/aarch64-elf-gcc \
--target=aarch64-elf \
--enable-shared \
--enable-threads=posix \
--enable-libmpx \
--with-system-zlib \
--with-isl \
--enable-__cxa_atexit \
--disable-libunwind-exceptions \
--enable-clocale=gnu \
--disable-libstdcxx-pch \
--disable-libssp \
--enable-plugin \
--disable-linker-build-id \
--enable-lto \
--enable-install-libiberty \
--with-linker-hash-style=gnu \
--with-gnu-ld\
--enable-gnu-indirect-function \
--disable-multilib \
--disable-werror \
--enable-checking=release \
--enable-default-pie \
--enable-default-ssp \
--disable-nls \
--enable-gnu-unique-object

make -j8
sudo make install
cd ..

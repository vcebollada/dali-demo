
if [ -d ./libpng/cmake/${TARGET} ]; then
cd ./libpng/cmake/${TARGET}
else
mkdir -p ./libpng/cmake/${TARGET}
cd ./libpng/cmake/${TARGET}
if [ -z "$ENABLE_TRACE" ]; then
export ENABLE_TRACE=OFF
fi
export PREFIX=${DALI_DIR}/dali-env/android
export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
export PKG_CONFIG_PATH=${PREFIX}/lib/${TARGET}/pkgconfig
${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/android-dependencies/libpng -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=24 -DANDROID_ABI=${TARGET} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=gnu++11' -DCMAKE_INSTALL_INCLUDEDIR=${PREFIX}/include -DCMAKE_INSTALL_LIBDIR=${PREFIX}/lib/${TARGET} -DCMAKE_BUILD_TYPE=$@ -DPNG_SHARED=ON -DPNG_LIB_NAME=png
fi
make
make install
cd -

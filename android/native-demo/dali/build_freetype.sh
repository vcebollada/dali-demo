
if [ -d ./libfreetype/cmake/${TARGET} ]; then
cd ./libfreetype/cmake/${TARGET}
else
mkdir -p ./libfreetype/cmake/${TARGET}
cd ./libfreetype/cmake/${TARGET}
export PREFIX=${DALI_DIR}/dali-env/android
export LIB_DIR=${PREFIX}/lib/${TARGET}
export INCLUDE_DIR=${PREFIX}/include
export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
export PKG_CONFIG_PATH=${LIB_DIR}/pkgconfig
${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/android-dependencies/libfreetype -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=24 -DANDROID_ABI=${TARGET} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=gnu++11' -DCMAKE_INSTALL_INCLUDEDIR=include -DCMAKE_INSTALL_LIBDIR=lib/${TARGET} -DCMAKE_BUILD_TYPE=$@ -DBUILD_SHARED_LIBS=ON -DFT_WITH_ZLIB=ON -DFT_WITH_PNG=OFF -DFT_WITH_HARFBUZZ=OFF -DCMAKE_C_FLAGS='-I./include -I./include/freetype -I./include/freetype/config'
# -DPNG_LIBRARY=${LIB_DIR} -DPNG_PNG_INCLUDE_DIR=${INCLUDE_DIR}
# -DHARFBUZZ_LIBRARIES=${LIB_DIR} -DHARFBUZZ_INCLUDE_DIRS=${INCLUDE_DIR}/harfbuzz
fi
make
make install
cd -

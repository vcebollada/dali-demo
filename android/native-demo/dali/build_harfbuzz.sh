
if [ -d ./libharfbuzz/cmake/${TARGET} ]; then
cd ./libharfbuzz/cmake/${TARGET}
else
mkdir -p ./libharfbuzz/cmake/${TARGET}
cd ./libharfbuzz/cmake/${TARGET}
if [ -z "$ENABLE_TRACE" ]; then
export ENABLE_TRACE=OFF
fi
export PREFIX=${DALI_DIR}/dali-env/android
export LIB_DIR=${PREFIX}/lib/${TARGET}
export INCLUDE_DIR=${PREFIX}/include
export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
export PKG_CONFIG_PATH=${LIB_DIR}/pkgconfig
${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/android-dependencies/libharfbuzz -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=24 -DANDROID_ABI=${TARGET} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=gnu++11 -I${INCLUDE_DIR}/freetype2/freetype -I${INCLUDE_DIR}/freetype2/freetype/config' -DCMAKE_INSTALL_INCLUDEDIR=${INCLUDE_DIR} -DCMAKE_INSTALL_LIBDIR=${LIB_DIR} -DCMAKE_PREFIX_PATH=${LIB_DIR}/pkgconfig -DCMAKE_BUILD_TYPE=$@ -DHB_BUILTIN_UCDN=ON -DHB_HAVE_FREETYPE=ON -DBUILD_SHARED_LIBS=ON -DCMAKE_C_FLAGS='-I${INCLUDE_DIR}/freetype2/freetype -I${INCLUDE_DIR}/freetype2/freetype/config'
fi
make
make install
cd -

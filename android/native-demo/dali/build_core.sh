
if [ -d ./dali-core/cmake/${TARGET} ]; then
cd ./dali-core/cmake/${TARGET}
else
mkdir -p ./dali-core/cmake/${TARGET}
cd ./dali-core/cmake/${TARGET}
if [ -z "$ENABLE_TRACE" ]; then
export ENABLE_TRACE=OFF
fi
export PREFIX=${DALI_DIR}/dali-env/android
export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
export PKG_CONFIG_PATH=${PREFIX}/lib/${TARGET}/pkgconfig
${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake ${DALI_DIR}/dali-core/build/tizen -DCMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang -DCMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=24 -DANDROID_ABI=${TARGET} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=gnu++11' -DINCLUDE_DIR=${PREFIX}/include -DLIB_DIR=${PREFIX}/lib/${TARGET} -DCMAKE_BUILD_TYPE=$@ -DENABLE_TRACE=${ENABLE_TRACE}
fi
make
make install
cd -

if [ -d ./libgif/cmake/${ANDROID_ABI} ]; then

cd ./libgif/cmake/${ANDROID_ABI}

else

mkdir -p ./libgif/cmake/${ANDROID_ABI}
cd ./libgif/cmake/${ANDROID_ABI}

export CMAKE_INSTALL_PREFIX=${DALI_DIR}/dali-env/android
export CMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_PREFIX}/lib/${ANDROID_ABI}
export CMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_PREFIX}/include
export PKG_CONFIG_LIBDIR=${ANDROID_NDK}/prebuilt/linux-x86_64/lib/pkgconfig
export PKG_CONFIG_PATH=${CMAKE_INSTALL_LIBDIR}/pkgconfig
export CMAKE_C_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang
export CMAKE_CXX_COMPILER=${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++
export CMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake
export CMAKE=${ANDROID_SDK}/cmake/3.10.2.4988404/bin/cmake

${CMAKE} ${DALI_DIR}/android-dependencies/libgif -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DANDROID_PLATFORM=${ANDROID_PLATFORM} -DANDROID_ABI=${DANDROID_ABI} -DANDROID_NDK=${ANDROID_NDK} -DANDROID_STL=c++_shared -DCMAKE_CXX_FLAGS='-fexceptions -frtti -w -Wall -std=gnu++11' -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR} -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR} -DCMAKE_BUILD_TYPE=$@ -DGIF_SHARED=ON

fi

make
make install


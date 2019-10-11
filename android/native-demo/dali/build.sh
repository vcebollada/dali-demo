#!/bin/bash
if [ -z "$NDK" ]; then
  . ./env.sh
fi

if [ ! -d "$ANDROID_SDK" ]; then
  echo "Please install Android SDK into "$ANDROID_SDK
  exit 1
fi

if [ ! -d "$ANDROID_NDK" ]; then
  echo "Please install Android NDK into "$ANDROID_NDK
  exit 1
fi

if [ "$1" = "clean" ]; then
echo "Cleaning dali..."
rm -rf ../../../../dali-env/android
rm -rf ./dali-core
exit 0
fi

if [ ! -d ../../../../dali-env/android ]; then
mkdir -p ../../../../dali-env/android
if [ -z "$DEBUG" ]; then
TARGET=x86_64 ./build_core.sh Release
TARGET=armeabi-v7a ./build_core.sh Release
else
export ENABLE_TRACE=ON
TARGET=x86_64 ./build_core.sh Debug
ENABLE_TRACE=ON TARGET=armeabi-v7a ./build_core.sh Debug
fi

echo "Copying dali headers..."
TARGET=x86_64 API=25 make headers
TARGET=armeabi-v7a API=25 make headers
cp ./system-cache-path.cpp ../../../../dali-adaptor/dali/internal/adaptor/common/system-cache-path.cpp

fi

if [ -z "$DEBUG" ]; then
TARGET=x86_64 ./build_core.sh Release
TARGET=armeabi-v7a ./build_core.sh Release
else
export ENABLE_TRACE=ON
TARGET=x86_64 ./build_core.sh Debug
TARGET=armeabi-v7a ./build_core.sh Debug
fi

TARGET=x86_64 API=25 make $@
TARGET=armeabi-v7a API=25 make $@

#!/bin/sh

ANDROID_NDK_TOOLCHAIN_ROOT=~/android-toolchain/ cmake \
-DBOOST_ROOT=~/boost-1.55.0-armeabi-v7a \
-DCMAKE_TOOLCHAIN_FILE=../src/zmAndroidClient/android.toolchain.cmake \
-DSWIG_EXECUTABLE=/usr/bin/swig \
-DSWIG_DIR=/usr/share/swig/2.0.11 \
-DLIBRARY_OUTPUT_PATH_ROOT=../src/zmAndroidClient/ \
-DEXECUTABLE_OUTPUT_PATH=../src/zmAndroidClient/bin \
../src


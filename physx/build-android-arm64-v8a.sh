#!/bin/bash

#export ANDROID_NDK_HOME=/Applications/AndroidNDK13750724.app/Contents/NDK
export ANDROID_NDK_HOME=/data/android-ndk-r27d
./generate_projects.sh android-arm64-v8a
cd compiler/android-arm64-v8a-release
cmake --build .

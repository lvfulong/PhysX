#!/bin/bash

#export ANDROID_NDK_HOME=/Applications/AndroidNDK13750724.app/Contents/NDK
export ANDROID_NDK_HOME=/data/android-ndk-r27d

./generate_projects.sh android-armeabi-v7a
cd compiler/android-armeabi-v7a-release
cmake --build .

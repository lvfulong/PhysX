#!/bin/bash

export ANDROID_NDK_HOME=/Applications/AndroidNDK13750724.app/Contents/NDK
./generate_projects.sh android-armeabi-v7a
cd compiler/android-armeabi-v7a-release
cmake --build .

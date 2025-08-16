#!/bin/bash

export ANDROID_NDK_HOME=/Applications/AndroidNDK13750724.app/Contents/NDK
./generate_projects.sh android-arm64-v8a
cd compiler/android-arm64-v8a-release
cmake --build .

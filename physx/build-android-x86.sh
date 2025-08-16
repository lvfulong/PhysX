#!/bin/bash

export ANDROID_NDK_HOME=/Applications/AndroidNDK13750724.app/Contents/NDK
./generate_projects.sh android-x86
cd compiler/android-x86-release
cmake --build .

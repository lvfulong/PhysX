#!/bin/sh

rm -rf iosFat
mkdir iosFat
lipo -create ./release-ios-arm64/libPhysX_static_64.a ./release-ios-x86_64/libPhysX_static_64.a -output ./iosFat/libPhysX_static_64.a
lipo -create ./release-ios-arm64/libPhysXCharacterKinematic_static_64.a ./release-ios-x86_64/libPhysXCharacterKinematic_static_64.a -output ./iosFat/libPhysXCharacterKinematic_static_64.a
lipo -create ./release-ios-arm64/libPhysXCommon_static_64.a ./release-ios-x86_64/libPhysXCommon_static_64.a -output ./iosFat/libPhysXCommon_static_64.a
lipo -create ./release-ios-arm64/libPhysXCooking_static_64.a ./release-ios-x86_64/libPhysXCooking_static_64.a -output ./iosFat/libPhysXCooking_static_64.a
lipo -create ./release-ios-arm64/libPhysXExtensions_static_64.a ./release-ios-x86_64/libPhysXExtensions_static_64.a -output ./iosFat/libPhysXExtensions_static_64.a
lipo -create ./release-ios-arm64/libPhysXFoundation_static_64.a ./release-ios-x86_64/libPhysXFoundation_static_64.a -output ./iosFat/libPhysXFoundation_static_64.a
lipo -create ./release-ios-arm64/libPhysXVehicle_static_64.a ./release-ios-x86_64/libPhysXVehicle_static_64.a -output ./iosFat/libPhysXVehicle_static_64.a
lipo -create ./release-ios-arm64/libPhysXVehicle2_static_64.a ./release-ios-x86_64/libPhysXVehicle2_static_64.a -output ./iosFat/libPhysXVehicle2_static_64.a
#lipo -create ./release-ios-arm64/libPhysXPvdSDK_static_64.a ./release-ios-x86_64/libPhysXPvdSDK_static_64.a -output ./iosFat/libPhysXPvdSDK_static_64.a


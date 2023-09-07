//
// Created by Oasis on 2023/3/21.
//

#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>

#include "../BindingHelper.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace emscripten;

struct LayaQuaryResult{
    public:
    bool Quary;
    PxU32 ActorUUID;
    PxU32 ShapeUUID;
    PxVec3 position;
    PxVec3 normal;
};

struct LayaTriggerInfo{
     public:
        PxU32 triggerShape;
	PxU32 triggerActor;
        PxU32 otherShape;
	PxU32 otherActor;
};

struct LayaContactPairInfo
{
     public:
	PxU32 pxShape0;
	PxU32 pxShape1;
	PxU8 contactCount;
	PxContactPairPoint contactPoint0;
	PxContactPairPoint contactPoint1;
	PxContactPairPoint contactPoint2;
	PxContactPairPoint contactPoint3;
};

EMSCRIPTEN_BINDINGS(physx_math) {
    // Define PxsetCMassLocalPoseec3, PxQuat and PxTransform as value objects to allow sumerian Vector3
    // and Quaternion to be used directly without the need to free the memory
    value_object<PxVec3>("PxVec3").field("x", &PxVec3::x).field("y", &PxVec3::y).field("z", &PxVec3::z);
    register_vector<PxVec3>("PxVec3Vector");
    value_object<PxQuat>("PxQuat")
            .field("x", &PxQuat::x)
            .field("y", &PxQuat::y)
            .field("z", &PxQuat::z)
            .field("w", &PxQuat::w);
    value_object<PxTransform>("PxTransform").field("translation", &PxTransform::p).field("rotation", &PxTransform::q);
    value_object<PxExtendedVec3>("PxExtendedVec3")
            .field("x", &PxExtendedVec3::x)
            .field("y", &PxExtendedVec3::y)
            .field("z", &PxExtendedVec3::z);
    enum_<PxIDENTITY>("PxIDENTITY").value("PxIdentity", PxIDENTITY::PxIdentity);

    value_object<LayaTriggerInfo>("LayaTriggerInfo")
            .field("triggerShape",&LayaTriggerInfo::triggerShape)
            .field("triggerActor",&LayaTriggerInfo::triggerActor)
            .field("otherShape",&LayaTriggerInfo::otherShape)
            .field("otherActor",&LayaTriggerInfo::otherActor);

    value_object<PxContactPairPoint>("PxContactPairPoint")
            .field("position",&PxContactPairPoint::position)
            .field("normal",&PxContactPairPoint::normal)
            .field("impulse",&PxContactPairPoint::impulse);

    value_object<LayaContactPairInfo>("LayaContactPairInfo")
            .field("pxShape0",&LayaContactPairInfo::pxShape0)
            .field("pxShape1",&LayaContactPairInfo::pxShape1)
            .field("contactCount",&LayaContactPairInfo::contactCount)
            .field("contactPoint0",&LayaContactPairInfo::contactPoint0)
            .field("contactPoint1",&LayaContactPairInfo::contactPoint1)
            .field("contactPoint2",&LayaContactPairInfo::contactPoint2)
            .field("contactPoint3",&LayaContactPairInfo::contactPoint3);
    
    value_object<LayaQuaryResult>("LayaQuaryResult")
        .field("Quary",&LayaQuaryResult::Quary)
        .field("ActorUUID",&LayaQuaryResult::ActorUUID)
        .field("ShapeUUID",&LayaQuaryResult::ShapeUUID)
        .field("position",&LayaQuaryResult::position)
        .field("normal",&LayaQuaryResult::normal);

    value_object<PxFilterData>("PxFilterData")
        .field("word0",&PxFilterData::word0)
        .field("word1",&PxFilterData::word1)
        .field("word2",&PxFilterData::word2)
        .field("word3",&PxFilterData::word3);
    register_vector<int>("Vector_Int");
    register_vector<LayaTriggerInfo>("Vector_PxTriggerPair");
    //register_vector<PxActor>("Vector_PxActor");
    register_vector<LayaContactPairInfo*>("Vector_PxContactPairInfo");
}
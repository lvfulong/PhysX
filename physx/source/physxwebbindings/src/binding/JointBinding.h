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

EMSCRIPTEN_BINDINGS(physx_joint) {
    /* PhysXJoint ✅ */
    class_<PxJoint>("PxJoint")
            .function("setActors", &PxJoint::setActors, allow_raw_pointers())  // ✅
            .function("setLocalPose",
                      optional_override([](PxJoint &joint, int actor, const PxVec3 &position, const PxQuat &rotation) {
                          joint.setLocalPose(PxJointActorIndex::Enum(actor), PxTransform(position, rotation));
                      }))                                        // ✅
            .function("setBreakForce", &PxJoint::setBreakForce)  // ✅
            .function("setConstraintFlag", optional_override([](PxJoint &joint, int flag, bool value) {
                          joint.setConstraintFlag(PxConstraintFlag::Enum(flag), value);
                      }))                                                    // ✅
            .function("setInvMassScale0", &PxJoint::setInvMassScale0)        // ✅
            .function("setInvInertiaScale0", &PxJoint::setInvInertiaScale0)  // ✅
            .function("setInvMassScale1", &PxJoint::setInvMassScale1)        // ✅
            .function("setInvInertiaScale1", &PxJoint::setInvInertiaScale1)  // ✅
            .function("release", &PxJoint::release)
            .function("setUUID", optional_override([](PxJoint &joint, uint32_t uuid) {
                          auto ptr = malloc(sizeof(uint32_t));
                          memcpy(ptr, &uuid, sizeof(uint32_t));
                          joint.userData = ptr;
            }))
            .function("getUUID", optional_override([](PxJoint &joint) {
                    return getJointUUID(&joint);
            }));
    /* PhysXFixedJoint ✅ */
    class_<PxFixedJoint, base<PxJoint>>("PxFixedJoint")
            .function("setProjectionLinearTolerance", &PxFixedJoint::setProjectionLinearTolerance)     // ✅
            .function("setProjectionAngularTolerance", &PxFixedJoint::setProjectionAngularTolerance);  // ✅
    /* PhysXSphericalJoint ✅ */
    class_<PxSphericalJoint, base<PxJoint>>("PxSphericalJoint")
            .function("setHardLimitCone", optional_override([](PxSphericalJoint &joint, PxReal yLimitAngle,
                                                               PxReal zLimitAngle, PxReal contactDist) {
                          joint.setLimitCone(PxJointLimitCone(yLimitAngle, zLimitAngle, contactDist));
                      }))  // ✅
            .function("setSoftLimitCone", optional_override([](PxSphericalJoint &joint, PxReal yLimitAngle,
                                                               PxReal zLimitAngle, PxReal stiffness, PxReal damping) {
                          joint.setLimitCone(PxJointLimitCone(yLimitAngle, zLimitAngle, PxSpring(stiffness, damping)));
                      }))  // ✅
            .function("setSphericalJointFlag", optional_override([](PxSphericalJoint &joint, PxReal flag, bool value) {
                          joint.setSphericalJointFlag(PxSphericalJointFlag::Enum(flag), value);
                      }))                                                                                // ✅
            .function("setProjectionLinearTolerance", &PxSphericalJoint::setProjectionLinearTolerance);  // ✅
    /* PhysXHingeJoint ✅ */
    class_<PxRevoluteJoint, base<PxJoint>>("PxRevoluteJoint")
            .function("setHardLimit", optional_override([](PxRevoluteJoint &joint, PxReal lowerLimit, PxReal upperLimit,
                                                           PxReal contactDist) {
                          joint.setLimit(PxJointAngularLimitPair(lowerLimit, upperLimit, contactDist));
                      }))  // ✅
            .function("setSoftLimit", optional_override([](PxRevoluteJoint &joint, PxReal lowerLimit, PxReal upperLimit,
                                                           PxReal stiffness, PxReal damping) {
                          joint.setLimit(PxJointAngularLimitPair(lowerLimit, upperLimit, PxSpring(stiffness, damping)));
                      }))                                                          // ✅
            .function("setDriveVelocity", &PxRevoluteJoint::setDriveVelocity)      // ✅
            .function("setDriveForceLimit", &PxRevoluteJoint::setDriveForceLimit)  // ✅
            .function("setDriveGearRatio", &PxRevoluteJoint::setDriveGearRatio)    // ✅
            .function("setRevoluteJointFlag", optional_override([](PxRevoluteJoint &joint, PxReal flag, bool value) {
                          joint.setRevoluteJointFlag(PxRevoluteJointFlag::Enum(flag), value);
                      }))                                                                                 // ✅
            .function("setProjectionLinearTolerance", &PxRevoluteJoint::setProjectionLinearTolerance)     // ✅
            .function("setProjectionAngularTolerance", &PxRevoluteJoint::setProjectionAngularTolerance);  // ✅
    /* PhysXSpringJoint ✅ */
    class_<PxDistanceJoint, base<PxJoint>>("PxDistanceJoint")
            .function("setMinDistance", &PxDistanceJoint::setMinDistance)  // ✅
            .function("setMaxDistance", &PxDistanceJoint::setMaxDistance)  // ✅
            .function("setTolerance", &PxDistanceJoint::setTolerance)      // ✅
            .function("setStiffness", &PxDistanceJoint::setStiffness)      // ✅
            .function("setDamping", &PxDistanceJoint::setDamping)          // ✅
            .function("setDistanceJointFlag", optional_override([](PxDistanceJoint &joint, PxReal flag, bool value) {
                          joint.setDistanceJointFlag(PxDistanceJointFlag::Enum(flag), value);
                      }));  // ✅
    /* PhysXTranslationalJoint ✅ */
    class_<PxPrismaticJoint, base<PxJoint>>("PxPrismaticJoint")
            .function("setHardLimit", optional_override([](PxPrismaticJoint &joint, PxTolerancesScale &scale,
                                                           PxReal lowerLimit, PxReal upperLimit, PxReal contactDist) {
                          joint.setLimit(PxJointLinearLimitPair(scale, lowerLimit, upperLimit, contactDist));
                      }))  // ✅
            .function("setSoftLimit", optional_override([](PxPrismaticJoint &joint, PxReal lowerLimit,
                                                           PxReal upperLimit, PxReal stiffness, PxReal damping) {
                          joint.setLimit(PxJointLinearLimitPair(lowerLimit, upperLimit, PxSpring(stiffness, damping)));
                      }))  // ✅
            .function("setPrismaticJointFlag", optional_override([](PxPrismaticJoint &joint, PxReal flag, bool value) {
                          joint.setPrismaticJointFlag(PxPrismaticJointFlag::Enum(flag), value);
                      }))                                                                                  // ✅
            .function("setProjectionLinearTolerance", &PxPrismaticJoint::setProjectionLinearTolerance)     // ✅
            .function("setProjectionAngularTolerance", &PxPrismaticJoint::setProjectionAngularTolerance);  // ✅
    /* PhysXConfigurableJoint ✅ */
    class_<PxD6Joint, base<PxJoint>>("PxD6Joint")
            .function("setMotion", optional_override([](PxD6Joint &joint, int axis, int type) {
                          joint.setMotion(PxD6Axis::Enum(axis), PxD6Motion::Enum(type));
                      }))  // ✅
            .function("setDistanceLimit",
                      optional_override([](PxD6Joint &joint, PxReal extent,PxReal bounceness,PxReal bounceThreshold, PxReal stiffness, PxReal damping) {
                        PxJointLinearLimit limit(extent,PxSpring(stiffness,damping));
                        limit.restitution = bounceness;
                        limit.bounceThreshold = bounceThreshold;
                        joint.setDistanceLimit(limit);
                      }))  // ✅
            .function("setLinearLimit", optional_override([](PxD6Joint &joint, int axis, PxReal lowerLimit,
                                                                 PxReal upperLimit,PxReal bounceness,PxReal bounceThreshold, PxReal stiffness ,PxReal damping) {
                          PxJointLinearLimitPair limit = PxJointLinearLimitPair(lowerLimit, upperLimit, PxSpring(stiffness, damping));
                          limit.restitution = bounceness;
                          limit.bounceThreshold = bounceThreshold;
                          joint.setLinearLimit(PxD6Axis::Enum(axis),limit);
                      }))  // ✅
            .function("setTwistLimit",
                      optional_override([](PxD6Joint &joint, PxReal lowerLimit, PxReal upperLimit,PxReal bounceness,PxReal bounceThreshold, PxReal stiffness ,PxReal damping) {
                          
                          PxJointAngularLimitPair limit = PxJointAngularLimitPair(lowerLimit, upperLimit, -1.0f);
                          limit.restitution = bounceness;
                          limit.bounceThreshold = bounceThreshold;
                          limit.stiffness = stiffness;
                          limit.damping = damping;
                          joint.setTwistLimit(limit);
                      }))  // ✅
            .function("setSwingLimit",
                      optional_override([](PxD6Joint &joint,PxReal yAngle,PxReal zAngle,PxReal bounceness,PxReal bounceThreshold,PxReal spring,PxReal damping) {
                          PxJointLimitCone limit = PxJointLimitCone(yAngle,zAngle,-1.0f);
                          limit.restitution = bounceness;
                          limit.bounceThreshold = bounceThreshold;
                          limit.stiffness = spring;
                          limit.damping = damping;
                          joint.setSwingLimit(limit);
                      }))  // ✅
            // .function("setHardPyramidSwingLimit",
            //           optional_override([](PxD6Joint &joint, PxReal yLimitAngleMin, PxReal yLimitAngleMax,
            //                                PxReal zLimitAngleMin, PxReal zLimitAngleMax, PxReal contactDist) {
            //               joint.setPyramidSwingLimit(PxJointLimitPyramid(yLimitAngleMin, yLimitAngleMax, zLimitAngleMin,
            //                                                              zLimitAngleMax, contactDist));
            //           }))  // ✅
            // .function("setSoftPyramidSwingLimit",
            //           optional_override([](PxD6Joint &joint, PxReal yLimitAngleMin, PxReal yLimitAngleMax,
            //                                PxReal zLimitAngleMin, PxReal zLimitAngleMax, PxReal stiffness,
            //                                PxReal damping) {
            //               joint.setPyramidSwingLimit(PxJointLimitPyramid(yLimitAngleMin, yLimitAngleMax, zLimitAngleMin,
            //                                                              zLimitAngleMax, PxSpring(stiffness, damping)));
            //           }))  // ✅
            .function("setDrive", optional_override([](PxD6Joint &joint,int index,PxReal stiffness,PxReal damping,PxReal forceLimit,PxReal flag) {
                        PxD6JointDrive limit = PxD6JointDrive();
                        limit.forceLimit = forceLimit;
                        limit.flags = PxD6JointDriveFlag::Enum(flag);
                        limit.stiffness = stiffness;
                        limit.damping = damping;
                        joint.setDrive(PxD6Drive::Enum(index),limit);
                      }))  // ✅
            .function("setDrivePosition", optional_override([](PxD6Joint &joint, const PxVec3 &pos, const PxQuat &rot) {
                          joint.setDrivePosition(PxTransform(pos, rot));
                      }))                                                                           // ✅
            .function("setDriveVelocity", &PxD6Joint::setDriveVelocity)                             // ✅
            .function("setProjectionLinearTolerance", &PxD6Joint::setProjectionLinearTolerance)     // ✅
            .function("setProjectionAngularTolerance", &PxD6Joint::setProjectionAngularTolerance);  // ✅
}

namespace emscripten {
namespace internal {
template <>
void raw_destructor<PxJoint>(PxJoint *) { /* do nothing */
}
}  // namespace internal
}  // namespace emscripten
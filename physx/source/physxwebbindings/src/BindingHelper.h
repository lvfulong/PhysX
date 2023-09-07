//
// Created by Oasis on 2023/3/21.
//

#pragma once

#include "PxPhysicsAPI.h"

inline uint32_t getUUID(physx::PxShape* shape) {
    return *static_cast<uint32_t*>(shape->userData);
}

inline uint32_t getUUID(const physx::PxShape* shape) {
    return *static_cast<uint32_t*>(shape->userData);
}

inline uint32_t getActorUUID(physx::PxActor* actor) {
    return *static_cast<uint32_t*>(actor->userData);
}

inline uint32_t getActorUUID(const physx::PxActor* actor) {
    return *static_cast<uint32_t*>(actor->userData);
}
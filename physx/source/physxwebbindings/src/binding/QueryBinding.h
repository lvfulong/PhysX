#pragma once

#include "../SimulationFilterShader.h"
#include "../BindingHelper.h"
#include "MathBinding.h"
#include "PxPhysicsAPI.h"
#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>

using namespace physx;
using namespace emscripten;




class PxCustomQueryFilterCallBack :public PxQueryFilterCallback {
public:
	virtual PxQueryHitType::Enum preFilter(
		const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* /*actor*/, PxHitFlags& /*queryFlags*/) {
		if (filterData.word0 & shape->getSimulationFilterData().word0) {
			return PxQueryHitType::eTOUCH;
		}
		else
			return PxQueryHitType::eNONE;
	}
}queryFilterCallBack;

bool raycastClosest(const PxScene& Scene, const PxVec3& ori, const PxVec3& dir, PxU32 filterGroup,LayaQuaryResult &quaryResult) {
	PxRaycastBuffer hitBuffer;                 // [out] Raycast results
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT|PxHitFlag::eANY_HIT;
	//PxQueryFilterData
	PxQueryFlags flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;
	PxQueryFilterData FilterDataSt(PxFilterData(filterGroup,0,0,0), flags);

	bool hitResult = Scene.raycast(ori, dir, 10000000, hitBuffer, hitFlag,FilterDataSt,&queryFilterCallBack);
	quaryResult.Quary = hitResult;
    if (hitResult) {
		//printf("rayCast Cloest\n");
		PxRaycastHit hit = hitBuffer.block;
        quaryResult.position = hit.position;
        quaryResult.normal = hit.normal;
        quaryResult.ShapeUUID = getUUID(hit.shape);
        quaryResult.ActorUUID = getActorUUID(hit.actor);
	}
	return hitResult;
};

bool raycastAnyHit(const PxScene& Scene, const PxVec3& t, const PxVec3& velocity, const int filterGroup) {
	/*UserCallback cb;
	cb.data*/

	//const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	//PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	//RayCastHitCallback hitCallBack(hitBuffer, bufferSize); // [out] Blocking and touching hits stored here
	PxRaycastBuffer hitBuffer;                 // [out] Raycast results
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT | PxHitFlag::eANY_HIT;
	//PxQueryFilterData
	PxQueryFilterData FilterDataSt;
	FilterDataSt.data = PxFilterData(filterGroup, 0, 0, 0);
	FilterDataSt.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::eANY_HIT | PxQueryFlag::ePREFILTER;

	//PxQueryFilterCallback
	//PxQueryCache
	//PxGeometryQueryFlags
	bool hitResult = Scene.raycast(t, velocity, 1000000, hitBuffer, hitFlag, FilterDataSt, &queryFilterCallBack);
	if (hitResult) {
		printf("rayCast anyHit");
		PxRaycastHit hit = hitBuffer.getAnyHit(0);
		printf("rayCast anyHit Position %f,%f,%f\n", hit.position.x, hit.position.y, hit.position.z);
	}

	return hitResult;
}


bool raycastAnyHits(const PxScene& Scene, const PxVec3& t, const PxVec3& velocity, const int filterGroup,const int hitNumber) {
	PxRaycastHit* hitArray = new PxRaycastHit[hitNumber];  // [out] User provided buffer for results
	PxRaycastBuffer hitBuffer(hitArray, hitNumber); // [out] Blocking and touching hits stored here
	
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT;
	//PxQueryFilterData
	PxQueryFilterData FilterDataSt;
	FilterDataSt.data = PxFilterData(filterGroup, 0, 0, 0);
	FilterDataSt.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eANY_HIT;

	//PxQueryFilterCallback
	//PxQueryCache
	//PxGeometryQueryFlags
	bool hitResult = Scene.raycast(t, velocity, 1000, hitBuffer, hitFlag, FilterDataSt, &queryFilterCallBack);
	
	if (hitResult) {
		printf("rayCast Mul");
		for (PxU32 i = 0; i < hitBuffer.nbTouches; i++) {
			PxRaycastHit hit = hitBuffer.touches[i];
			printf("rayCast anyHit Position %f,%f,%f\n", hit.position.x, hit.position.y, hit.position.z);
		}
	}
	delete[] hitArray;
	return hitResult;
}


// struct PxRaycastCallbackWrapper : public wrapper<PxRaycastCallback> {
//     EMSCRIPTEN_WRAPPER(explicit PxRaycastCallbackWrapper)

//     PxAgain processTouches(const PxRaycastHit *buffer, PxU32 nbHits) override {
//         for (PxU32 i = 0; i < nbHits; i++) {
//             bool again = call<PxAgain>("processTouches", buffer[i]);
//             if (!again) {
//                 return false;
//             }
//         }
//         return true;
//     }
// };

// PxRaycastHit *allocateRaycastHitBuffers(PxU32 nb) {
//     auto *myArray = new PxRaycastHit[nb];
//     return myArray;
// }

//----------------------------------------------------------------------------------------------------------------------
// struct PxSweepCallbackWrapper : public wrapper<PxSweepCallback> {
//     EMSCRIPTEN_WRAPPER(explicit PxSweepCallbackWrapper)

//     PxAgain processTouches(const PxSweepHit *buffer, PxU32 nbHits) override {
//         for (PxU32 i = 0; i < nbHits; i++) {
//             bool again = call<PxAgain>("processTouches", buffer[i]);
//             if (!again) {
//                 return false;
//             }
//         }
//         return true;
//     }
// };

// PxSweepHit *allocateSweepHitBuffers(PxU32 nb) {
//     auto *myArray = new PxSweepHit[nb];
//     return myArray;
// }

//----------------------------------------------------------------------------------------------------------------------
// struct PxQueryFilterCallbackWrapper : public wrapper<PxQueryFilterCallback> {
//     EMSCRIPTEN_WRAPPER(explicit PxQueryFilterCallbackWrapper)

//     PxQueryHitType::Enum postFilter(const PxFilterData &filterData, const PxQueryHit &hit) override {
//         return (PxQueryHitType::Enum)call<int>("postFilter", filterData, hit);
//     }

//     PxQueryHitType::Enum preFilter(const PxFilterData &filterData,
//                                    const PxShape *shape,
//                                    const PxRigidActor *actor,
//                                    PxHitFlags &) override {
//         return (PxQueryHitType::Enum)call<int>("preFilter", filterData, getUUID(shape), actor);
//     }
// };
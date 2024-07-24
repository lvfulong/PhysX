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

bool raycastClosest(const PxScene& Scene, const PxVec3& ori, const PxVec3& dir, PxU32 distance,PxU32 filterGroup, PxU32 filterMask,LayaQuaryResult &quaryResult) {
	PxRaycastBuffer hitBuffer;                 // [out] Raycast results
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT|PxHitFlag::eANY_HIT;
	//PxQueryFilterData
	PxQueryFlags flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;
	PxQueryFilterData FilterDataSt(PxFilterData(filterGroup,filterMask,0,0), flags);

	bool hitResult = Scene.raycast(ori, dir, distance, hitBuffer, hitFlag,FilterDataSt,&queryFilterCallBack);
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


bool raycastAllHits(const PxScene& Scene, const PxVec3& ori, const PxVec3& dir, PxU32 distance, PxU32 filterGroup, PxU32 filterMask,std::vector<LayaQuaryResult> &quaryResults) {
	PxRaycastHit* hitArray = new PxRaycastHit[1024];
	PxRaycastBuffer hitBuffer(hitArray, 1024);                 // [out] Raycast results
	
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT;
	//PxQueryFilterData
	PxQueryFilterData FilterDataSt;
	FilterDataSt.data = PxFilterData(filterGroup, filterMask, 0, 0);
	FilterDataSt.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eNO_BLOCK;
	bool hitResult = Scene.raycast(ori, dir, distance, hitBuffer, hitFlag,FilterDataSt,&queryFilterCallBack);
    if (hitResult) {
		PxU32 nHits = hitBuffer.getNbAnyHits();
		// printf("get nNBAnyhits %d\n", nHits);
		// PxU32 nbTouches = hitBuffer.nbTouches;
		// printf("get nbTouches %d\n", nbTouches);
		for(PxU32 i = 0;i < nHits; i++){
			PxRaycastHit hit = hitBuffer.getAnyHit(i);
			// printf("rayCast anyHit Position %f,%f,%f\n", hit.position.x, hit.position.y, hit.position.z);
			LayaQuaryResult quaryRes;
	        quaryRes.position = hit.position;
    	    quaryRes.normal = hit.normal;
        	quaryRes.ShapeUUID = getUUID(hit.shape);
        	quaryRes.ActorUUID = getActorUUID(hit.actor);			
			quaryResults.push_back(quaryRes);
		}
	}
	delete[] hitArray;
	return hitResult;
};

bool shapeCastClosest(const PxScene &scene, const PxGeometry &geometry, const PxTransform &pose, const PxVec3 &unitDir, const PxReal distance, PxU32 filterGroup, PxU32 filterMask, LayaQuaryResult &queryResult, PxReal inflation) {
    PxHitFlags outputFlags = PxHitFlag::eANY_HIT | PxHitFlag::eDEFAULT; // 返回第一个hit目标
    PxSceneQueryFilterData filterData(PxFilterData(filterGroup, filterMask, 0, 0), PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER);
    PxSceneQueryFilterCallback* filterCall = nullptr; // 初始化指针
    const PxSceneQueryCache* cache = nullptr; // 初始化指针
    PxSweepBuffer buf;

    bool hasHit = scene.sweep(geometry, pose, unitDir, distance, buf, outputFlags, filterData, filterCall, cache, inflation);

    if (hasHit && buf.hasBlock) {
        // printf("    ====  hit shape ====    ");
        queryResult.Quary = buf.hasBlock;
        PxSweepHit hit = buf.block;
        queryResult.position = hit.position;
        queryResult.normal = hit.normal;
        queryResult.ShapeUUID = getUUID(hit.shape);
        queryResult.ActorUUID = getActorUUID(hit.actor);
    }

    return buf.hasBlock;
}

bool shapeCastAll(const PxScene &scene, const PxGeometry &geometry, const PxTransform &pose, const PxVec3 &unitDir, const PxReal distance, PxU32 filterGroup, PxU32 filterMask, std::vector<LayaQuaryResult> &quaryResults, PxReal inflation){
	PxSweepHit hits[1024];
    PxSweepBuffer buf(hits, 1024);
	PxHitFlags outputFlags = PxHitFlag::eMESH_ANY | PxHitFlag::ePRECISE_SWEEP; // 任何mesh与所有sweep
    PxSceneQueryFilterData filterData(PxFilterData(filterGroup, filterMask, 0, 0), PxQueryFlag::eNO_BLOCK | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER);
    PxSceneQueryFilterCallback* filterCall = nullptr; // 初始化指针
    const PxSceneQueryCache* cache = nullptr; // 初始化指针

    bool hasHit = scene.sweep(geometry, pose, unitDir, distance, buf, outputFlags, filterData, filterCall, cache, inflation);

    if (hasHit) {
		PxU32 nHits = buf.getNbAnyHits();
		for(PxU32 i = 0;i < nHits; i++){
			PxSweepHit hit = buf.getAnyHit(i);
			// printf("rayCast anyHit Position %f,%f,%f\n", hit.position.x, hit.position.y, hit.position.z);
			LayaQuaryResult quaryRes;
	        quaryRes.position = hit.position;
    	    quaryRes.normal = hit.normal;
        	quaryRes.ShapeUUID = getUUID(hit.shape);
        	quaryRes.ActorUUID = getActorUUID(hit.actor);			
			quaryResults.push_back(quaryRes);
		}
    }
    return buf.hasBlock;
}


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
//
// Created by Oasis on 2023/3/21.
//

#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>

#include "../SimulationFilterShader.h"
#include "../BindingHelper.h"
#include "PxPhysicsAPI.h"
#include "QueryBinding.h"
#include <vector>

using namespace physx;
using namespace emscripten;


//custom FilterShader
static	PxFilterFlags triggersUsingFilterCallback(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/)
{
	//PX_ASSERT(getImpl() == FILTER_CALLBACK);

	//Trigger & Trigger
	if (PxFilterObjectIsTrigger(attributes0) && PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eSUPPRESS;
	}

	//group mask
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {//can fillter
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {
            PxPairFlags myPairFlag0 = PxPairFlags(filterData0.word2&filterData1.word2);
			pairFlags = (myPairFlag0&PxPairFlag::eTRIGGER_DEFAULT);
			return PxFilterFlag::eDEFAULT;
		}
		else {
            PxPairFlags myPairFlag1 = PxPairFlags((filterData0.word2|filterData1.word2));
			pairFlags = (PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_PERSISTS|PxPairFlag::eNOTIFY_CONTACT_POINTS)&myPairFlag1;
			return PxFilterFlag::eDEFAULT;
		}
	}
	else {//cant filter
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eSUPPRESS;
	}
}


//contact Event
struct ContactReportCallback : public wrapper<PxSimulationEventCallback>
{
    EMSCRIPTEN_WRAPPER(explicit ContactReportCallback)

	void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/) override
	{
		//TODO
	}

	void onWake(PxActor** actors, PxU32 count) override
	{
        if(count==0)
            return;
        std::vector<PxU32> wakeActor;
        wakeActor.resize(count);
        int index= 0;
        while (count--)
        {
            PxActor* actor = *actors++;
            wakeActor[index++] = getActorUUID(actor);
        }
        call<void>("onWake",wakeActor);
        
	}

	void onSleep(PxActor** actors, PxU32 count) override
	{
         if(count==0)
            return;
        std::vector<PxU32> sleepActor;
        sleepActor.resize(count);
        int index= 0;
        while (count--)
        {
            PxActor* actor = *actors++;
           sleepActor[index++] =getActorUUID(actor);
        }
        call<void>("onSleep",sleepActor);
        
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count) override
	{
		if(count==0)
            return;
		while (count--)
		{
            std::vector<LayaTriggerInfo> startTrigger;
            std::vector<LayaTriggerInfo> lostTrigger;
			const PxTriggerPair& current = *pairs++;
            LayaTriggerInfo triggerInfo;
            triggerInfo.triggerShape = getUUID(current.triggerShape);
            triggerInfo.otherShape = getUUID(current.otherShape);
            triggerInfo.triggerActor = getActorUUID(current.triggerActor);
            triggerInfo.otherActor = getActorUUID(current.otherActor);
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				startTrigger.push_back(triggerInfo);
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				lostTrigger.push_back(triggerInfo);
            if(startTrigger.size()>0){
                call<void>("onTriggerBegin",startTrigger);
            }
            if(lostTrigger.size()>0){
                call<void>("onTriggerEnd",lostTrigger);
            }
		}
	}

	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override
	{
		//printf("onAdvance\n");
	}

	void onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count) override
	{
		//		printf("onContact: %d pairs\n", count);
        if(count==0)
            return;
		std::vector<PxContactPairPoint> contactPoints;
        std::vector<LayaContactPairInfo> startContact;
        std::vector<LayaContactPairInfo> persistsContact;
        std::vector<LayaContactPairInfo> lostContact;
		while (count--)
		{
			const PxContactPair& current = *pairs++;
            LayaContactPairInfo partInfo;
            partInfo.pxShape0 = getUUID(current.shapes[0]);
            partInfo.pxShape1 = getUUID(current.shapes[1]);
            //start
            if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD | PxPairFlag::eNOTIFY_TOUCH_PERSISTS)){
				PxU32 contactCount = current.contactCount>4?4:current.contactCount;//4 point max
                partInfo.contactCount = contactCount;
				if (contactCount)
				{
					contactPoints.resize(contactCount);
					current.extractContacts(&contactPoints[0], contactCount);
					if( contactCount>0) partInfo.contactPoint0 = contactPoints[0];
                    if( contactCount>1) partInfo.contactPoint1 = contactPoints[1];
                    if( contactCount>2) partInfo.contactPoint2 = contactPoints[2];
                    if( contactCount>3) partInfo.contactPoint3 = contactPoints[3];
				}
                if(current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
                    startContact.push_back(partInfo);
                else
                    persistsContact.push_back(partInfo);
			}

			if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST){
                lostContact.push_back(partInfo);
            }
			if(contactPoints.size()>0){
                  call<void>("onContactBegin",startContact);
            }
            if(persistsContact.size()>0){
                  call<void>("onContactPersist",persistsContact);
            }
            if(lostContact.size()>0){
                  call<void>("onContactEnd",lostContact);
            }
		}
	}
};

// struct PxSimulationEventCallbackWrapper : public wrapper<PxSimulationEventCallback> {
//     EMSCRIPTEN_WRAPPER(explicit PxSimulationEventCallbackWrapper)

//     void onConstraintBreak(PxConstraintInfo *, PxU32) override {}

//     void onWake(PxActor **, PxU32) override {}

//     void onSleep(PxActor **, PxU32) override {}

//     void onContact(const PxContactPairHeader &, const PxContactPair *pairs, PxU32 nbPairs) override {
//         for (PxU32 i = 0; i < nbPairs; i++) {
//             const PxContactPair &cp = pairs[i];

//             if (cp.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD)) {
//                 call<void>("onContactBegin", getUUID(cp.shapes[0]), getUUID(cp.shapes[1]));
//             } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST) {
//                 if (!cp.flags.isSet(PxContactPairFlag::Enum::eREMOVED_SHAPE_0) &&
//                     !cp.flags.isSet(PxContactPairFlag::Enum::eREMOVED_SHAPE_1)) {
//                     call<void>("onContactEnd", getUUID(cp.shapes[0]), getUUID(cp.shapes[1]));
//                 }
//             } else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
//                 call<void>("onContactPersist", getUUID(cp.shapes[0]), getUUID(cp.shapes[1]));
//             }
//         }
//     }

//     void onTrigger(PxTriggerPair *pairs, PxU32 count) override {
//         for (PxU32 i = 0; i < count; i++) {
//             const PxTriggerPair &tp = pairs[i];

//             if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
//                 call<void>("onTriggerBegin", getUUID(tp.triggerShape), getUUID(tp.otherShape));
//             } else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST) {
//                 if (!tp.flags.isSet(PxTriggerPairFlag::Enum::eREMOVED_SHAPE_OTHER) &&
//                     !tp.flags.isSet(PxTriggerPairFlag::Enum::eREMOVED_SHAPE_TRIGGER)) {
//                     call<void>("onTriggerEnd", getUUID(tp.triggerShape), getUUID(tp.otherShape));
//                 }
//             }
//         }
//     }

//     void onAdvance(const PxRigidBody *const *, const PxTransform *, const PxU32) override {}
// };
//create Scene
PxSceneDesc *getDefaultSceneDesc(PxTolerancesScale &scale, int numThreads, PxSimulationEventCallback *callback) {
    auto *sceneDesc = new PxSceneDesc(scale);
    sceneDesc->gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc->cpuDispatcher = PxDefaultCpuDispatcherCreate(numThreads);
    sceneDesc->filterShader = triggersUsingFilterCallback;
    sceneDesc->simulationEventCallback = callback;
    return sceneDesc;
}

EMSCRIPTEN_BINDINGS(physx_scene) {
    function("getDefaultSceneDesc", &getDefaultSceneDesc, allow_raw_pointers());
    function("PxDefaultSimulationFilterShader", &PxDefaultSimulationFilterShader, allow_raw_pointers());

    class_<PxSimulationEventCallback>("PxSimulationEventCallback")
            .allow_subclass<ContactReportCallback>("ContactReportCallback");

    class_<PxSceneDesc>("PxSceneDesc").constructor<PxTolerancesScale>().property("gravity", &PxSceneDesc::gravity);

    class_<PxSceneFlags>("PxSceneFlags");
    enum_<PxSceneFlag::Enum>("PxSceneFlag")
            .value("eENABLE_ACTIVE_ACTORS ", PxSceneFlag::Enum::eENABLE_ACTIVE_ACTORS)
            .value("eENABLE_CCD", PxSceneFlag::Enum::eENABLE_CCD)
            .value("eDISABLE_CCD_RESWEEP", PxSceneFlag::Enum::eDISABLE_CCD_RESWEEP)
            .value("eENABLE_PCM", PxSceneFlag::Enum::eENABLE_PCM)
            .value("eDISABLE_CONTACT_REPORT_BUFFER_RESIZE", PxSceneFlag::Enum::eDISABLE_CONTACT_REPORT_BUFFER_RESIZE)
            .value("eDISABLE_CONTACT_CACHE", PxSceneFlag::Enum::eDISABLE_CONTACT_CACHE)
            .value("eREQUIRE_RW_LOCK", PxSceneFlag::Enum::eREQUIRE_RW_LOCK)
            .value("eENABLE_STABILIZATION", PxSceneFlag::Enum::eENABLE_STABILIZATION)
            .value("eENABLE_AVERAGE_POINT", PxSceneFlag::Enum::eENABLE_AVERAGE_POINT)
            .value("eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS", PxSceneFlag::Enum::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS)
            .value("eENABLE_GPU_DYNAMICS", PxSceneFlag::Enum::eENABLE_GPU_DYNAMICS)
            .value("eENABLE_ENHANCED_DETERMINISM", PxSceneFlag::Enum::eENABLE_ENHANCED_DETERMINISM)
            .value("eENABLE_FRICTION_EVERY_ITERATION", PxSceneFlag::Enum::eENABLE_FRICTION_EVERY_ITERATION)
            .value("eSUPPRESS_READBACK", PxSceneFlag::Enum::eSUPPRESS_READBACK)
            .value("eFORCE_READBACK", PxSceneFlag::Enum::eFORCE_READBACK)
            .value("eMUTABLE_FLAGS", PxSceneFlag::Enum::eMUTABLE_FLAGS);

            


    /** PhysXPhysicsManager ✅ */
    class_<PxScene>("PxScene")
            .function("setGravity", &PxScene::setGravity)  // ✅
            .function("getGravity", &PxScene::getGravity)
            .function("addActor", &PxScene::addActor, allow_raw_pointers())        // ✅
            .function("removeActor", &PxScene::removeActor, allow_raw_pointers())  // ✅
            .function("getScenePvdClient", &PxScene::getScenePvdClient, allow_raw_pointers())
            .function("getActors", &PxScene::getActors, allow_raw_pointers())
            .function("setVisualizationCullingBox", &PxScene::setVisualizationCullingBox)
            .function("collide",
                      optional_override([](PxScene &scene, PxReal elapsedTime) { scene.collide(elapsedTime); }))
            .function("setPVDClient",optional_override([](PxScene &scene) { 
                PxPvdSceneClient* pvdClient = scene.getScenePvdClient();
                if(pvdClient){
                    pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                }
            }))
            .function("fetchCollision", &PxScene::fetchCollision)
            .function("advance", optional_override([](PxScene &scene) { scene.advance(); }))
            .function("fetchResults", optional_override([](PxScene &scene, bool block) {
                          // fetchResults uses an out pointer
                          // which embind can't represent
                          // so let's override.
                          bool fetched = scene.fetchResults(block);
                          return fetched;
                      }))  // ✅
            .function("simulate", optional_override([](PxScene &scene, PxReal elapsedTime, bool controlSimulation) {
                          scene.simulate(elapsedTime, nullptr, nullptr, 0, controlSimulation);
                      }))  // ✅
            .function("raycastCloset",optional_override([](PxScene &scene,PxVec3 ori,PxVec3 dir,PxU32 filterGroup){
                LayaQuaryResult quaryResult;
                raycastClosest(scene,ori,dir,filterGroup,quaryResult);
                return quaryResult;
            }
            ))
            // .function("raycast", &PxScene::raycast, allow_raw_pointers())
            // .function("raycastAny", optional_override([](const PxScene &scene, const PxVec3 &origin,
            //                                              const PxVec3 &unitDir, const PxReal distance) {
            //               PxSceneQueryHit hit;
            //               return PxSceneQueryExt::raycastAny(scene, origin, unitDir, distance, hit);
            //           }))
            // .function("raycastSingle",
            //           optional_override([](const PxScene &scene, const PxVec3 &origin, const PxVec3 &unitDir,
            //                                const PxReal distance, PxRaycastHit &hit,
            //                                const PxSceneQueryFilterData &filterData,
            //                                PxQueryFilterCallbackWrapper *callback) {
            //               return PxSceneQueryExt::raycastSingle(scene, origin, unitDir, distance,
            //                                                     PxHitFlags(PxHitFlag::eDEFAULT), hit, filterData,
            //                                                     callback);
            //           }),
            //           allow_raw_pointers())  // ✅
            // .function("sweep", &PxScene::sweep, allow_raw_pointers())
            // .function("sweepAny",
            //           optional_override([](const PxScene &scene, const PxGeometry &geometry, const PxTransform &pose,
            //                                const PxVec3 &unitDir, const PxReal distance, PxSceneQueryFlags queryFlags) {
            //               PxSweepHit hit;
            //               return PxSceneQueryExt::sweepAny(scene, geometry, pose, unitDir, distance, queryFlags, hit);
            //           }))
            // .function("sweepSingle",
            //           optional_override([](const PxScene &scene, const PxGeometry &\, const PxTransform &pose,
            //                                const PxVec3 &unitDir, const PxReal distance, PxSceneQueryFlags outputFlags,
            //                                PxSweepHit &hit) {
            //               return PxSceneQueryExt::sweepSingle(scene, geometry, pose, unitDir, distance, outputFlags,
            //                                                   hit);
            //           }))
            .function("createControllerManager",
                      optional_override([](PxScene &scene) { return PxCreateControllerManager(scene); }),
                      allow_raw_pointers());  // ✅

    // class_<PxLocationHit>("PxLocationHit")
    //         .property("position", &PxLocationHit::position)
    //         .property("normal", &PxLocationHit::normal)
    //         .property("distance", &PxLocationHit::distance);
    // class_<PxRaycastHit, base<PxLocationHit>>("PxRaycastHit")
    //         .constructor<>()
    //         .function("getShape", optional_override([](PxRaycastHit &block) { return block.shape; }),
    //                   allow_raw_pointers());
    // class_<PxRaycastCallback>("PxRaycastCallback")
    //         .property("block", &PxRaycastCallback::block)
    //         .property("hasBlock", &PxRaycastCallback::hasBlock)
    //         .allow_subclass<PxRaycastCallbackWrapper>("PxRaycastCallbackWrapper", constructor<PxRaycastHit *, PxU32>());
    //class_<PxRaycastBuffer, base<PxRaycastCallback>>("PxRaycastBuffer").constructor<>();

    //function("allocateRaycastHitBuffers", &allocateRaycastHitBuffers, allow_raw_pointers());

    // class_<PxSweepHit, base<PxLocationHit>>("PxSweepHit")
    //         .constructor<>()
    //         .function("getShape", optional_override([](PxSweepHit &block) { return block.shape; }),
    //                   allow_raw_pointers())
    //         .function("getActor", optional_override([](PxSweepHit &block) { return block.actor; }),
    //                   allow_raw_pointers());
    // class_<PxSweepCallback>("PxSweepCallback")
    //         .property("block", &PxSweepCallback::block)
    //         .property("hasBlock", &PxSweepCallback::hasBlock)
    //         .allow_subclass<PxSweepCallbackWrapper>("PxSweepCallbackWrapper", constructor<PxSweepHit *, PxU32>());
    // class_<PxSweepBuffer, base<PxSweepCallback>>("PxSweepBuffer").constructor<>();

    //function("allocateSweepHitBuffers", &allocateSweepHitBuffers, allow_raw_pointers());

    class_<PxHitFlags>("PxHitFlags").constructor<int>();
    enum_<PxHitFlag::Enum>("PxHitFlag")
            .value("eDEFAULT", PxHitFlag::Enum::eDEFAULT)
            .value("eMESH_BOTH_SIDES", PxHitFlag::Enum::eMESH_BOTH_SIDES)
            .value("eMESH_MULTIPLE", PxHitFlag::Enum::eMESH_MULTIPLE);

    class_<PxQueryFilterData>("PxQueryFilterData").constructor<>().property("flags", &PxQueryFilterData::flags);
    class_<PxQueryFlags>("PxQueryFlags").constructor<int>();
    enum_<PxQueryFlag::Enum>("PxQueryFlag")
            .value("eANY_HIT", PxQueryFlag::Enum::eANY_HIT)
            .value("eDYNAMIC", PxQueryFlag::Enum::eDYNAMIC)
            .value("eSTATIC", PxQueryFlag::Enum::eSTATIC)
            .value("eNO_BLOCK", PxQueryFlag::Enum::eNO_BLOCK);
    enum_<PxQueryHitType::Enum>("PxQueryHitType")
            .value("eNONE", PxQueryHitType::Enum::eNONE)
            .value("eBLOCK", PxQueryHitType::Enum::eBLOCK)
            .value("eTOUCH", PxQueryHitType::Enum::eTOUCH);

    // class_<PxQueryFilterCallback>("PxQueryFilterCallback")
    //         .allow_subclass<PxQueryFilterCallbackWrapper>("PxQueryFilterCallbackWrapper", constructor<>());
    // class_<PxQueryCache>("PxQueryCache");
}

namespace emscripten {
namespace internal {
template <>
void raw_destructor<PxScene>(PxScene *) { /* do nothing */
}

}  // namespace internal
}  // namespace emscripten
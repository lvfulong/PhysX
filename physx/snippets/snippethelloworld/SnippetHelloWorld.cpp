// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2022 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************

#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "../snippetcommon/SnippetPrint.h"
#include "../snippetcommon/SnippetPVD.h"
#include "../snippetutils/SnippetUtils.h"
#include <vector>

using namespace physx;

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
static PxFoundation*			gFoundation = NULL;
static PxPhysics*				gPhysics	= NULL;
static PxDefaultCpuDispatcher*	gDispatcher = NULL;
static PxScene*					gScene		= NULL;
static PxMaterial*				gMaterial	= NULL;
static PxPvd*					gPvd        = NULL;

static PxReal stackZ = 10.0f;
static uint32_t userID = 0;



//----------------ray cast fillterGroup---------------------------------------------

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

//static PxCustomQueryFilterCallBack* queryFilterCallBack;

bool raycastClosest(const PxScene& Scene, const PxVec3& ori, const PxVec3& dir, PxU32 filterGroup) {
	PxRaycastBuffer hitBuffer;                 // [out] Raycast results
	//PxHitFlags
	PxHitFlags hitFlag = PxHitFlag::eDEFAULT|PxHitFlag::eANY_HIT;
	//PxQueryFilterData
	PxQueryFlags flags = PxQueryFlag::eANY_HIT | PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;
	PxQueryFilterData FilterDataSt(PxFilterData(filterGroup,0,0,0), flags);

	bool hitResult = Scene.raycast(ori, dir, 10000000, hitBuffer, hitFlag,FilterDataSt,&queryFilterCallBack);
	if (hitResult) {
		//printf("rayCast Cloest\n");
		PxRaycastHit hit = hitBuffer.block;
	
			//printf("rayCast anyHit Position %f,%f,%f\n", hit.position.x, hit.position.y, hit.position.z);
			//EventTODO
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
	unsigned int aa = 10;
	PxQueryFlags aaa = PxQueryFlags(aa);
	
	FilterDataSt.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eANY_HIT| aaa;

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
//--------------------collider event---------------------------------------------

struct LayaContactPairInfo
{
public:
	PxShape* pxShape1;
	PxShape* pxShape2;
	PxU8 contactCount;
	PxContactPairPoint contactPoint1;
	PxContactPairPoint contactPoint2;
	PxContactPairPoint ContactPoint3;
	PxContactPairPoint ContactPoint4;

};


class ContactReportCallback : public PxSimulationEventCallback
{
	void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/)
	{
		printf("onConstraintBreak\n");
	}

	void onWake(PxActor** /*actors*/, PxU32 count)
	{
		while (count--)
		{
			//PxActor* current = *actors++;
			//current.gettyp
			printf("onWake\n");
		}
	}

	void onSleep(PxActor** actors, PxU32 count)
	{
		while (count--)
		{
			const PxActor* current = *actors++;
			printf("onSleep\n");
		}
	}

	void onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		//		printf("onTrigger: %d trigger pairs\n", count);
		while (count--)
		{
			const PxTriggerPair& current = *pairs++;
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				printf("Shape is entering trigger volume\n");
			if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				printf("Shape is leaving trigger volume\n");
			
		}
	}

	void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
	{
		printf("onAdvance\n");
	}

	void onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count)
	{
		//		printf("onContact: %d pairs\n", count);
		std::vector<PxContactPairPoint> contactPoints;
		//contactPoints[0] -=
		
		while (count--)
		{
			const PxContactPair& current = *pairs++;
			if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD)){
				printf("Connect Found\n");
				PxU32 contactCount = current.contactCount;
				if (contactCount)
				{
					contactPoints.resize(contactCount);
					current.extractContacts(&contactPoints[0], contactCount);
					printf("contectPoint number %d\n", contactCount);
					for (PxU32 j = 0; j < contactCount; j++)
					{
						PxContactPairPoint contactPoint = contactPoints[j];
						printf("contect point Position %f,%f,%f\n", contactPoint.position.x, contactPoint.position.y, contactPoint.position.z);
						printf("contect point Normal %f,%f,%f\n", contactPoint.normal.x, contactPoint.normal.y, contactPoint.normal.z);
						printf("contect point implush %f,%f,%f\n", contactPoint.impulse.x, contactPoint.impulse.y, contactPoint.impulse.z);
					}
				}
			}

			if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				printf("Connect Lost\n");
			if (current.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
				printf("Connect stat\n");

			/*if (isTriggerShape(current.shapes[0]) && isTriggerShape(current.shapes[1]))
				printf("Trigger-trigger overlap detected\n");*/
		}
	}
}gContactReportCallback;

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
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}
		else {
			pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eNOTIFY_TOUCH_PERSISTS|PxPairFlag::eNOTIFY_CONTACT_POINTS;
			return PxFilterFlag::eDEFAULT;
		}
	}
	else {//cant filter
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		return PxFilterFlag::eSUPPRESS;
	}
}

class TriggersFilterCallback : public PxSimulationFilterCallback
{
	virtual		PxFilterFlags	pairFound(PxU32 /*pairID*/,
		PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/, const PxActor* /*a0*/, const PxShape* s0,
		PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/, const PxActor* /*a1*/, const PxShape* s1,
		PxPairFlags& pairFlags)
	{
		//		printf("pairFound\n");

		if (s0->userData || s1->userData)	// See createTriggerShape() function
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;

			/*if (usesCCD())
				pairFlags |= */PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eNOTIFY_TOUCH_CCD;
		}
		else
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

		return PxFilterFlag::eDEFAULT;
	}

	virtual		void			pairLost(PxU32 /*pairID*/,
		PxFilterObjectAttributes /*attributes0*/,
		PxFilterData /*filterData0*/,
		PxFilterObjectAttributes /*attributes1*/,
		PxFilterData /*filterData1*/,
		bool /*objectRemoved*/)
	{
		//		printf("pairLost\n");
	}

	virtual		bool			statusChange(PxU32& /*pairID*/, PxPairFlags& /*pairFlags*/, PxFilterFlags& /*filterFlags*/)
	{
		//		printf("statusChange\n");
		return false;
	}
}gTriggersFilterCallback;




static PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}


static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	PxFilterData filterData = PxFilterData();
	filterData.word0 = 0xffffffff;
	filterData.word1 = 0xffffffff;
	shape->setSimulationFilterData(filterData);
	shape->setQueryFilterData(filterData);
	//for(PxU32 i=0; i<size;i++)
	//{
	//	for(PxU32 j=0;j<size-i;j++)
	//	{
			PxTransform localTm(PxVec3(PxReal(0.0*2) - PxReal(size-0.0), PxReal(0.0*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
			body->attachShape(*shape);

			
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		/*}
	}\*/

	shape->release();
}

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	printf("using custom callback");
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	sceneDesc.filterShader	= triggersUsingFilterCallback;
	//sceneDesc.filterCallback = &gTriggersFilterCallback;

	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	
	gScene->addActor(*groundPlane);
	PxShape* a[1];
	groundPlane->getShapes(a, sizeof(PxShape*), 0);
	a[0]->setSimulationFilterData(PxFilterData(0xffffffff, 0xffffffff, 0, 0));

	for(PxU32 i=0;i<5;i++)
		createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);

	if(!interactive)
		createDynamic(PxTransform(PxVec3(0,40,100)), PxSphereGeometry(10), PxVec3(0,-50,-100));
}

void stepPhysics(bool /*interactive*/)
{
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}
	
void cleanupPhysics(bool /*interactive*/)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
	
	printf("SnippetHelloWorld done.\n");
}

void keyPress(unsigned char key, const PxTransform& camera)
{
	switch(toupper(key))
	{
	case 'B':	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);						break;
	case ' ':	createDynamic(camera, PxSphereGeometry(3.0f), camera.rotate(PxVec3(0,0,-1))*200);	break;
 	case 'V':  raycastClosest(*gScene, camera.p, camera.rotate(PxVec3(0, 0, -1)),0xffffffff); break;
	}
}

int snippetMain(int, const char*const*)
{
#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}

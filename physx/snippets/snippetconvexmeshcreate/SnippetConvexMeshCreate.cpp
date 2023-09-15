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
// This snippet creates convex meshes with different cooking settings 
// and shows how these settings affect the convex mesh creation performance and 
// the size of the resulting cooked meshes.
// ****************************************************************************

#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "../snippetutils/SnippetUtils.h"
#include "../../source/lowlevel/common/include/utils/PxcScratchAllocator.h"

using namespace physx;

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
static PxFoundation*			gFoundation = NULL;
static PxPhysics*				gPhysics	= NULL;

static float rand(float loVal, float hiVal)
{
	return loVal + (float(rand())/float(RAND_MAX))*(hiVal - loVal);
}

template<PxConvexMeshCookingType::Enum convexMeshCookingType, bool directInsertion, PxU32 gaussMapLimit>
static void createRandomConvex(PxU32 numVerts, const PxVec3* verts)
{
	PxTolerancesScale tolerances;
	PxCookingParams params(tolerances);

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.convexMeshCookingType = convexMeshCookingType;

	// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
	// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
	params.gaussMapLimit = gaussMapLimit;

	// Setup the convex mesh descriptor
	PxConvexMeshDesc desc;

	// We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
	desc.points.data = verts;
	desc.points.count = numVerts;
	desc.points.stride = sizeof(PxVec3);
	desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxU32 meshSize = 0;
	PxConvexMesh* convex = NULL;

	PxU64 startTime = SnippetUtils::getCurrentTimeCounterValue();

	if(directInsertion)
	{
		// Directly insert mesh into PhysX
		convex = PxCreateConvexMesh(params, desc, gPhysics->getPhysicsInsertionCallback());
		PX_ASSERT(convex);
	}
	else
	{
		// Serialize the cooked mesh into a stream.
		PxDefaultMemoryOutputStream outStream;
		bool res = PxCookConvexMesh(params, desc, outStream);
		PX_UNUSED(res);
		PX_ASSERT(res);
		meshSize = outStream.getSize();

		// Create the mesh from a stream.
		PxDefaultMemoryInputData inStream(outStream.getData(), outStream.getSize());
		convex = gPhysics->createConvexMesh(inStream);
		PX_ASSERT(convex);
	}

	// Print the elapsed time for comparison
	PxU64 stopTime = SnippetUtils::getCurrentTimeCounterValue();
	float elapsedTime = SnippetUtils::getElapsedTimeInMilliseconds(stopTime - startTime);
	printf("\t -----------------------------------------------\n");
	printf("\t Create convex mesh with %d triangles: \n", numVerts);
	directInsertion ? printf("\t\t Direct mesh insertion enabled\n") : printf("\t\t Direct mesh insertion disabled\n");
	printf("\t\t Gauss map limit: %d \n", gaussMapLimit);
	printf("\t\t Created hull number of vertices: %d \n", convex->getNbVertices());
	printf("\t\t Created hull number of polygons: %d \n", convex->getNbPolygons());
	printf("\t Elapsed time in ms: %f \n", double(elapsedTime));
	if (!directInsertion)
	{
		printf("\t Mesh size: %d \n", meshSize);
	}

	convex->release();
}

static void createConvexMeshes()
{
	const PxU32 numVerts = 64;
	PxVec3* vertices = new PxVec3[numVerts];

	// Prepare random verts
	for(PxU32 i = 0; i < numVerts; i++)
	{
		vertices[i] = PxVec3(rand(-20.0f, 20.0f), rand(-20.0f, 20.0f), rand(-20.0f, 20.0f));
	}

	// Create convex mesh using the quickhull algorithm with different settings
	printf("-----------------------------------------------\n");
	printf("Create convex mesh using the quickhull algorithm: \n\n");

	// The default convex mesh creation serializing to a stream, useful for offline cooking.
	createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 16>(numVerts, vertices);

	// The default convex mesh creation without the additional gauss map data.
	createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, false, 256>(numVerts, vertices);

	// Convex mesh creation inserting the mesh directly into PhysX. 
	// Useful for runtime cooking.
	createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 16>(numVerts, vertices);

	// Convex mesh creation inserting the mesh directly into PhysX, without gauss map data.
	// Useful for runtime cooking.
	createRandomConvex<PxConvexMeshCookingType::eQUICKHULL, true, 256>(numVerts, vertices);

	delete [] vertices;
}

void initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true);
}
	
void cleanupPhysics()
{
	PX_RELEASE(gPhysics);
	PX_RELEASE(gFoundation);
	
	printf("SnippetConvexMeshCreate done.\n");
}


int snippetMain(int, const char*const*)
{	
	initPhysics();
	createConvexMeshes();
	cleanupPhysics();

	return 0;
}


static const PxVec3 convexVerts[] = { PxVec3(0,1,0),PxVec3(1,0,0),PxVec3(-1,0,0),PxVec3(0,0,1),PxVec3(0,0,-1) };

void* testCreateConvex(PxRigidActor& aConvexActor,PxMaterial& aMaterial) {
	//��������
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	//range 4-255
	//���ʹ��PxConverxFlag:��eCHECK_ZERO_AREA_TRIANGLES�����㷨���������С��PxCookingParams:��areaTestEpsilon�������Ρ�����㷨�Ҳ���4��û��С�����εĳ�ʼ���㣬�򷵻�PxConverxMeshCookingResult:��eZERO_AREA_TEST_FAILED������ζ���ṩ�Ķ���λ�ڷǳ�С�������ڣ������޷�������Ч����ǡ�Ϊ�˻��͹����⿵�³�����ԣ������ʧ�ܵ�����¿���ʹ��AABB��OBB��
	//����ṩ�˴��������붥�㣬�������붥����ܻ�����ã�����������£�ʹ��PxConverxFlag:��eQUANTIZE_input�����������PxConvexMeshDesc:��quantizedCount��
	convexDesc.vertexLimit = 10;
	//cooking��
	PxTolerancesScale scale;
	PxCookingParams params(scale);
	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
	if (!PxCookConvexMesh(params, convexDesc, buf, &result))
		return NULL;
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);

	PxRigidActorExt::createExclusiveShape(aConvexActor,
		PxConvexMeshGeometry(convexMesh), aMaterial);
	return NULL;
	
}

void testFast_INERTIA_COMPUTATION(PxRigidActor& aConvexActor, PxMaterial& aMaterial) {
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;
	
	PxTolerancesScale scale;
	PxCookingParams cookingParams(scale);
#ifdef _DEBUG
	// mesh should be validated before cooking without the mesh cleaning
	//��ע�⣬���Ժͼ��Ĺ�����Ҫ������֤����Ϊ��δ��֤����������������������ܻᵼ��δ�������Ϊ
	//���ɽ�ʹ��SIMD����·������·���ļ����ٶȸ��죬�����ȸ��͡�
	//͹������֧�ָ�������
	bool res = PxValidateConvexMesh(cookingParams, convexDesc);
	PX_ASSERT(res);
#endif

	PxConvexMesh* aConvexMesh = PxCreateConvexMesh(cookingParams, convexDesc,
		gPhysics->getPhysicsInsertionCallback());

	PxConvexMeshGeometry geometry = PxConvexMeshGeometry(aConvexMesh);
	geometry.meshFlags = PxConvexMeshGeometryFlag::Enum::eTIGHT_BOUNDS;//�������ʵ�����Ǽ���ɱ����ߣ��������͹�����໥����ʱ���������ģ�����ܡ�

	PxRigidActorExt::createExclusiveShape(aConvexActor, geometry, aMaterial);
}




//Triangle Meshes
//��PxCookingParams�У�
//scale defines���ڼ���������Ƿ�̫�󡣴˼�齫���������ģ���ȶ��ԡ�
//suppressTriangleMeshRemapTable ָ���Ƿ񴴽�����ӳ������û�У��⽫��ʡ�����ڴ棬��SDK���޷��ṩ�й���ײ��ɨ������Ͷ���������ĸ�ԭʼ���������ε���Ϣ��
//buildTriangleAdjacenciesָ���Ƿ񴴽��������ڽ���Ϣ������ʹ��getTriangle�������������ε����������Ρ�
//meshPreprocessParamsָ������Ԥ���������
	//PxMeshPreprocessingFlag:��eWELD_VERTICES����������������⿹��������ö��㺸�ӡ�
			//PxMeshPreprocessingFlag:��eDISABLE_CLEAN_MESH��������������̡������������˫���ԣ������о޴������β��ԡ�δ��ɶ��㺸�ӡ�ȷʵ�ӿ�������ٶȡ�
			//PxMeshPreprocessingFlag:��eDISABLE_ACTIVE_EDGES_PRECOMPUTE���ö����Ԥ���㡣ʹ��⿸��죬��������Ӵ��Ĳ�����
//meshWeldTolerance-������������񺸽ӣ�����ƺ��Ӷ���ľ��롣���δ�������񺸽ӣ����ֵ����������֤�Ľ��ܾ��롣����˾�����û���������㣬����Ϊ�����Ǹɾ��ġ����򣬽��������档Ϊ�˻��������ܣ���Ҫ��һ���ɾ���ɸ����
//midphaseDescָ��������м�׶μ��ٶȽṹ��������
		//PxBVH33MidphaseDsc-PxMeshMidPhase:��eBVH33����PhysX 3.3֮ǰ�ľɰ汾��ʹ�õġ����������õ����ܣ�����ƽ̨��֧������
		//PxBVH34MidphaseDsc - PxMeshMidPhase:��eBVH34��PhysX 3.4�������һ�����·��ʵ�ʵ�֣�Ŀǰ����Ĭ�Ͻṹ������������ܺ�����ʱ���ܷ��涼�������Ը��죬��Ŀǰ����֧��SSE2ָ���ƽ̨�Ͽ��á�
PxTriangleMesh* TriangleMeshesCookTest(PxTolerancesScale scale) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = 5;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = convexVerts;

	PxU32 indices32[12];
	meshDesc.triangles.count = 4;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices32;
	
	PxTolerancesScale scale;
	PxCookingParams params(scale);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status)
		return NULL;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return gPhysics->createTriangleMesh(readBuffer);
	PxTriangleMeshGeometry();
}


//HeightFields
//Rowscale  Columnscale
//Sample[0]-sample[N]�Ƕ���
//�� - X��
//�� - Z��
//�߶� - Y��
//����˼�壬���ο���ͨ��������β��������ϵĸ߶�ֵ��������
void testHeightFields(int* heightData) {
	int numRows = 4;
	int numCols = 4;
	int heightScale = 1;
	int rowScale = 1;
	int colScale = 1;
	PxHeightFieldSample* samples = (PxHeightFieldSample*)gAllocator.allocate(sizeof(PxHeightFieldSample) *(numRows * numCols),0,0,0);
	for (int i = 0, n = numRows * numCols; i < n; i++) {
		samples[i].height = heightData[i];
	}
	
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = numCols;
	hfDesc.nbRows = numRows;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	PxHeightField* aHeightField = PxCreateHeightField(hfDesc,
		gPhysics->getPhysicsInsertionCallback());

	PxHeightFieldGeometry hfGeom(aHeightField, PxMeshGeometryFlags(), heightScale, rowScale,
		colScale);
	//PxShape* aHeightFieldShape = PxRigidActorExt::createExclusiveShape(*aHeightFieldActor,
	//	hfGeom, aMaterialArray, nbMaterials);

	//localScaledVertex = PxVec3(row * rowScale, PxF32(heightSample) * heightScale,
	//	col * columnScale)
	//	worldVertex = shapeTransform(localScaledVertex)
}
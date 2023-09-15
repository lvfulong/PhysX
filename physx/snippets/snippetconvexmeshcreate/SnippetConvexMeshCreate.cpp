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
	//创建描述
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	//range 4-255
	//如果使用PxConverxFlag:：eCHECK_ZERO_AREA_TRIANGLES，则算法不包括面积小于PxCookingParams:：areaTestEpsilon的三角形。如果算法找不到4个没有小三角形的初始顶点，则返回PxConverxMeshCookingResult:：eZERO_AREA_TEST_FAILED。这意味着提供的顶点位于非常小的区域内，炊具无法生成有效的外壳。为了获得凸网烹饪的鲁棒策略，在烹饪失败的情况下可以使用AABB或OBB。
	//如果提供了大量的输入顶点，量化输入顶点可能会很有用，在这种情况下，使用PxConverxFlag:：eQUANTIZE_input并设置所需的PxConvexMeshDesc:：quantizedCount。
	convexDesc.vertexLimit = 10;
	//cooking成
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
	//请注意，调试和检查的构建需要网格验证，因为从未验证的输入描述符创建网格可能会导致未定义的行为
	//卷集成将使用SIMD代码路径，该路径的计算速度更快，但精度更低。
	//凸面网格不支持负比例。
	bool res = PxValidateConvexMesh(cookingParams, convexDesc);
	PX_ASSERT(res);
#endif

	PxConvexMesh* aConvexMesh = PxCreateConvexMesh(cookingParams, convexDesc,
		gPhysics->getPhysicsInsertionCallback());

	PxConvexMeshGeometry geometry = PxConvexMeshGeometry(aConvexMesh);
	geometry.meshFlags = PxConvexMeshGeometryFlag::Enum::eTIGHT_BOUNDS;//这个更紧实，但是计算成本更高，但当许多凸对象相互作用时，可以提高模拟性能。

	PxRigidActorExt::createExclusiveShape(aConvexActor, geometry, aMaterial);
}




//Triangle Meshes
//在PxCookingParams中：
//scale defines用于检查三角形是否不太大。此检查将有助于提高模拟稳定性。
//suppressTriangleMeshRemapTable 指定是否创建面重映射表。如果没有，这将节省大量内存，但SDK将无法提供有关碰撞、扫描或光线投射命中中哪个原始网格三角形的信息。
//buildTriangleAdjacencies指定是否创建三角形邻接信息。可以使用getTriangle检索给定三角形的相邻三角形。
//meshPreprocessParams指定网格预处理参数。
	//PxMeshPreprocessingFlag:：eWELD_VERTICES可在三角形网格烹饪过程中启用顶点焊接。
			//PxMeshPreprocessingFlag:：eDISABLE_CLEAN_MESH禁用网格清理过程。不搜索顶点的双面性，不进行巨大三角形测试。未完成顶点焊接。确实加快了烹饪速度。
			//PxMeshPreprocessingFlag:：eDISABLE_ACTIVE_EDGES_PRECOMPUTE禁用顶点边预计算。使烹饪更快，但会减缓接触的产生。
//meshWeldTolerance-如果启用了网格焊接，则控制焊接顶点的距离。如果未启用网格焊接，则该值定义网格验证的接受距离。如果此距离内没有两个顶点，则认为网格是干净的。否则，将发出警告。为了获得最佳性能，需要有一个干净的筛网。
//midphaseDesc指定所需的中间阶段加速度结构描述符。
		//PxBVH33MidphaseDsc-PxMeshMidPhase:：eBVH33是在PhysX 3.3之前的旧版本中使用的。它具有良好的性能，所有平台都支持它。
		//PxBVH34MidphaseDsc - PxMeshMidPhase:：eBVH34是PhysX 3.4中引入的一个重新访问的实现，目前它是默认结构。它在烹饪性能和运行时性能方面都可以明显更快，但目前仅在支持SSE2指令集的平台上可用。
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
//Sample[0]-sample[N]是顶点
//行 - X轴
//列 - Z轴
//高度 - Y轴
//顾名思义，地形可以通过规则矩形采样网格上的高度值来描述：
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
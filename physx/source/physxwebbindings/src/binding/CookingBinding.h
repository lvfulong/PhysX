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

// PxConvexMesh *createConvexMeshFromBuffer(int vertices,
//                                          PxU32 vertCount,
//                                          int indices,
//                                          PxU32 indexCount,
//                                          bool isU16,
//                                          PxCooking &cooking,
//                                          PxPhysics &physics) {
//     PxConvexMeshDesc convexDesc;
//     convexDesc.points.count = vertCount;
//     convexDesc.points.stride = sizeof(PxVec3);
//     convexDesc.points.data = (PxVec3 *)vertices;
//     if (isU16) {
//         convexDesc.indices.stride = 3 * sizeof(PxU16);
//         convexDesc.indices.data = (PxU16 *)indices;
//         convexDesc.flags = PxConvexFlag::e16_BIT_INDICES;
//     } else {
//         convexDesc.indices.stride = 3 * sizeof(PxU32);
//         convexDesc.indices.data = (PxU32 *)indices;
//     }
//     convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

//     PxConvexMesh *convexMesh = cooking.createConvexMesh(convexDesc, physics.getPhysicsInsertionCallback());

//     return convexMesh;
// }

//eCOMPUTE_CONVEX;
PxConvexMesh *createConvexMeshFromBuffer(PxVec3* vertices, PxU32 vertCount, PxPhysics &physics,PxU32 VetexLimit,PxTolerancesScale &scale,int ConvexFlags) {
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = vertCount;
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = vertices;
    convexDesc.flags = PxConvexFlag::Enum(ConvexFlags)
	convexDesc.vertexLimit = vertexLimit;
    PxCookingParams params(scale);
	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;
    if(!PxCookConvexMesh(params,convexDesc,buf,&result)){
        return NULL;
    }
    PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
    PxConvexMesh* convexMesh = physics.createConvexMesh(input);
    return convexMesh;
}

PxTriangleMesh *createTriMesh(PxVec3* vertices,
                              PxU32 vertCount,
                              int indices,
                              PxU32 indexCount,
                              bool isU16,
                              PxTolerancesScale &scale,
                              PxPhysics &physics) {
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = vertCount;
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices;

    meshDesc.triangles.count = indexCount;
    if (isU16) {
        meshDesc.triangles.stride = 3 * sizeof(PxU16);
        meshDesc.triangles.data = (PxU16 *)indices;
        meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
    } else {
        meshDesc.triangles.stride = 3 * sizeof(PxU32);
        meshDesc.triangles.data = (PxU32 *)indices;
    }
	PxCookingParams params(scale);

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = PxCookTriangleMesh(params, meshDesc, writeBuffer, &result);
	if (!status)
		return NULL;

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    return gPhysphysicsics->createTriangleMesh(readBuffer);;
}

PxHeightField* createHeightField(int numRows,int numCols,
                                PxI16* heightData,
                              PxDefaultAllocator &gAllocator
                              PxTolerancesScale &scale,
                              PxPhysics &physics
){
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
    return aHeightField;
}

EMSCRIPTEN_BINDINGS(physx_cooking) {
    function("PxCreateCooking", &PxCreateCooking, allow_raw_pointers());

    class_<PxMeshScale>("PxMeshScale").constructor<const PxVec3 &, const PxQuat &>();
    //triangleMesh
    class_<PxTriangleMesh>("PxTriangleMesh").function("release", &PxTriangleMesh::release);
    class_<PxTriangleMeshGeometry, base<PxGeometry>>("PxTriangleMeshGeometry")
            .constructor<PxTriangleMesh *, const PxMeshScale &, PxMeshGeometryFlags>()
            .property("scale",&PxTriangleMeshGeometry::scale),
    class_<PxMeshGeometryFlags>("PxMeshGeometryFlags").constructor<int>();
    enum_<PxMeshGeometryFlag::Enum>("PxMeshGeometryFlag")
            .value("eDOUBLE_SIDED",PxMeshGeometryFlag::Enum::eTIGHT_BOUNDS);
            .value("eDOUBLE_SIDED", PxMeshGeometryFlag::Enum::eDOUBLE_SIDED);
    //ConvexMesh
    class_<PxConvexMesh>("PxConvexMesh").function("release", &PxConvexMesh::release);
    class_<PxConvexMeshGeometry, base<PxGeometry>>("PxConvexMeshGeometry")
            .constructor<PxConvexMesh *, const PxMeshScale &, PxConvexMeshGeometryFlags>()
            .property("scale",&PxConvexMeshGeometry::scale);
    class_<PxConvexMeshGeometryFlags>("PxConvexMeshGeometryFlags").constructor<int>();
    enum_<PxConvexMeshGeometryFlag::Enum>("PxConvexMeshGeometryFlag")
            .value("eTIGHT_BOUNDS", PxConvexMeshGeometryFlag::Enum::eTIGHT_BOUNDS);

    //heightField
    class_<PxHeightField>("PxHeightField").function("release",&PxHeightField.release);
    class_<PxHeightFieldGeometry>("PxHeightFieldGeometry").constructor<PxHeightField*,	PxMeshGeometryFlags ,PxReal ,PxReal ,PxReal >();
    // class_<PxCooking>("PxCooking")
    //         .function("createConvexMeshWithIndices",
    //                   optional_override([](PxCooking &cooking, int vertices, PxU32 vertCount, int indices,
    //                                        PxU32 indexCount, bool isU16, PxPhysics &physics) {
    //                       return createConvexMeshFromBuffer(vertices, vertCount, indices, indexCount, isU16, cooking,
    //                                                         physics);
    //                   }),
    //                   allow_raw_pointers())
    //         .function("createConvexMesh",
    //                   optional_override([](PxCooking &cooking, int vertices, PxU32 vertCount, PxPhysics &physics) {
    //                       return createConvexMeshFromBuffer(vertices, vertCount, cooking, physics);
    //                   }),
    //                   allow_raw_pointers())
    //         .function("createTriMesh",
    //                   optional_override([](PxCooking &cooking, int vertices, PxU32 vertCount, int indices,
    //                                        PxU32 indexCount, bool isU16, PxPhysics &physics) {
    //                       return createTriMesh(vertices, vertCount, indices, indexCount, isU16, cooking, physics);
    //                   }),
    //                   allow_raw_pointers());
    class_<PxCookingParams>("PxCookingParams").constructor<PxTolerancesScale>();
}

namespace emscripten {
namespace internal {
template <>
// void raw_destructor<PxCooking>(PxCooking *) { /* do nothing */
// }

template <>
void raw_destructor<PxConvexMesh>(PxConvexMesh *) { /* do nothing */
}

template <>
void raw_destructor<PxTriangleMesh>(PxTriangleMesh *) { /* do nothing */
}
}  // namespace internal
}  // namespace emscripten
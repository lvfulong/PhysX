##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##  * Neither the name of NVIDIA CORPORATION nor the names of its
##    contributors may be used to endorse or promote products derived
##    from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
## EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
## CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
## PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
## OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Copyright (c) 2018-2019 NVIDIA Corporation. All rights reserved.

#
# Build PhysXFoundation
#

SET(PXSHARED_PLATFORM_HEADERS
	${PXSHARED_PATH}/include/foundation/unix/PxUnixIntrinsics.h	
)
SOURCE_GROUP(shared\\include\\unix FILES ${PXSHARED_PLATFORM_HEADERS})

SET(PXFOUNDATION_LIBTYPE STATIC)

SET(PHYSXFOUNDATION_PLATFORM_SOURCE
	${LL_SOURCE_DIR}/unix/FdUnixAtomic.cpp
	${LL_SOURCE_DIR}/unix/FdUnixMutex.cpp
	${LL_SOURCE_DIR}/unix/FdUnixSync.cpp
	${LL_SOURCE_DIR}/unix/FdUnixThread.cpp
	${LL_SOURCE_DIR}/unix/FdUnixPrintString.cpp
	${LL_SOURCE_DIR}/unix/FdUnixSList.cpp
	${LL_SOURCE_DIR}/unix/FdUnixSocket.cpp
	${LL_SOURCE_DIR}/unix/FdUnixTime.cpp
	${LL_SOURCE_DIR}/unix/FdUnixFPU.cpp
)
SOURCE_GROUP("src\\src\\unix" FILES ${PHYSXFOUNDATION_PLATFORM_FILES})

SET(PHYSXFOUNDATION_PLATFORM_INCLUDES
	${PHYSX_ROOT_DIR}/include/linux
)

SET(PHYSXFOUNDATION_NEON_FILES
	${PHYSX_ROOT_DIR}/include/foundation/unix/neon/PxUnixNeonAoS.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/neon/PxUnixNeonInlineAoS.h
)

SET(PHYSXFOUNDATION_SSE2_FILES
	${PHYSX_ROOT_DIR}/include/foundation/unix/sse2/PxUnixSse2AoS.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/sse2/PxUnixSse2InlineAoS.h
)

SET(PHYSXFOUNDATION_PLATFORM_SOURCE_HEADERS
	${PHYSX_ROOT_DIR}/include/foundation/unix/PxUnixAoS.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/PxUnixFPU.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/PxUnixInlineAoS.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/PxUnixIntrinsics.h
	${PHYSX_ROOT_DIR}/include/foundation/unix/PxUnixTrigConstants.h
)
SOURCE_GROUP("src\\include\\unix" FILES ${PHYSXFOUNDATION_PLATFORM_SOURCE_HEADERS})

INSTALL(FILES ${PHYSXFOUNDATION_PLATFORM_SOURCE_HEADERS} DESTINATION include/foundation/unix)
INSTALL(FILES ${PHYSXFOUNDATION_NEON_FILES} DESTINATION include/foundation/unix/neon)
INSTALL(FILES ${PHYSXFOUNDATION_SSE2_FILES} DESTINATION include/foundation/unix/sse2)
INSTALL(FILES ${PXSHARED_PLATFORM_HEADERS} DESTINATION include/foundation/unix)

SET(PHYSXFOUNDATION_PLATFORM_FILES
	${PHYSXFOUNDATION_PLATFORM_SOURCE}
	${PHYSXFOUNDATION_PLATFORM_SOURCE_HEADERS}
	${PHYSXFOUNDATION_NEON_FILES}
	${PHYSXFOUNDATION_SSE2_FILES}
	${PHYSXFOUNDATION_RESOURCE_FILE}
)



# Use generator expressions to set config specific preprocessor definitions
SET(PHYSXFOUNDATION_COMPILE_DEFS
	# Common to all configurations
	${PHYSX_EMSCRIPTEN_COMPILE_DEFS};${PXFOUNDATION_LIBTYPE_DEFS}

	$<$<CONFIG:debug>:${PHYSX_EMSCRIPTEN_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PHYSX_EMSCRIPTEN_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PHYSX_EMSCRIPTEN_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PHYSX_EMSCRIPTEN_RELEASE_COMPILE_DEFS};>
)

SET(PXFOUNDATION_PLATFORM_LINK_FLAGS "-m64")

cmake_minimum_required(VERSION 3.0)
project(NoobWerkz)

# set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_BUILD_TYPE "RELEASE")

set(CMAKE_CXX_FLAGS "-std=c++1y -fpermissive -DASIO_STANDALONE -DOM_STATIC_BUILD -DOM_GARBAGE_NO_STATUS_WARNING -DAS_CAN_USE_CPP11 -DEIGEN_MPL2_ONLY ${CMAKE_CXX_FLAGS}") 
set(CMAKE_C_FLAGS "-std=c99 -pipe -O2 -fno-strict-aliasing -DSQLITE_ENABLE_COLUMN_METADATA -DSTB_IMAGE_STATIC -DSQLITE_ENABLE_FTS4 -DSQLITE_ENABLE_RTREE -Wall ${CMAKE_C_FLAGS}")

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

include_directories(lib/brigand)

include_directories(lib/cppformat)
list (APPEND cppformat_src
	lib/cppformat/format.cc
	lib/cppformat/posix.cc
	)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

include_directories(lib/freetype/include)
add_subdirectory(lib/freetype)

include_directories(lib/voropp/src)
list (APPEND voropp_src
	lib/voropp/src/voro++.cc
	)

# Used to link to the system lib, but then had problems. Now following Couman's advice in http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=7168
include_directories(lib/bullet/src)
list (APPEND bullet_src
	lib/bullet/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp
	lib/bullet/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp
	lib/bullet/src/BulletDynamics/Dynamics/Bullet-C-API.cpp
	lib/bullet/src/BulletDynamics/Dynamics/btRigidBody.cpp
	lib/bullet/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp
	lib/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp
	lib/bullet/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyConstraint.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyJointMotor.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBody.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyPoint2Point.cpp
	lib/bullet/src/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.cpp
	lib/bullet/src/BulletDynamics/Character/btKinematicCharacterController.cpp
	lib/bullet/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp
	lib/bullet/src/BulletDynamics/Vehicle/btWheelInfo.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp
	lib/bullet/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btGhostObject.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btUnionFind.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/Gimpact/gim_box_set.cpp
	lib/bullet/src/BulletCollision/Gimpact/gim_contact.cpp
	lib/bullet/src/BulletCollision/Gimpact/btGImpactShape.cpp
	lib/bullet/src/BulletCollision/Gimpact/gim_tri_collision.cpp
	lib/bullet/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp
	lib/bullet/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp
	lib/bullet/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp
	lib/bullet/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp
	lib/bullet/src/BulletCollision/Gimpact/btContactProcessing.cpp
	lib/bullet/src/BulletCollision/Gimpact/gim_memory.cpp
	lib/bullet/src/BulletCollision/Gimpact/btGImpactBvh.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btCompoundShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btBoxShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btShapeHull.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btCollisionShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btBox2dShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConeShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btEmptyShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btCylinderShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btSphereShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp
	lib/bullet/src/BulletCollision/CollisionShapes/btConcaveShape.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btDbvt.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp
	lib/bullet/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp
	lib/bullet/src/LinearMath/btQuickprof.cpp
	lib/bullet/src/LinearMath/btAlignedAllocator.cpp
	lib/bullet/src/LinearMath/btSerializer.cpp
	lib/bullet/src/LinearMath/btPolarDecomposition.cpp
	lib/bullet/src/LinearMath/btConvexHull.cpp
	lib/bullet/src/LinearMath/btGeometryUtil.cpp
	lib/bullet/src/LinearMath/btConvexHullComputer.cpp
	lib/bullet/src/LinearMath/btVector3.cpp
	lib/bullet/src/BulletSoftBody/btDefaultSoftBodySolver.cpp
	lib/bullet/src/BulletSoftBody/btSoftBody.cpp
	lib/bullet/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp
	lib/bullet/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp
	lib/bullet/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp
	lib/bullet/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp
	lib/bullet/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp
	lib/bullet/src/BulletSoftBody/btSoftBodyHelpers.cpp
	#lib/bullet/src/MiniCL/MiniCL.cpp
	#lib/bullet/src/MiniCL/MiniCLTask/MiniCLTask.cpp
	#lib/bullet/src/MiniCL/MiniCLTaskScheduler.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/btSoftBodySolver_DX11SIMDAware.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/btSoftBodySolver_DX11.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolver_OpenCL.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolverOutputCLtoGL.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolver_OpenCLSIMDAware.cpp
	#lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/MiniCLTaskWrap.cpp
	lib/bullet/src/BulletMultiThreaded/btParallelConstraintSolver.cpp
	lib/bullet/src/BulletMultiThreaded/SpuSampleTaskProcess.cpp
	lib/bullet/src/BulletMultiThreaded/SpuLibspe2Support.cpp
	lib/bullet/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.cpp
	lib/bullet/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/boxBoxDistance.cpp
	lib/bullet/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuContactResult.cpp
	lib/bullet/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuMinkowskiPenetrationDepthSolver.cpp
	lib/bullet/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuCollisionShapes.cpp
	lib/bullet/src/BulletMultiThreaded/Win32ThreadSupport.cpp
	lib/bullet/src/BulletMultiThreaded/SpuFakeDma.cpp
	lib/bullet/src/BulletMultiThreaded/SpuCollisionTaskProcess.cpp
	lib/bullet/src/BulletMultiThreaded/PosixThreadSupport.cpp
	lib/bullet/src/BulletMultiThreaded/btThreadSupportInterface.cpp
	lib/bullet/src/BulletMultiThreaded/SpuContactManifoldCollisionAlgorithm.cpp
	lib/bullet/src/BulletMultiThreaded/SequentialThreadSupport.cpp
	lib/bullet/src/BulletMultiThreaded/SpuCollisionObjectWrapper.cpp
	lib/bullet/src/BulletMultiThreaded/SpuGatheringCollisionDispatcher.cpp
	lib/bullet/src/BulletMultiThreaded/btGpu3DGridBroadphase.cpp
	lib/bullet/src/BulletMultiThreaded/SpuSampleTask/SpuSampleTask.cpp

	)

include_directories(lib/polyvox/include)

# include_directories(# lib/dcollide)
# list (APPEND dcollide_src
# lib/dcollide/debug.cpp

# lib/dcollide/broadphase/broadphase.cpp
# lib/dcollide/broadphase/broadphasefactory.cpp
# lib/dcollide/broadphase/broadphaseproxydata.cpp
# lib/dcollide/broadphase/broadphasejob.cpp
# lib/dcollide/broadphase/broadphasecollisions.cpp

# 	Broadphase Octree Files
# lib/dcollide/broadphase/broadphasehierarchicalgrid.cpp
# lib/dcollide/broadphase/broadphasehierarchicalgridproxydata.cpp
# lib/dcollide/broadphase/broadphasehierarchicalgridjob.cpp
# lib/dcollide/broadphase/broadphasehierarchicalgridjob2.cpp
# lib/dcollide/broadphase/broadphasehierarchicalgridjobcollection.cpp
# lib/dcollide/broadphase/hierarchicalgrid.cpp

# 	BroadPhase Brute Force Files
# lib/dcollide/broadphase/broadphasebruteforce.cpp
# lib/dcollide/broadphase/broadphasebruteforcejob.cpp

# lib/dcollide/math/matrix.cpp
# lib/dcollide/math/quaternion.cpp
# lib/dcollide/math/vector.cpp
# lib/dcollide/math/plane.cpp
# lib/dcollide/world.cpp
# lib/dcollide/pipeline.cpp
# lib/dcollide/worldparameters.cpp
# lib/dcollide/worldcollisions.cpp
# lib/dcollide/proxy.cpp
# lib/dcollide/proxyfactory.cpp
# lib/dcollide/thread/thread.cpp
# lib/dcollide/thread/jobpool.cpp
# lib/dcollide/thread/jobpoolcollection.cpp
# lib/dcollide/thread/threadpool.cpp
# lib/dcollide/thread/threadjobcollection.cpp
# lib/dcollide/debuglog.cpp
# lib/dcollide/debugstream.cpp
# lib/dcollide/debugstreamconfiguration_initialize.cpp

# lib/dcollide/detectordeform/detectordeformmanager.cpp
# lib/dcollide/detectordeform/detectordeformalgorithm.cpp
# lib/dcollide/detectordeform/detectordeformjob.cpp
# lib/dcollide/detectordeform/detectordeformproxydata.cpp
# lib/dcollide/detectordeform/spatialhash/intvector3.cpp
# lib/dcollide/detectordeform/spatialhash/simplehash.cpp
# lib/dcollide/detectordeform/spatialhash/spatialhash.cpp
# lib/dcollide/detectordeform/spatialhash/spatialgrid.cpp
# lib/dcollide/detectordeform/spatialhash/spatialhashalgorithm.cpp
# lib/dcollide/detectordeform/spatialhash/spatialhashjobcollection.cpp
# lib/dcollide/detectordeform/surfacehierarchy/surfacehierarchy.cpp
# lib/dcollide/detectordeform/surfacehierarchy/normalcone.cpp
# lib/dcollide/detectordeform/surfacehierarchy/deformablebvhnode.cpp
# lib/dcollide/detectordeform/surfacehierarchy/surfacehierarchyalgorithm.cpp
# lib/dcollide/detectordeform/surfacehierarchy/surfacehierarchyjob.cpp
# lib/dcollide/detectordeform/surfacehierarchy/surfacehierarchyselfjob.cpp
# lib/dcollide/detectordeform/surfacehierarchy/surfacehierarchyproxydata.cpp
# lib/dcollide/detectordeform/bvhierarchy/bvhierarchyalgorithm.cpp
# lib/dcollide/detectordeform/bvhierarchy/bvhierarchybvhnode.cpp
# lib/dcollide/detectordeform/bvhierarchy/bvhierarchyjob.cpp
# lib/dcollide/detectordeform/bvhierarchy/bvhierarchyproxydata.cpp

# lib/dcollide/bvhnode.cpp
# lib/dcollide/bvhnodedefault.cpp
# lib/dcollide/meshsplitter.cpp

# lib/dcollide/boundingvolumes/aabb.cpp
# lib/dcollide/boundingvolumes/obb.cpp
# lib/dcollide/boundingvolumes/boundingvolume.cpp
# lib/dcollide/boundingvolumes/boundingsphere.cpp
# lib/dcollide/boundingvolumes/kdop.cpp

# lib/dcollide/shapes/box.cpp
# lib/dcollide/shapes/cone.cpp
# lib/dcollide/shapes/cylinder.cpp
# lib/dcollide/shapes/mesh.cpp
# lib/dcollide/shapes/shape.cpp
# lib/dcollide/shapes/sphere.cpp
# lib/dcollide/shapes/wedge.cpp
# lib/dcollide/shapes/mesh/triangle.cpp
# lib/dcollide/shapes/mesh/vertex.cpp
# lib/dcollide/shapes/mesh/line.cpp
# lib/dcollide/shapes/mesh/meshfactory.cpp

# lib/dcollide/exceptions/exception.cpp

# lib/dcollide/detectorrigid/bvhtraverse.cpp
# lib/dcollide/detectorrigid/middlephaserigidjob.cpp
# lib/dcollide/detectorrigid/middlephaserigidjobcreator.cpp

# lib/dcollide/narrowphase/narrowphase.cpp
# lib/dcollide/narrowphase/triangleintersector.cpp
# lib/dcollide/narrowphase/boxsphereintersector.cpp
# lib/dcollide/narrowphase/boxboxintersector.cpp
# lib/dcollide/narrowphase/wedgeintersector.cpp
# lib/dcollide/narrowphase/intersectionhelpers.cpp
# lib/dcollide/narrowphase/cylinderintersector.cpp
# lib/dcollide/narrowphase/coneintersector.cpp
# lib/dcollide/narrowphase/spheresphereintersector.cpp
# lib/dcollide/narrowphase/meshmeshintersector.cpp
# lib/dcollide/narrowphase/narrowphasejob.cpp
# lib/dcollide/narrowphase/spheretriangleintersector.cpp

# lib/dcollide/collisioncache.cpp
# lib/dcollide/collisionpair.cpp
#	)


include_directories(lib/eigen)
add_subdirectory(lib/rbdl)

# TODO: Remove
include_directories(lib/csgjs-cpp)
list (APPEND csgjs_src
	lib/csgjs-cpp/csgjs.cpp
	)

include_directories(lib/openmesh/src)
list (APPEND openmesh_src
	lib/openmesh/src/OpenMesh/Core/System/omstream.cc
	#lib/openmesh/src/OpenMesh/Core/Templates/newClass.cc
	#lib/openmesh/src/OpenMesh/Core/Templates/newClassT.cc
	lib/openmesh/src/OpenMesh/Core/Utils/SingletonT.cc
	lib/openmesh/src/OpenMesh/Core/Utils/BaseProperty.cc
	lib/openmesh/src/OpenMesh/Core/Utils/RandomNumberGenerator.cc
	lib/openmesh/src/OpenMesh/Core/Utils/Endian.cc
	lib/openmesh/src/OpenMesh/Core/Geometry/NormalConeT.cc
	lib/openmesh/src/OpenMesh/Core/IO/OMFormatT.cc
	lib/openmesh/src/OpenMesh/Core/IO/BinaryHelper.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/BaseReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/PLYReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/OBJReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/STLReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/OMReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/reader/OFFReader.cc
	lib/openmesh/src/OpenMesh/Core/IO/IOManager.cc
	lib/openmesh/src/OpenMesh/Core/IO/OMFormat.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/STLWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/BaseWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/OBJWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/VTKWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/PLYWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/OFFWriter.cc
	lib/openmesh/src/OpenMesh/Core/IO/writer/OMWriter.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/TriMeshT.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/PolyConnectivity.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/TriConnectivity.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/PolyMeshT.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/BaseKernel.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/ArrayKernelT.cc
	lib/openmesh/src/OpenMesh/Core/Mesh/ArrayKernel.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/ModQuadricT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/DecimaterT.cc
	lib/openmesh/src/OpenMesh/Tools/Decimater/Observer.cc
	)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty)
include_directories(lib/bgfx/3rdparty/khronos)

include_directories(lib/lemon)

include_directories(lib/cereal/include)

include_directories(lib/stb)

include_directories(lib/inline_variant_visitor)

include_directories(common)
include_directories(common/scripting)
include_directories(common/drawing)
include_directories(common/drawing/font)
include_directories(common/drawing/shaderc)

list(APPEND bgfx_src
	lib/bgfx/src/amalgamated.cpp
	)

list(APPEND bgfx_utils_src
	common/drawing/font/font_manager.cpp
	common/drawing/font/utf8.cpp
	common/drawing/font/text_metrics.cpp
	common/drawing/font/text_buffer_manager.cpp
	common/drawing/font/cube_atlas.cpp
	)

include_directories(lib/variant)
include_directories(lib/recastnavigation/Detour/Include)
include_directories(lib/recastnavigation/Recast/Include)
include_directories(lib/recastnavigation/RecastDemo/Include)
include_directories(lib/recastnavigation/DebugUtils/Include)
include_directories(lib/recastnavigation/DetourTileCache/Include)
include_directories(lib/recastnavigation/DetourCrowd/Include)

list (APPEND recast_src
	lib/recastnavigation/Recast/Source/RecastAlloc.cpp
	lib/recastnavigation/Recast/Source/RecastLayers.cpp
	lib/recastnavigation/Recast/Source/RecastContour.cpp
	lib/recastnavigation/Recast/Source/RecastArea.cpp
	lib/recastnavigation/Recast/Source/RecastMeshDetail.cpp
	lib/recastnavigation/Recast/Source/RecastRasterization.cpp
	lib/recastnavigation/Recast/Source/RecastMesh.cpp
	lib/recastnavigation/Recast/Source/RecastFilter.cpp
	lib/recastnavigation/Recast/Source/Recast.cpp
	lib/recastnavigation/Recast/Source/RecastRegion.cpp
	lib/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp
	lib/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp
	#lib/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp
	#lib/recastnavigation/DebugUtils/Source/RecastDebugDraw.cpp
	#lib/recastnavigation/DebugUtils/Source/RecastDump.cpp
	#lib/recastnavigation/DebugUtils/Source/DebugDraw.cpp
	#lib/recastnavigation/RecastDemo/Source/CrowdTool.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_TempObstacles.cpp
	#lib/recastnavigation/RecastDemo/Source/main.cpp
	#lib/recastnavigation/RecastDemo/Source/imguiRenderGL.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample.cpp
	#lib/recastnavigation/RecastDemo/Source/Filelist.cpp
	#lib/recastnavigation/RecastDemo/Source/ConvexVolumeTool.cpp
	#lib/recastnavigation/RecastDemo/Source/imgui.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_Debug.cpp
	#lib/recastnavigation/RecastDemo/Source/SlideShow.cpp
	#lib/recastnavigation/RecastDemo/Source/NavMeshPruneTool.cpp
	#lib/recastnavigation/RecastDemo/Source/PerfTimer.cpp
	#lib/recastnavigation/RecastDemo/Source/InputGeom.cpp
	#lib/recastnavigation/RecastDemo/Source/NavMeshTesterTool.cpp
	#lib/recastnavigation/RecastDemo/Source/MeshLoaderObj.cpp
	#lib/recastnavigation/RecastDemo/Source/ChunkyTriMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_SoloMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/Sample_TileMesh.cpp
	#lib/recastnavigation/RecastDemo/Source/SampleInterfaces.cpp
	#lib/recastnavigation/RecastDemo/Source/OffMeshConnectionTool.cpp
	#lib/recastnavigation/RecastDemo/Source/TestCase.cpp
	#lib/recastnavigation/RecastDemo/Source/ValueHistory.cpp
	#lib/recastnavigation/RecastDemo/Contrib/fastlz/fastlz.c
	lib/recastnavigation/Detour/Source/DetourCommon.cpp
	lib/recastnavigation/Detour/Source/DetourNavMesh.cpp
	lib/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp
	lib/recastnavigation/Detour/Source/DetourAlloc.cpp
	lib/recastnavigation/Detour/Source/DetourNode.cpp
	lib/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourPathQueue.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourObstacleAvoidance.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourProximityGrid.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourCrowd.cpp
	lib/recastnavigation/DetourCrowd/Source/DetourLocalBoundary.cpp
	)

include_directories(lib/angelscript-sdk/angelscript/include)
include_directories(lib/angelscript-sdk/add_on/scriptbuilder)
include_directories(lib/angelscript-sdk/add_on/scriptstdstring)
include_directories(lib/angelscript-sdk/add_on/scripthelper)

list(APPEND angelscript_src
	lib/angelscript-sdk/angelscript/source/as_atomic.cpp
	lib/angelscript-sdk/angelscript/source/as_builder.cpp
	lib/angelscript-sdk/angelscript/source/as_bytecode.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_arm.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_mips.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_ppc_64.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_sh4.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_gcc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_mingw.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x64_msvc.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_x86.cpp
	lib/angelscript-sdk/angelscript/source/as_callfunc_xenon.cpp
	lib/angelscript-sdk/angelscript/source/as_compiler.cpp
	lib/angelscript-sdk/angelscript/source/as_configgroup.cpp
	lib/angelscript-sdk/angelscript/source/as_context.cpp
	lib/angelscript-sdk/angelscript/source/as_datatype.cpp
	lib/angelscript-sdk/angelscript/source/as_gc.cpp
	lib/angelscript-sdk/angelscript/source/as_generic.cpp
	lib/angelscript-sdk/angelscript/source/as_globalproperty.cpp
	lib/angelscript-sdk/angelscript/source/as_memory.cpp
	lib/angelscript-sdk/angelscript/source/as_module.cpp
	lib/angelscript-sdk/angelscript/source/as_objecttype.cpp
	lib/angelscript-sdk/angelscript/source/as_outputbuffer.cpp
	lib/angelscript-sdk/angelscript/source/as_parser.cpp
	lib/angelscript-sdk/angelscript/source/as_restore.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptcode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptengine.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptfunction.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptnode.cpp
	lib/angelscript-sdk/angelscript/source/as_scriptobject.cpp
	lib/angelscript-sdk/angelscript/source/as_string.cpp
	lib/angelscript-sdk/angelscript/source/as_string_util.cpp
	lib/angelscript-sdk/angelscript/source/as_thread.cpp
	lib/angelscript-sdk/angelscript/source/as_tokenizer.cpp
	lib/angelscript-sdk/angelscript/source/as_typeinfo.cpp
	lib/angelscript-sdk/angelscript/source/as_variablescope.cpp
	lib/angelscript-sdk/add_on/scriptany/scriptany.cpp
	lib/angelscript-sdk/add_on/scriptdictionary/scriptdictionary.cpp
	lib/angelscript-sdk/add_on/scriptmath/scriptmathcomplex.cpp
	lib/angelscript-sdk/add_on/scriptmath/scriptmath.cpp
	lib/angelscript-sdk/add_on/debugger/debugger.cpp
	lib/angelscript-sdk/add_on/contextmgr/contextmgr.cpp
	#lib/angelscript-sdk/add_on/autowrapper/generator/generateheader.cpp
	lib/angelscript-sdk/add_on/serializer/serializer.cpp
	lib/angelscript-sdk/add_on/weakref/weakref.cpp
	lib/angelscript-sdk/add_on/scriptbuilder/scriptbuilder.cpp
	lib/angelscript-sdk/add_on/scriptarray/scriptarray.cpp
	lib/angelscript-sdk/add_on/scriptgrid/scriptgrid.cpp
	#lib/angelscript-sdk/add_on/scriptfile/scriptfilesystem.cpp
	#lib/angelscript-sdk/add_on/scriptfile/scriptfile.cpp
	lib/angelscript-sdk/add_on/scripthelper/scripthelper.cpp
	lib/angelscript-sdk/add_on/scripthandle/scripthandle.cpp
	lib/angelscript-sdk/add_on/scriptstdstring/scriptstdstring.cpp
	lib/angelscript-sdk/add_on/scriptstdstring/scriptstdstring_utils.cpp
	)

#include_directories(engine/common)

list(APPEND common_src
	common/Globals.cpp
	common/BasicMesh.cpp
	common/Graphics.cpp
	common/Logger.cpp
	common/Application.cpp
	common/MathFuncs.cpp
	common/NoobUtils.cpp
	common/EditorUtils.cpp
	common/VoxelWorld.cpp
	common/VoxelWorldStable.cpp
	common/ActiveMesh.cpp
	common/UIFont.cpp
	common/GUI.cpp
	common/Camera.cpp
	common/Stage.cpp
	common/Body.cpp
	common/Shape.cpp
	common/MeshUtils.cpp
	common/BasicModel.cpp
	common/AnimatedModel.cpp
	common/SkeletalAnim.cpp
	common/ModelLoader.cpp
	common/drawing/TriplanarGradientMapLit.cpp
	common/drawing/TriplanarGradientMap.cpp
	common/drawing/BasicRenderer.cpp
	common/SimpleNavMesh.cpp
	common/FileSystem.cpp
	common/CSG.cpp
	common/GeometricWorld.cpp
	common/scripting/RegisterMath.cpp
	common/scripting/RegisterBasicMesh.cpp
	common/scripting/RegisterActiveMesh.cpp
	common/scripting/RegisterGlobals.cpp
	common/scripting/RegisterStage.cpp
	common/scripting/RegisterLight.cpp
	common/scripting/RegisterReflectance.cpp
	common/scripting/RegisterShaders.cpp
	common/scripting/RegisterVoxels.cpp
	common/scripting/RegisterScaledModel.cpp
	common/scripting/RegisterHandles.cpp
	)

list(APPEND user_src
	../sandbox/UserApp.cpp
	)

include_directories(platform/nix)

list(APPEND main_src platform/nix/EngineDesktop.cpp platform/nix/NDOF.cpp)

add_executable(NoobEdit ${user_src} ${main_src} ${common_src} ${bgfx_src} ${bgfx_utils_src} ${vhacd_src} ${openmesh_src} ${cppformat_src}  ${recast_src} ${angelscript_src} ${bullet_src} ${voropp_src}) # ${dcollide_src}

target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} freetype rt dl pthread assimp m spnav ozz_animation_offline_r ozz_animation_r ozz_geometry_r ozz_base_r ${Boost_LIBRARIES})

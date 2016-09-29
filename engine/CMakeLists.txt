cmake_minimum_required(VERSION 3.5)
project(NoobWerkz)

# set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_BUILD_TYPE "RELEASE")
set(BUILD_SHARED_LIBS false)

set (bgfx_macros "-D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS  -DBGFX_CONFIG_MULTITHREADED=0")

set(CMAKE_CXX_FLAGS "-std=c++1y  -fno-exceptions -fno-rtti -DEIGEN_MPL2_ONLY ${bgfx_macros} ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-DSTB_IMAGE_STATIC -DFT2_BUILD_LIBRARY ${CMAKE_C_FLAGS}")
# This is to ensure that platform/android/jni/android_fopen.h gets to be among the top includes
if (NOOB_PLATFORM_ANDROID)
endif()



include_directories(lib/freetype/include)
list (APPEND freetype_src
#./lib/freetype/builds/wince/ftdebug.c
#./lib/freetype/builds/amiga/src/base/ftdebug.c
#./lib/freetype/builds/amiga/src/base/ftsystem.c
#./lib/freetype/builds/mac/ftmac.c
#./lib/freetype/builds/windows/ftdebug.c
#./lib/freetype/builds/vms/ftsystem.c
#./lib/freetype/builds/unix/ftsystem.c

#./lib/freetype/src/psaux/psaux.c
./lib/freetype/src/psaux/psconv.c
./lib/freetype/src/psaux/afmparse.c
./lib/freetype/src/psaux/t1decode.c
./lib/freetype/src/psaux/psauxmod.c
./lib/freetype/src/psaux/t1cmap.c
./lib/freetype/src/psaux/psobjs.c
./lib/freetype/src/autofit/afblue.c
./lib/freetype/src/autofit/afshaper.c
./lib/freetype/src/autofit/afmodule.c
./lib/freetype/src/autofit/afpic.c
./lib/freetype/src/autofit/afcjk.c
./lib/freetype/src/autofit/afindic.c
./lib/freetype/src/autofit/aflatin.c
./lib/freetype/src/autofit/afglobal.c
./lib/freetype/src/autofit/afdummy.c
./lib/freetype/src/autofit/afloader.c
./lib/freetype/src/autofit/afranges.c
./lib/freetype/src/autofit/afangles.c
./lib/freetype/src/autofit/afwarp.c
#./lib/freetype/src/autofit/autofit.c

#./lib/freetype/src/autofit/aflatin2.c

./lib/freetype/src/autofit/afhints.c
./lib/freetype/src/sfnt/ttkern.c
./lib/freetype/src/sfnt/ttpost.c
./lib/freetype/src/sfnt/sfobjs.c
./lib/freetype/src/sfnt/pngshim.c
./lib/freetype/src/sfnt/ttload.c
./lib/freetype/src/sfnt/ttmtx.c
./lib/freetype/src/sfnt/ttsbit.c
#./lib/freetype/src/sfnt/sfnt.c
./lib/freetype/src/sfnt/ttcmap.c
./lib/freetype/src/sfnt/ttbdf.c
./lib/freetype/src/sfnt/sfdriver.c
./lib/freetype/src/sfnt/sfntpic.c
./lib/freetype/src/pfr/pfrload.c
./lib/freetype/src/pfr/pfrcmap.c
./lib/freetype/src/pfr/pfrsbit.c
./lib/freetype/src/pfr/pfrdrivr.c
#./lib/freetype/src/pfr/pfr.c
./lib/freetype/src/pfr/pfrobjs.c
./lib/freetype/src/pfr/pfrgload.c
./lib/freetype/src/bdf/bdfdrivr.c
./lib/freetype/src/bdf/bdflib.c
#./lib/freetype/src/bdf/bdf.c
#./lib/freetype/src/psnames/psnames.c
./lib/freetype/src/psnames/psmodule.c
./lib/freetype/src/psnames/pspic.c
./lib/freetype/src/raster/ftrend1.c
./lib/freetype/src/raster/rastpic.c
./lib/freetype/src/raster/ftraster.c
#./lib/freetype/src/raster/raster.c
#./lib/freetype/src/pcf/pcf.c
./lib/freetype/src/pcf/pcfutil.c
./lib/freetype/src/pcf/pcfdrivr.c
./lib/freetype/src/pcf/pcfread.c
#./lib/freetype/src/otvalid/otvalid.c
./lib/freetype/src/otvalid/otvcommn.c
./lib/freetype/src/otvalid/otvjstf.c
./lib/freetype/src/otvalid/otvgsub.c
./lib/freetype/src/otvalid/otvmod.c
./lib/freetype/src/otvalid/otvbase.c
./lib/freetype/src/otvalid/otvgdef.c
./lib/freetype/src/otvalid/otvgpos.c
./lib/freetype/src/otvalid/otvmath.c
./lib/freetype/src/type42/t42objs.c
./lib/freetype/src/type42/t42parse.c
#./lib/freetype/src/type42/type42.c
./lib/freetype/src/type42/t42drivr.c
./lib/freetype/src/lzw/ftzopen.c
#./lib/freetype/src/lzw/ftlzw.c
./lib/freetype/src/smooth/ftspic.c
./lib/freetype/src/smooth/ftsmooth.c
./lib/freetype/src/smooth/ftgrays.c
#./lib/freetype/src/smooth/smooth.c
#./lib/freetype/src/cache/ftcache.c
./lib/freetype/src/cache/ftcmru.c
./lib/freetype/src/cache/ftcglyph.c
./lib/freetype/src/cache/ftccache.c
./lib/freetype/src/cache/ftcimage.c
./lib/freetype/src/cache/ftcbasic.c
./lib/freetype/src/cache/ftcmanag.c
./lib/freetype/src/cache/ftcsbits.c
./lib/freetype/src/cache/ftccmap.c
./lib/freetype/src/pshinter/pshalgo.c
#./lib/freetype/src/pshinter/pshinter.c
./lib/freetype/src/pshinter/pshrec.c
./lib/freetype/src/pshinter/pshglob.c
./lib/freetype/src/pshinter/pshpic.c
./lib/freetype/src/pshinter/pshmod.c
./lib/freetype/src/type1/t1driver.c
./lib/freetype/src/type1/t1objs.c
#./lib/freetype/src/type1/type1.c
./lib/freetype/src/type1/t1afm.c
./lib/freetype/src/type1/t1gload.c
./lib/freetype/src/type1/t1load.c
./lib/freetype/src/type1/t1parse.c
./lib/freetype/src/bzip2/ftbzip2.c
./lib/freetype/src/winfonts/winfnt.c
./lib/freetype/src/base/ftsnames.c
./lib/freetype/src/base/ftotval.c
./lib/freetype/src/base/fttype1.c
./lib/freetype/src/base/ftapi.c
./lib/freetype/src/base/ftoutln.c
./lib/freetype/src/base/ftinit.c
./lib/freetype/src/base/ftgasp.c
./lib/freetype/src/base/ftpic.c
./lib/freetype/src/base/ftfntfmt.c

#./lib/freetype/src/base/ftmac.c

./lib/freetype/src/base/ftfstype.c
./lib/freetype/src/base/ftobjs.c
./lib/freetype/src/base/md5.c
./lib/freetype/src/base/ftstream.c
./lib/freetype/src/base/ftpfr.c
./lib/freetype/src/base/ftwinfnt.c
./lib/freetype/src/base/ftcid.c
./lib/freetype/src/base/ftadvanc.c
./lib/freetype/src/base/ftsynth.c
./lib/freetype/src/base/ftdebug.c
./lib/freetype/src/base/ftgxval.c
./lib/freetype/src/base/ftbitmap.c
./lib/freetype/src/base/ftcalc.c
./lib/freetype/src/base/ftpatent.c
./lib/freetype/src/base/ftglyph.c
./lib/freetype/src/base/ftgloadr.c
./lib/freetype/src/base/ftmm.c
./lib/freetype/src/base/ftbdf.c
./lib/freetype/src/base/ftstroke.c
#./lib/freetype/src/base/ftbase.c
./lib/freetype/src/base/ftrfork.c
./lib/freetype/src/base/ftutil.c
./lib/freetype/src/base/ftsystem.c
./lib/freetype/src/base/fthash.c
./lib/freetype/src/base/basepic.c
./lib/freetype/src/base/ftlcdfil.c
./lib/freetype/src/base/ftbbox.c
./lib/freetype/src/base/ftdbgmem.c
./lib/freetype/src/base/fttrigon.c

./lib/freetype/src/gzip/inflate.c

#./lib/freetype/src/gzip/infutil.c
#./lib/freetype/src/gzip/zutil.c

./lib/freetype/src/gzip/adler32.c
./lib/freetype/src/gzip/infcodes.c
./lib/freetype/src/gzip/infblock.c
./lib/freetype/src/gzip/ftgzip.c
./lib/freetype/src/gzip/inftrees.c

#./lib/freetype/src/tools/test_bbox.c
#./lib/freetype/src/tools/ftfuzzer/rasterfuzzer.cc
#./lib/freetype/src/tools/ftfuzzer/runinput.cc
#./lib/freetype/src/tools/ftfuzzer/ftmutator.cc
#./lib/freetype/src/tools/ftfuzzer/ftfuzzer.cc
#./lib/freetype/src/tools/test_trig.c
#./lib/freetype/src/tools/apinames.c
#./lib/freetype/src/tools/test_afm.c
#./lib/freetype/src/tools/ftrandom/ftrandom.c

./lib/freetype/src/lzw/ftlzw.c
#./lib/freetype/src/lzw/ftzopen.c

./lib/freetype/src/truetype/ttdriver.c
./lib/freetype/src/truetype/ttgload.c
./lib/freetype/src/truetype/ttobjs.c
./lib/freetype/src/truetype/ttpload.c
./lib/freetype/src/truetype/ttinterp.c
./lib/freetype/src/truetype/ttpic.c
./lib/freetype/src/truetype/ttgxvar.c
#./lib/freetype/src/truetype/truetype.c
./lib/freetype/src/truetype/ttsubpix.c
./lib/freetype/src/cff/cf2font.c
./lib/freetype/src/cff/cffload.c
./lib/freetype/src/cff/cffgload.c
./lib/freetype/src/cff/cf2hints.c
./lib/freetype/src/cff/cf2error.c
./lib/freetype/src/cff/cf2read.c
./lib/freetype/src/cff/cf2arrst.c
./lib/freetype/src/cff/cf2ft.c
./lib/freetype/src/cff/cffdrivr.c
./lib/freetype/src/cff/cf2intrp.c
./lib/freetype/src/cff/cf2blues.c
./lib/freetype/src/cff/cffparse.c
./lib/freetype/src/cff/cffcmap.c
#./lib/freetype/src/cff/cff.c
./lib/freetype/src/cff/cf2stack.c
./lib/freetype/src/cff/cffobjs.c
./lib/freetype/src/cff/cffpic.c
#./lib/freetype/src/cid/type1cid.c
./lib/freetype/src/cid/cidgload.c
./lib/freetype/src/cid/cidload.c
./lib/freetype/src/cid/cidobjs.c
./lib/freetype/src/cid/cidriver.c
./lib/freetype/src/cid/cidparse.c
)

include_directories(lib/glm)
include_directories(lib/eigen)

include_directories(lib/base-utils/include)
include_directories(lib/base-utils/rde)
list (APPEND base_src
	lib/base-utils/rde/rdestl/allocator.cpp
	lib/base-utils/rde/rdestl/intrusive_list.cpp
	lib/base-utils/rde/rdestl/intrusive_slist.cpp
	lib/base-utils/rde/rdestl/list.cpp
	lib/base-utils/include/noob/strings/modp_numtoa.c
	lib/base-utils/include/noob/fast_hashtable/fast_hashtable.cpp
	)

include_directories(lib/bx/include)
include_directories(lib/bgfx/include)
include_directories(lib/bgfx/3rdparty/sdf)
include_directories(lib/bgfx/3rdparty/khronos)
include_directories(lib/bgfx/3rdparty)

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

include_directories(lib/voropp/src)
list (APPEND voropp_src
	lib/voropp/src/voro++.cc
	)

include_directories(lib/ozz-animation/include)
include_directories(lib/ozz-animation/include/ozz)
include_directories(lib/ozz-animation/src)

list(APPEND ozz_src
	lib/ozz-animation/src/animation/runtime/local_to_model_job.cc
	lib/ozz-animation/src/animation/runtime/skeleton_utils.cc
	lib/ozz-animation/src/animation/runtime/sampling_job.cc
	lib/ozz-animation/src/animation/runtime/animation.cc
	lib/ozz-animation/src/animation/runtime/blending_job.cc
	lib/ozz-animation/src/animation/runtime/skeleton.cc
	# lib/ozz-animation/src/animation/offline/collada/dae2skel.cc
	# lib/ozz-animation/src/animation/offline/collada/collada_transform.cc
	# lib/ozz-animation/src/animation/offline/collada/collada_base.cc
	# lib/ozz-animation/src/animation/offline/collada/collada_skeleton.cc
	# lib/ozz-animation/src/animation/offline/collada/dae2anim.cc
	# lib/ozz-animation/src/animation/offline/collada/collada_animation.cc
	# lib/ozz-animation/src/animation/offline/collada/collada.cc
	lib/ozz-animation/src/animation/offline/animation_optimizer.cc
	lib/ozz-animation/src/animation/offline/skeleton_builder.cc
	lib/ozz-animation/src/animation/offline/additive_animation_builder.cc
	lib/ozz-animation/src/animation/offline/raw_skeleton.cc
	lib/ozz-animation/src/animation/offline/animation_builder.cc
	lib/ozz-animation/src/animation/offline/raw_animation.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx_base.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx2anim.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx_skeleton.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx_animation.cc
	# lib/ozz-animation/src/animation/offline/fbx/fbx2skel.cc
	lib/ozz-animation/src/animation/offline/raw_animation_archive.cc
	lib/ozz-animation/src/animation/offline/raw_animation_utils.cc
	lib/ozz-animation/src/animation/offline/raw_skeleton_archive.cc
	# lib/ozz-animation/src/animation/offline/tools/convert2anim.cc
	# lib/ozz-animation/src/animation/offline/tools/convert2skel.cc
	lib/ozz-animation/src/base/io/archive.cc
	lib/ozz-animation/src/base/io/stream.cc
	lib/ozz-animation/src/base/memory/allocator.cc
	lib/ozz-animation/src/base/log.cc
	lib/ozz-animation/src/base/containers/string_archive.cc
	lib/ozz-animation/src/base/maths/box.cc
	lib/ozz-animation/src/base/maths/soa_math_archive.cc
	lib/ozz-animation/src/base/maths/simd_math_archive.cc
	lib/ozz-animation/src/base/maths/math_archive.cc
	lib/ozz-animation/src/base/maths/simd_math.cc
	lib/ozz-animation/src/geometry/runtime/skinning_job.cc
	)	

list (APPEND assimp_src)

if (DEFINED NOOB_USE_ASSIMP)

	set(ASSIMP_FLAGS " -DNOOB_USE_ASSIMP -DASSIMP_BUILD_BOOST_WORKAROUND -DASSIMP_BUILD_NO_EXPORT -DASSIMP_BUILD_NO_XGL_IMPORTER -DASSIMP_BUILD_NO_FBX_IMPORTER -DASSIMP_BUILD_NO_C4D_IMPORTER -DASSIMP_BUILD_NO_X_IMPORTER -DASSIMP_BUILD_NO_OBJ_IMPORTER -DASSIMP_BUILD_NO_MD3_IMPORTER -DASSIMP_BUILD_NO_MD2_IMPORTER -DASSIMP_BUILD_NO_MDL_IMPORTER -DASSIMP_BUILD_NO_ASE_IMPORTER -DASSIMP_BUILD_NO_HMP_IMPORTER -DASSIMP_BUILD_NO_SMD_IMPORTER -DASSIMP_BUILD_NO_MDC_IMPORTER -DASSIMP_BUILD_NO_MD5_IMPORTER -DASSIMP_BUILD_NO_LWO_IMPORTER -DASSIMP_BUILD_NO_DXF_IMPORTER -DASSIMP_BUILD_NO_NFF_IMPORTER -DASSIMP_BUILD_NO_RAW_IMPORTER -DASSIMP_BUILD_NO_AC_IMPORTER -DASSIMP_BUILD_NO_BVH_IMPORTER -DASSIMP_BUILD_NO_IRRMESH_IMPORTER -DASSIMP_BUILD_NO_IRR_IMPORTER -DASSIMP_BUILD_NO_Q3D_IMPORTER -DASSIMP_BUILD_NO_B3D_IMPORTER -DASSIMP_BUILD_NO_COLLADA_IMPORTER -DASSIMP_BUILD_NO_TERRAGEN_IMPORTER -DASSIMP_BUILD_NO_CSM_IMPORTER -DASSIMP_BUILD_NO_3D_IMPORTER -DASSIMP_BUILD_NO_LWS_IMPORTER -DASSIMP_BUILD_NO_OGRE_IMPORTER -DASSIMP_BUILD_NO_OPENGEX_IMPORTER -DASSIMP_BUILD_NO_COB_IMPORTER -DASSIMP_BUILD_NO_BLEND_IMPORTER -DASSIMP_BUILD_NO_Q3BSP_IMPORTER -DASSIMP_BUILD_NO_NDO_IMPORTER -DASSIMP_BUILD_NO_IFC_IMPORTER")

	set(CMAKE_CXX_FLAGS "-DNOOB_USE_ASSIMP ${ASSIMP_FLAGS} ${CMAKE_CXX_FLAGS}")

	include_directories(lib/assimp/include)
	include_directories(lib/assimp/code)
	include_directories(lib/assimp/contrib/zlib)
	include_directories(lib/assimp/contrib/openddlparser/include)
	include_directories(lib/assimp/contrib/poly2tri)

	list (APPEND assimp_src
		lib/assimp/contrib/clipper/clipper.cpp
		lib/assimp/contrib/ConvertUTF/ConvertUTF.c
		lib/assimp/contrib/unzip/unzip.c
		lib/assimp/contrib/unzip/ioapi.c
		lib/assimp/contrib/openddlparser/code/OpenDDLParser.cpp
		lib/assimp/contrib/openddlparser/code/Value.cpp
		lib/assimp/contrib/openddlparser/code/DDLNode.cpp
		lib/assimp/contrib/irrXML/irrXML.cpp
		lib/assimp/contrib/zlib/trees.c
		lib/assimp/contrib/zlib/inflate.c
		lib/assimp/contrib/zlib/inffast.c
		lib/assimp/contrib/zlib/gzwrite.c
		lib/assimp/contrib/zlib/deflate.c
		lib/assimp/contrib/zlib/gzlib.c
		lib/assimp/contrib/zlib/crc32.c
		lib/assimp/contrib/zlib/zutil.c
		lib/assimp/contrib/zlib/infback.c
		lib/assimp/contrib/zlib/gzclose.c
		lib/assimp/contrib/zlib/compress.c
		lib/assimp/contrib/zlib/adler32.c
		lib/assimp/contrib/zlib/uncompr.c
		lib/assimp/contrib/zlib/gzread.c
		lib/assimp/contrib/zlib/inftrees.c
		lib/assimp/contrib/poly2tri/poly2tri/sweep/cdt.cc
		lib/assimp/contrib/poly2tri/poly2tri/sweep/sweep.cc
		lib/assimp/contrib/poly2tri/poly2tri/sweep/sweep_context.cc
		lib/assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.cc
		lib/assimp/contrib/poly2tri/poly2tri/common/shapes.cc
		# lib/assimp/port/jassimp/jassimp-native/src/jassimp.cpp
		# lib/assimp/port/AndroidJNI/AndroidJNIIOSystem.cpp
		# lib/assimp/samples/SimpleOpenGL/Sample_SimpleOpenGL.c
		# lib/assimp/samples/SimpleTexturedOpenGL/SimpleTexturedOpenGL/src/model_loading.cpp
		lib/assimp/code/UnrealLoader.cpp
		lib/assimp/code/SkeletonMeshBuilder.cpp
		lib/assimp/code/NFFLoader.cpp
		lib/assimp/code/StandardShapes.cpp
		lib/assimp/code/TriangulateProcess.cpp
		lib/assimp/code/FBXTokenizer.cpp
		lib/assimp/code/HMPLoader.cpp
		lib/assimp/code/3DSConverter.cpp
		lib/assimp/code/FBXProperties.cpp
		lib/assimp/code/MD2Loader.cpp
		lib/assimp/code/FBXConverter.cpp
		lib/assimp/code/BlenderBMesh.cpp
		lib/assimp/code/ProcessHelper.cpp
		lib/assimp/code/DefaultIOStream.cpp
		lib/assimp/code/ACLoader.cpp
		lib/assimp/code/SpatialSort.cpp
		lib/assimp/code/AssimpPCH.cpp
		lib/assimp/code/FBXMaterial.cpp
		lib/assimp/code/SGSpatialSort.cpp
		lib/assimp/code/OptimizeGraph.cpp
		lib/assimp/code/DeboneProcess.cpp
		lib/assimp/code/OFFLoader.cpp
		lib/assimp/code/Exporter.cpp
		lib/assimp/code/BaseProcess.cpp
		lib/assimp/code/ImproveCacheLocality.cpp
		lib/assimp/code/BaseImporter.cpp
		lib/assimp/code/RawLoader.cpp
		lib/assimp/code/AssimpCExport.cpp
		lib/assimp/code/AssbinLoader.cpp
		lib/assimp/code/LWOBLoader.cpp
		lib/assimp/code/XFileImporter.cpp
		lib/assimp/code/ColladaParser.cpp
		lib/assimp/code/LWOMaterial.cpp
		lib/assimp/code/IFCGeometry.cpp
		lib/assimp/code/BlenderModifier.cpp
		lib/assimp/code/IFCMaterial.cpp
		lib/assimp/code/OgreXmlSerializer.cpp
		lib/assimp/code/FBXDocument.cpp
		lib/assimp/code/FBXModel.cpp
		lib/assimp/code/TerragenLoader.cpp
		lib/assimp/code/PlyExporter.cpp
		lib/assimp/code/ObjExporter.cpp
		lib/assimp/code/IFCCurve.cpp
		lib/assimp/code/PlyParser.cpp
		lib/assimp/code/Q3BSPFileParser.cpp
		lib/assimp/code/OgreBinarySerializer.cpp
		lib/assimp/code/PostStepRegistry.cpp
		lib/assimp/code/LWSLoader.cpp
		lib/assimp/code/IRRLoader.cpp
		lib/assimp/code/IRRShared.cpp
		lib/assimp/code/STLExporter.cpp
		lib/assimp/code/MakeVerboseFormat.cpp
		lib/assimp/code/Q3BSPFileImporter.cpp
		lib/assimp/code/ComputeUVMappingProcess.cpp
		lib/assimp/code/RemoveVCProcess.cpp
		lib/assimp/code/OgreImporter.cpp
		lib/assimp/code/XFileExporter.cpp
		lib/assimp/code/FBXImporter.cpp
		lib/assimp/code/SceneCombiner.cpp
		lib/assimp/code/MS3DLoader.cpp
		lib/assimp/code/IFCUtil.cpp
		lib/assimp/code/MDLLoader.cpp
		lib/assimp/code/XGLLoader.cpp
		lib/assimp/code/BlenderLoader.cpp
		lib/assimp/code/IFCBoolean.cpp
		lib/assimp/code/RemoveRedundantMaterials.cpp
		lib/assimp/code/GenVertexNormalsProcess.cpp
		lib/assimp/code/OgreStructs.cpp
		lib/assimp/code/FBXDocumentUtil.cpp
		lib/assimp/code/OgreMaterial.cpp
		lib/assimp/code/ColladaExporter.cpp
		lib/assimp/code/IFCOpenings.cpp
		lib/assimp/code/Assimp.cpp
		lib/assimp/code/MaterialSystem.cpp
		lib/assimp/code/MD5Parser.cpp
		lib/assimp/code/3DSExporter.cpp
		lib/assimp/code/ConvertToLHProcess.cpp
		lib/assimp/code/MDLMaterialLoader.cpp
		lib/assimp/code/ObjFileParser.cpp
		lib/assimp/code/LWOAnimation.cpp
		lib/assimp/code/STEPFileReader.cpp
		lib/assimp/code/ValidateDataStructure.cpp
		lib/assimp/code/Q3BSPZipArchive.cpp
		lib/assimp/code/CSMLoader.cpp
		lib/assimp/code/FBXBinaryTokenizer.cpp
		lib/assimp/code/OptimizeMeshes.cpp
		lib/assimp/code/JoinVerticesProcess.cpp
		lib/assimp/code/SMDLoader.cpp
		lib/assimp/code/Subdivision.cpp
		lib/assimp/code/NDOLoader.cpp
		lib/assimp/code/IFCReaderGen.cpp
		lib/assimp/code/FBXMeshGeometry.cpp
		lib/assimp/code/IRRMeshLoader.cpp
		lib/assimp/code/FBXDeformer.cpp
		lib/assimp/code/ObjFileImporter.cpp
		lib/assimp/code/ASEParser.cpp
		lib/assimp/code/RemoveComments.cpp
		lib/assimp/code/MDCLoader.cpp
		lib/assimp/code/IFCLoader.cpp
		lib/assimp/code/STEPFileEncoding.cpp
		lib/assimp/code/FindInstancesProcess.cpp
		lib/assimp/code/3DSLoader.cpp
		lib/assimp/code/Bitmap.cpp
		lib/assimp/code/BlenderScene.cpp
		lib/assimp/code/FindDegenerates.cpp
		lib/assimp/code/FBXParser.cpp
		lib/assimp/code/LimitBoneWeightsProcess.cpp
		lib/assimp/code/Importer.cpp
		lib/assimp/code/VertexTriangleAdjacency.cpp
		lib/assimp/code/AssxmlExporter.cpp
		lib/assimp/code/COBLoader.cpp
		lib/assimp/code/PlyLoader.cpp
		lib/assimp/code/ColladaLoader.cpp
		lib/assimp/code/LWOLoader.cpp
		lib/assimp/code/TextureTransform.cpp
		lib/assimp/code/SplitLargeMeshes.cpp
		lib/assimp/code/FixNormalsStep.cpp
		lib/assimp/code/ASELoader.cpp
		lib/assimp/code/Q3DLoader.cpp
		lib/assimp/code/BlenderDNA.cpp
		lib/assimp/code/BlenderTessellator.cpp
		lib/assimp/code/OpenGEXImporter.cpp
		lib/assimp/code/STLLoader.cpp
		lib/assimp/code/DefaultIOSystem.cpp
		lib/assimp/code/BVHLoader.cpp
		# lib/assimp/code/C4DImporter.cpp
		lib/assimp/code/GenFaceNormalsProcess.cpp
		lib/assimp/code/AssbinExporter.cpp
		lib/assimp/code/ScenePreprocessor.cpp
		lib/assimp/code/DXFLoader.cpp
		lib/assimp/code/SplitByBoneCountProcess.cpp
		lib/assimp/code/MD5Loader.cpp
		lib/assimp/code/ObjFileMtlImporter.cpp
		lib/assimp/code/ImporterRegistry.cpp
		lib/assimp/code/FBXUtil.cpp
		lib/assimp/code/DefaultLogger.cpp
		lib/assimp/code/PretransformVertices.cpp
		lib/assimp/code/FBXAnimation.cpp
		lib/assimp/code/TargetAnimation.cpp
		lib/assimp/code/XFileParser.cpp
		lib/assimp/code/FindInvalidDataProcess.cpp
		lib/assimp/code/MD3Loader.cpp
		lib/assimp/code/B3DImporter.cpp
		lib/assimp/code/CalcTangentsProcess.cpp
		lib/assimp/code/IFCProfile.cpp
		lib/assimp/code/SortByPTypeProcess.cpp
		lib/assimp/code/FBXNodeAttribute.cpp
		)
endif()


# Used to link to the system lib, but then had problems. Now following Couman's advice in http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=7168
include_directories(lib/bullet/src)
include_directories(lib/bullet/BulletCollision)
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
	# lib/bullet/src/MiniCL/MiniCL.cpp
	# lib/bullet/src/MiniCL/MiniCLTask/MiniCLTask.cpp
	# lib/bullet/src/MiniCL/MiniCLTaskScheduler.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/btSoftBodySolver_DX11SIMDAware.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/btSoftBodySolver_DX11.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolver_OpenCL.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolverOutputCLtoGL.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolver_OpenCLSIMDAware.cpp
	# lib/bullet/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/MiniCLTaskWrap.cpp
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

include_directories(lib/stb)

include_directories(common)
include_directories(common/drawing)
include_directories(common/drawing/font)

include_directories(lib/libogg/include)
list (APPEND ogg_src
	lib/libogg/src/framing.c
	lib/libogg/src/bitwise.c
	)

include_directories(lib/libvorbis/include)
include_directories(lib/libvorbis/lib)

include_directories(lib/libvorbis/lib/modes)
include_directories(lib/libvorbis/lib/books/coupled)
include_directories(lib/libvorbis/lib/books/uncoupled)
include_directories(lib/libvorbis/lib/books/floor)

list (APPEND vorbis_src
	lib/libvorbis/lib/codebook.c
	lib/libvorbis/lib/psytune.c
	lib/libvorbis/lib/block.c
	lib/libvorbis/lib/bitrate.c
	lib/libvorbis/lib/lookup.c
	lib/libvorbis/lib/envelope.c
	lib/libvorbis/lib/lsp.c
	lib/libvorbis/lib/vorbisenc.c
	lib/libvorbis/lib/floor0.c
	lib/libvorbis/lib/psy.c
	lib/libvorbis/lib/mapping0.c
	# lib/libvorbis/lib/tone.c
	lib/libvorbis/lib/vorbisfile.c
	lib/libvorbis/lib/smallft.c
	lib/libvorbis/lib/window.c
	lib/libvorbis/lib/floor1.c
	lib/libvorbis/lib/sharedbook.c
	# lib/libvorbis/lib/barkmel.c
	lib/libvorbis/lib/mdct.c
	lib/libvorbis/lib/lpc.c
	lib/libvorbis/lib/analysis.c
	lib/libvorbis/lib/info.c
	lib/libvorbis/lib/res0.c
	lib/libvorbis/lib/synthesis.c
	lib/libvorbis/lib/registry.c
	# lib/libvorbis/vq/latticetune.c
	# lib/libvorbis/vq/huffbuild.c
	# lib/libvorbis/vq/distribution.c
	# lib/libvorbis/vq/metrics.c
	# lib/libvorbis/vq/latticebuild.c
	# lib/libvorbis/vq/vqgen.c
	# lib/libvorbis/vq/bookutil.c
	)

include_directories(lib/r8brain-free-src)

list(APPEND r8_src
	lib/r8brain-free-src/r8bbase.cpp
	)

list (APPEND recast_src)

if (DEFINED NOOB_USE_RECAST)

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
		# lib/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp
		# lib/recastnavigation/DebugUtils/Source/RecastDebugDraw.cpp
		# lib/recastnavigation/DebugUtils/Source/RecastDump.cpp
		# lib/recastnavigation/DebugUtils/Source/DebugDraw.cpp
		# lib/recastnavigation/RecastDemo/Source/CrowdTool.cpp
		# lib/recastnavigation/RecastDemo/Source/Sample_TempObstacles.cpp
		# lib/recastnavigation/RecastDemo/Source/main.cpp
		# lib/recastnavigation/RecastDemo/Source/imguiRenderGL.cpp
		# lib/recastnavigation/RecastDemo/Source/Sample.cpp
		# lib/recastnavigation/RecastDemo/Source/Filelist.cpp
		# lib/recastnavigation/RecastDemo/Source/ConvexVolumeTool.cpp
		# lib/recastnavigation/RecastDemo/Source/imgui.cpp
		# lib/recastnavigation/RecastDemo/Source/Sample_Debug.cpp
		# lib/recastnavigation/RecastDemo/Source/SlideShow.cpp
		# lib/recastnavigation/RecastDemo/Source/NavMeshPruneTool.cpp
		# lib/recastnavigation/RecastDemo/Source/PerfTimer.cpp
		# lib/recastnavigation/RecastDemo/Source/InputGeom.cpp
		# lib/recastnavigation/RecastDemo/Source/NavMeshTesterTool.cpp
		# lib/recastnavigation/RecastDemo/Source/MeshLoaderObj.cpp
		# lib/recastnavigation/RecastDemo/Source/ChunkyTriMesh.cpp
		# lib/recastnavigation/RecastDemo/Source/Sample_SoloMesh.cpp
		# lib/recastnavigation/RecastDemo/Source/Sample_TileMesh.cpp
		# lib/recastnavigation/RecastDemo/Source/SampleInterfaces.cpp
		# lib/recastnavigation/RecastDemo/Source/OffMeshConnectionTool.cpp
		# lib/recastnavigation/RecastDemo/Source/TestCase.cpp
		# lib/recastnavigation/RecastDemo/Source/ValueHistory.cpp
		# lib/recastnavigation/RecastDemo/Contrib/fastlz/fastlz.c
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
endif()

include_directories(lib/short-alloc)
include_directories(lib/network-host)
include_directories(lib/math-funcs)

list (APPEND shiny_src )

if (DEFINED NOOB_USE_SHINY)
	SET(CMAKE_CXX_FLAGS "-DSHINY_IS_COMPILED=TRUE  ${CMAKE_CXX_FLAGS}")

	include_directories(lib/Shiny/include)

	list(APPEND shiny_src
		lib/Shiny/src/ShinyManager.cpp
		lib/Shiny/src/ShinyNode.cpp
		lib/Shiny/src/ShinyNodePool.cpp
		lib/Shiny/src/ShinyOutput.cpp
		lib/Shiny/src/ShinyTools.cpp
		)
endif()

list(APPEND common_src
	common/Globals.cpp
	common/BasicMesh.cpp
	common/Graphics.cpp
	common/Application.cpp
	common/NoobUtils.cpp
	common/Camera.cpp
	common/Stage.cpp
	common/Body.cpp
	common/Joint.cpp
	common/Ghost.cpp
	common/Shape.cpp
	common/MeshUtils.cpp
	common/BasicModel.cpp
	common/AnimatedModel.cpp
	common/SkeletalAnim.cpp
	common/Particles.cpp
	common/UIFont.cpp
	common/GUI.cpp
	common/drawing/TriplanarGradientMap.cpp
	common/drawing/BasicRenderer.cpp
	# common/NavMesh.cpp
	common/FileSystem.cpp
	common/Mixer.cpp
	common/AudioSample.cpp
	)

list(APPEND user_src
	../sandbox/UserApp.cpp
	)

# Platform-specific:
if (NOOB_PLATFORM_LINUX)
	add_subdirectory(lib/glfw)
	include_directories(lib/glfw/include)	
	include_directories(platform/nix)
	list(APPEND main_src platform/nix/EngineDesktop.cpp
		platform/desktop/SoundInterface.cpp)
endif()



# All platforms get these:
list (APPEND app_src ${assimp_src} ${freetype_src} ${user_src} ${main_src} ${common_src} ${bgfx_src} ${bgfx_utils_src} ${base_src} ${recast_src} ${bullet_src} ${voropp_src} ${ozz_src} ${ogg_src} ${vorbis_src} ${r8_src} ${shiny_src})

# More platform-specific:
if (NOOB_PLATFORM_LINUX)
	add_executable(NoobEdit ${app_src})
	target_link_libraries(NoobEdit LINK_PUBLIC glfw ${GLFW_LIBRARIES} GL rt dl pthread m soundio.a asound ${OPENGL_LIBRARIES})	
	# target_link_libraries(NoobEdit LINK_PUBLIC spnav)
endif()


if (NOOB_PLATFORM_ANDROID)
	set (CMAKE_C_FLAGS "-DNOOB_PLATFORM_ANDROID -DBGFX_CONFIG_RENDERER_OPENGLES=20 -include '${CMAKE_SOURCE_DIR}/platform/android/jni/android_fopen.h'  ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-DNOOB_PLATFORM_ANDROID -DBGFX_CONFIG_RENDERER_OPENGLES=20 -include '${CMAKE_SOURCE_DIR}/platform/android/jni/android_fopen.h' ${CMAKE_CXX_FLAGS}")
	include_directories($ENV{ANDROID_NDK_HOME}/sources/android/native_app_glue) 

	include_directories(platform/android/jni)
	list (APPEND app_src
		$ENV{ANDROID_NDK_HOME}/sources/android/native_app_glue/android_native_app_glue.c
		platform/droid/SoundInterface.cpp
		platform/android/jni/engine_droid.cpp
		platform/android/jni/android_fopen.c
		)

	add_library(Entry SHARED ${app_src})
	target_link_libraries(Entry log android GLESv2 EGL dl m)
endif()


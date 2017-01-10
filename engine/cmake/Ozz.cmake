include_directories(lib/ozz-animation/include)
include_directories(lib/ozz-animation/include/ozz)
include_directories(lib/ozz-animation/src)

list(APPEND main_src
	lib/ozz-animation/src/animation/runtime/local_to_model_job.cc
	lib/ozz-animation/src/animation/runtime/skeleton_utils.cc
	lib/ozz-animation/src/animation/runtime/sampling_job.cc
	lib/ozz-animation/src/animation/runtime/animation.cc
	lib/ozz-animation/src/animation/runtime/blending_job.cc
	lib/ozz-animation/src/animation/runtime/skeleton.cc
	lib/ozz-animation/src/animation/offline/animation_optimizer.cc
	lib/ozz-animation/src/animation/offline/skeleton_builder.cc
	lib/ozz-animation/src/animation/offline/additive_animation_builder.cc
	lib/ozz-animation/src/animation/offline/raw_skeleton.cc
	lib/ozz-animation/src/animation/offline/animation_builder.cc
	lib/ozz-animation/src/animation/offline/raw_animation.cc
	lib/ozz-animation/src/animation/offline/raw_animation_archive.cc
	lib/ozz-animation/src/animation/offline/raw_animation_utils.cc
	lib/ozz-animation/src/animation/offline/raw_skeleton_archive.cc
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

#lib/ozz-animation/src/animation/offline/fbx/fbx_base.cc
#lib/ozz-animation/src/animation/offline/fbx/fbx2anim.cc
#lib/ozz-animation/src/animation/offline/fbx/fbx.cc
#lib/ozz-animation/src/animation/offline/fbx/fbx_skeleton.cc
#lib/ozz-animation/src/animation/offline/fbx/fbx_animation.cc
#lib/ozz-animation/src/animation/offline/fbx/fbx2skel.cc
#lib/ozz-animation/src/animation/offline/tools/convert2anim.cc
#lib/ozz-animation/src/animation/offline/tools/convert2skel.cc


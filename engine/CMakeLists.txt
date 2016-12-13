cmake_minimum_required(VERSION 3.5)
project(NoobWerkz)

#set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_BUILD_TYPE "RELEASE")
set(BUILD_SHARED_LIBS false)

# Current Bullet bug: -ftrapv causes crash on Android.
set(NOOB_COMMON_FLAGS "-Werror=return-type")
set(NOOB_COMMON_C_FLAGS "-DSTB_IMAGE_STATIC -DFT2_BUILD_LIBRARY")
set(NOOB_COMMON_CXX_FLAGS "-std=c++14 -fno-exceptions -fno-rtti -DEIGEN_MPL2_ONLY")

set(NOOB_STRICT_FLAGS " -pedantic -Wdouble-promotion -Wshadow -pedantic-errors -Wextra -Wall -Wextra -Winit-self -Wold-style-cast -Wuninitialized -Wmissing-declarations")
set(NOOB_STRICT_C_FLAGS "-Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition")
set(NOOB_STRICT_CXX_FLAGS "-fno-elide-constructors -Woverloaded-virtual -Winit-self ")


set(CMAKE_C_FLAGS "${NOOB_COMMON_FLAGS} ${NOOB_COMMON_C_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${NOOB_COMMON_FLAGS} ${NOOB_COMMON_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

if(NOT NOOB_STRICT_FLAGS)
	set(CMAKE_C_FLAGS "${NOOB_STRICT_FLAGS} ${NOOB_STRICT_C_FLAGS} ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${NOOB_STRICT_FLAGS} ${NOOB_STRICT_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
endif()

list (APPEND main_src)
include(cmake/FreeType.cmake)
include(cmake/BaseUtils.cmake)
include(cmake/UTF8.cmake)
include(cmake/Voropp.cmake)
include(cmake/Ozz.cmake)
include(cmake/Assimp.cmake)
include(cmake/Bullet.cmake)
include(cmake/Ogg.cmake)
include(cmake/Vorbis.cmake)
include(cmake/Recast.cmake)
include(cmake/R8.cmake)
include(cmake/Shiny.cmake)
include(cmake/TextureAtlas.cmake)

#add_subdirectory(lib/ozz-animation)

include_directories(lib/sema)
include_directories(lib/glm)
include_directories(lib/eigen)
include_directories(lib/sdf)
include_directories(lib/stb)
include_directories(lib/math-funcs/include)
include_directories(common)

list(APPEND common_src
	common/Globals.cpp
	common/Mesh3D.cpp
	common/Graphics.cpp
	common/Materials.cpp	
	common/Application.cpp
	common/Camera.cpp
	common/Stage.cpp
	common/Body.cpp
	common/Joint.cpp
	common/Ghost.cpp
	common/Shape.cpp
	common/Shapes.cpp	
	common/Physics.cpp
	common/MeshUtils.cpp
	common/SkeletalAnim.cpp
	common/Particles.cpp
	common/GUI.cpp
	common/BinPacker.cpp
	common/Mixer.cpp
	common/AudioSample.cpp
	common/Sounds.cpp	
	common/Font.cpp
	common/TextureLoader.cpp
	)

list(APPEND user_src
	../sandbox/UserApp.cpp
	)

list (APPEND main_src ${common_src} ${user_src})

########################
# Platforms
########################

if (NOOB_PLATFORM_LINUX)
	set(CMAKE_C_FLAGS "-DHAVE_ALLOCA_H ${CMAKE_C_FLAGS}")
	include_directories(lib/Khronos)
	add_subdirectory(lib/glfw)
	add_subdirectory(lib/libsoundio)
	
	include_directories(lib/glfw/include)
	include_directories(platform/nix)

	list(APPEND main_src
		platform/nix/EngineDesktop.cpp
		platform/nix/NDOF.cpp
		platform/desktop/SoundInterface.cpp)

	add_executable(NoobEdit ${main_src})

	target_link_libraries(NoobEdit LINK_PUBLIC rt z dl pthread m soundio.a asound spnav X11 Xrandr Xcursor Xinerama glfw ${GLFW_LIBRARIES} GL ${OpenGL_gl_LIBRARY})
endif()

if (NOOB_PLATFORM_ANDROID)
	include_directories("$ENV{ANDROID_HOME}/ndk-bundle/platforms/android-21/arch-arm/usr/include")
	LINK_DIRECTORIES($ENV{ANDROID_HOME}/ndk-bundle/platforms/android-21/arch-arm/usr/lib)
	set (CMAKE_C_FLAGS "-DNOOB_PLATFORM_ANDROID -std=c99 ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-DNOOB_PLATFORM_ANDROID ${CMAKE_CXX_FLAGS}")

	list (APPEND main_src
		platform/droid/EngineDroid.cpp
		platform/droid/AudioInterfaceDroid.cpp
		)

	add_library(Entry SHARED ${main_src})
	target_link_libraries(Entry dl log android EGL GLESv3 OpenSLES)
endif()

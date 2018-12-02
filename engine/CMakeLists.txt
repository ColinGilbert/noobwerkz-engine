cmake_minimum_required(VERSION 3.5)
project(NoobWerkz)

# set(CMAKE_BUILD_TYPE "DEBUG")
	set(CMAKE_BUILD_TYPE "RELEASE")
set(BUILD_SHARED_LIBS false)

# Bullet bug: -ftrapv causes crash on Android.
	set(NOOB_COMMON_FLAGS "-Werror=return-type -DHAVE_UCDN -DHAVE_OT -DHB_NO_MT -DBSTRLIB_DOESNT_THROW_EXCEPTIONS")

	set(NOOB_COMMON_C_FLAGS "-DSTB_IMAGE_STATIC -DFT2_BUILD_LIBRARY -DSQLITE_THREADSAFE=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS -DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DECLTYPE -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_PROGRESS_CALLBACK -DSQLITE_OMIT_SHARED_CACHE -DSQLITE_ENABLE_FTS5")

	set(NOOB_COMMON_CXX_FLAGS "-std=c++14 -fno-exceptions -fno-rtti -DEIGEN_MPL2_ONLY")

	set(NOOB_STRICT_FLAGS " -pedantic -Wdouble-promotion -Wshadow -pedantic-errors -Wextra -Wall -Wextra -Winit-self -Wold-style-cast -Wuninitialized -Wmissing-declarations")
	set(NOOB_STRICT_C_FLAGS "-Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition")
	set(NOOB_STRICT_CXX_FLAGS "-fno-elide-constructors -Woverloaded-virtual -Winit-self ")


	set(CMAKE_C_FLAGS "${NOOB_COMMON_FLAGS} ${NOOB_COMMON_C_FLAGS} ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${NOOB_COMMON_FLAGS} ${NOOB_COMMON_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

if(NOOB_STRICT)
	set(CMAKE_C_FLAGS "${NOOB_STRICT_FLAGS} ${NOOB_STRICT_C_FLAGS} ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${NOOB_STRICT_FLAGS} ${NOOB_STRICT_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
endif()


list (APPEND main_src)
#include(cmake/BStr.cmake)
#include(cmake/FreeType.cmake)
#include(cmake/Assimp.cmake)
#include(cmake/Recast.cmake)
#include(cmake/Shiny.cmake)
#include(cmake/Harfbuzz.cmake)
#add_subdirectory(lib/ozz-animation)

	include(cmake/BaseUtils.cmake)
	include(cmake/Voropp.cmake)
	include(cmake/Ozz.cmake)
	include(cmake/Bullet.cmake)
	include(cmake/Ogg.cmake)
	include(cmake/Vorbis.cmake)
	include(cmake/R8.cmake)
	include(cmake/TextureAtlas.cmake)
	include(cmake/Sqlite.cmake)

	add_subdirectory(lib/freetype)
	
	include_directories(lib/freetype/include)
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
			common/Mixer.cpp
			common/AudioSample.cpp
			common/Sounds.cpp	
	common/Font.cpp
	common/TextureLoader.cpp
	common/Database.cpp
	)

	list(APPEND user_src
			../sandbox/user-project/UserApp.cpp
	    )

list (APPEND main_src ${common_src} ${user_src})

########################
# Platforms
########################

if (NOOB_PLATFORM_LINUX)
	#set(CMAKE_C_FLAGS "-DHAVE_ALLOCA_H ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "-DNOOB_PLATFORM_LINUX ${CMAKE_CXX_FLAGS}")

	include_directories(lib/Khronos/OpenGL-Registry/api)
	include_directories(lib/Khronos/EGL-Registry/api)	

	find_package(X11)
	find_library(M_LIB m)

	find_library(OPENGLES3_LIBRARY GLESv2)
	find_library(EGL_LIBRARY EGL)

#include_directories(${EGL_INCLUDE_DIR})

	include_directories(lib/libsoundio)
	add_subdirectory(lib/libsoundio)

#include_directories(lib/glfw/include)
	include_directories(platform/nix)

	list(APPEND main_src
			platform/nix/EntryPoint.cpp
			platform/nix/EngineNix.cpp
			platform/nix/AppFacade.cpp
			platform/desktop/SoundInterface.cpp)

list (APPEND 3dmouse)

if (NOOB_SPNAV)
	set (CMAKE_CXX_FLAGS "-DNOOB_SPNAV ${CMAKE_CXX_FLAGS}")
	list (APPEND 3dmouse spnav)
	list (APPEND main_src platform/nix/NDOF.cpp)
endif()

add_executable(NoobEdit ${main_src})

target_link_libraries(NoobEdit LINK_PUBLIC rt z dl pthread libsoundio_static asound freetype Xi ${OPENGLES3_LIBRARY} ${EGL_LIBRARY} ${X11_LIBRARIES} ${M_LIBRARY} ${3dmouse})

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

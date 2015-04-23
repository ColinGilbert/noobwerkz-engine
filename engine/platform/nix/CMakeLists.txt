cmake_minimum_required (VERSION 3.0)

project (Engine)
SET(CMAKE_CC "clang++")
SET(CMAKE_CXX_FLAGS "-fpermissive -std=c++11 -DASIO_STANDALONE -DASIO_DISABLE_THREADS ${CMAKE_CXX_FLAGS}")
SET(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")

add_subdirectory(extern)

include_directories(extern/cereal/include)
include_directories(extern/asio/asio)
include_directories(extern/noisepp/core)
include_directories(extern/noisepp/threadpp)
include_directories(extern/noisepp/utils/include)
include_directories(extern/polyvox/include)
include_directories(extern/freetype/include)
include_directories(extern/bx/include)
include_directories(extern/bgfx/include)
include_directories(extern/bgfx/3rdparty/khronos)

LIST(APPEND bgfx_src
extern/bgfx/src/amalgamated.cpp)

LIST(APPEND soil2_src
	extern/soil2/src/SOIL2/etc1_utils.c
	extern/soil2/src/SOIL2/stb_image_write.c
	extern/soil2/src/SOIL2/stb_image.c
	extern/soil2/src/SOIL2/image_DXT.c
	extern/soil2/src/SOIL2/image_helper.c
	extern/soil2/src/SOIL2/SOIL2.c)

LIST(APPEND noisepp_src
	extern/noisepp/utils/NoiseColourValue.cpp
	extern/noisepp/utils/NoiseGradientRenderer.cpp
	extern/noisepp/utils/NoiseInStream.cpp
	extern/noisepp/utils/NoiseModules.cpp
	extern/noisepp/utils/NoiseReader.cpp
	extern/noisepp/utils/NoiseWriter.cpp
	extern/noisepp/utils/NoiseBuilders.cpp
	extern/noisepp/utils/NoiseEndianUtils.cpp
	extern/noisepp/utils/NoiseImage.cpp
	extern/noisepp/utils/NoiseJobQueue.cpp
	extern/noisepp/utils/NoiseOutStream.cpp
	extern/noisepp/utils/NoiseSystem.cpp)	

LIST(APPEND headerz Logger.hpp Application.hpp glHeaders.h NoobUtils.hpp Model.hpp Graphics.hpp Renderer.hpp MathFuncs.hpp CommonIncludes.hpp)

LIST(APPEND sourcez Logger.cpp Main.cpp Renderer.cpp Application.cpp Graphics.cpp NoobUtils.cpp Model.cpp MathFuncs.cpp SandboxApp.cpp Camera.cpp CommonIncludes.cpp)

add_executable(Engine ${headerz} ${sourcez} ${soil2_src} ${noisepp_src} ${bgfx_src})

target_link_libraries(Engine LINK_PUBLIC glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} anl freetype Newton assimp rt dl pthread tinyxml)

project (LogServer)

add_executable(LogServer LogServer.cpp)

if (DEFINED NOOB_USE_SHINY)
	SET(CMAKE_CXX_FLAGS "-DSHINY_IS_COMPILED=TRUE  ${CMAKE_CXX_FLAGS}")

	include_directories(lib/Shiny/include)

	list(APPEND main_src
		lib/Shiny/src/ShinyManager.cpp
		lib/Shiny/src/ShinyNode.cpp
		lib/Shiny/src/ShinyNodePool.cpp
		lib/Shiny/src/ShinyOutput.cpp
		lib/Shiny/src/ShinyTools.cpp
		)
endif()

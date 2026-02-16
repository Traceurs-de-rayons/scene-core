cmake_minimum_required(VERSION 3.16)
project(scene-core VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SCENE_CORE_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(scene-core STATIC ${SCENE_CORE_SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(scene-core PUBLIC -std=gnu++2b)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(scene-core PUBLIC /std:c++latest)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
		target_link_libraries(scene-core PRIVATE stdc++fs)
	endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
		target_link_libraries(scene-core PRIVATE stdc++fs)
	endif()
endif()

target_include_directories(scene-core
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:include>
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core-utils core-utils-build)

target_link_libraries(scene-core
	PUBLIC core-utils
)

set_target_properties(scene-core PROPERTIES OUTPUT_NAME "scene-core")

include(GNUInstallDirs)
install(TARGETS scene-core
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(BUILD_CORE_UTILS_EXAMPLES "Build examples for scene-core" OFF)

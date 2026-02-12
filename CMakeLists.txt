cmake_minimum_required(VERSION 3.16)
project(scene-core VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SCENE_CORE_SOURCES
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/*.c"
)

add_library(scene-core STATIC ${SCENE_CORE_SOURCES})

target_compile_features(scene-core PUBLIC cxx_std_17)

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
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

add_subdirectory(${CMAKE_SOURCE_DIR}/../core-utils core-utils-build)

target_link_libraries(scene-core
	PUBLIC core-utils
)

# target_include_directories(core-utils
#   PUBLIC
#     $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
#     $<INSTALL_INTERFACE:include>
# )

# install(TARGETS core-utils
#   EXPORT core-utils-targets
#   ARCHIVE DESTINATION lib
#   INCLUDES DESTINATION include
# )

# install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)
# install(EXPORT core-utils-targets
#   FILE core-utils-targets.cmake
#   NAMESPACE core-utils::
#   DESTINATION lib/cmake/core-utils
# )

target_compile_features(scene-core PUBLIC cxx_std_17)

set_target_properties(scene-core PROPERTIES OUTPUT_NAME "scene-core")

include(GNUInstallDirs)
install(TARGETS scene-core
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

option(BUILD_CORE_UTILS_EXAMPLES "Build examples for scene-core" OFF)

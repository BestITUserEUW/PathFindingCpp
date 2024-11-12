cmake_minimum_required(VERSION 3.10)

project(PathFinding)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT /O2")
endif()

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/monitor.cpp
	src/path_finding.cpp
	src/entity.cpp
	src/mission_runner.cpp
	src/argparse.cpp
	src/windows_helpers.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Threads::Threads
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

target_link_libraries(${PROJECT_NAME} PRIVATE
	stdc++exp
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-static-libstdc++ -static-libgcc
)

target_link_options(${PROJECT_NAME} PRIVATE
	-s -g0
)

endif()
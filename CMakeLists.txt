cmake_minimum_required(VERSION 3.10)

project(PathFinding)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/monitor.cpp
	src/path_finding.cpp
	src/entity.cpp
	src/argparse.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Threads::Threads
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

target_link_libraries(${PROJECT_NAME} PRIVATE
	stdc++exp
)

target_compile_options(${PROJECT_NAME} PRIVATE
	-static-libstdc++ 
	-static-libgcc
	-O3
)

target_link_options(${PROJECT_NAME} PRIVATE
	-s
	-g0
)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

target_compile_options(${PROJECT_NAME} PRIVATE
	/MT 
	/O2
)

endif()
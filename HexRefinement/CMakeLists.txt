cmake_minimum_required(VERSION 3.10)
project(HexRefinement)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-O3")

SET(ENV{VTK_DIR} "D:/Program Files (x86)/VTK") 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# gdb debug
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(src SRC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})


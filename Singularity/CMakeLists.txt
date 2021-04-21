cmake_minimum_required(VERSION 3.10)
project(Test)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O3")

# open volume mesh
set(OPEN_VOLUME_MESH_USE_STATIC_LIBS ON)
include_directories("D:/Program Files (x86)/OpenVolumeMesh_mingw/include")
link_directories("D:/Program Files (x86)/OpenVolumeMesh_mingw/lib")

# gdb debug
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
# SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(src SRC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} OpenVolumeMesh)




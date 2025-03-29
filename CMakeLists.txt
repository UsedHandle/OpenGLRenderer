cmake_minimum_required(VERSION 3.12)

set(EXE_NAME "modelloading2")

project(${EXE_NAME} C CXX)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY	https://github.com/assimp/assimp.git
	GIT_TAG master
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(glad STATIC glad.c)
add_executable(${EXE_NAME} modelloading.cpp fonts.cpp camera.cpp model.cpp shaders.cpp textures.cpp utility.cpp)

if(MSVC)
	target_compile_options(${EXE_NAME} PRIVATE /W4)
else()
	target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -O3)
	target_compile_options(glad PRIVATE -O3)
endif()

include_directories(include/ libs/assimp-src/include)
target_link_libraries(${EXE_NAME} PRIVATE  glad glm::glm assimp glfw freetype ${OPENGL_LIBRARIES} )

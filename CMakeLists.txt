cmake_minimum_required(VERSION 3.12)

set(EXE_NAME "OpenGLRenderer")

project(${EXE_NAME} C CXX)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 		0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)


find_package(assimp QUIET)
if( NOT assimp_FOUND )
	FetchContent_Declare(
		assimp
		GIT_REPOSITORY	https://github.com/assimp/assimp.git
		GIT_TAG			master
	)

	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
	set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
	set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

	FetchContent_MakeAvailable(assimp)
endif()

find_package(glfw3 QUIET)

if( NOT glfw3_FOUND )
	FetchContent_Declare(
		glfw3
		GIT_REPOSITORY	https://github.com/glfw/glfw.git
		GIT_TAG		7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # 3.4
	)

	set( GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" )
	set( GLFW_INSTALL OFF CACHE BOOL "GLFW lib only" )

	FetchContent_MakeAvailable(glfw3)
endif()


#find_package(Freetype REQUIRED)
find_path(FREETYPE_INCLUDE_DIR NAMES freetype.h PATH_SUFFIXES freetype )
find_library(FREETYPE_LIBRARY
	NAMES freetype
	PATH_SUFFIXES lib
)


if(NOT FREETYPE_LIBRARY-NOTFOUND)
	if(NOT TARGET Freetype::Freetype)
		add_library(Freetype::Freetype INTERFACE IMPORTED)
		set_target_properties(Freetype::Freetype PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_INCLUDE_DIR}"
		INTERFACE_LINK_LIBRARIES "${FREETYPE_LIBRARY}"
		)
	endif()
endif()

find_package(OpenGL REQUIRED)

add_library(glad STATIC src/glad.c)
add_executable(${EXE_NAME} src/main.cpp src/fonts.cpp src/camera.cpp src/model.cpp src/shaders.cpp src/textures.cpp src/utility.cpp)

if(MSVC)
	target_compile_options(${EXE_NAME} PRIVATE /W4)
else()
	target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -O3)
	target_compile_options(glad PRIVATE -O3)
endif()

# include_directories(include/ libs/assimp-src/include)
include_directories(include/ ${FREETYPE_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR})
target_link_libraries(${EXE_NAME} PRIVATE  glad glm::glm assimp glfw Freetype::Freetype ${OPENGL_LIBRARIES} )

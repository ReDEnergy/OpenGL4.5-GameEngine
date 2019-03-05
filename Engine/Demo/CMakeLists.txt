cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

# =============================================================================
# General variables
# =============================================================================

set(APP_ROOT "./")
set(SOURCE_ROOT "./")
set(APP_NAME "EngineDemo")
set(APP_VERSION "0.1.0")

project(${APP_NAME})

# =============================================================================
# Platform
# =============================================================================

if(WIN32)
	set(TARGET_PLATFORM "Win")
elseif(UNIX)
	set(TARGET_PLATFORM "Linux")

	# test if arm arhitecture
	set(IS_ARM FALSE CACHE BOOL "Build ARM")
	if(IS_ARM)
		set(TARGET_PLATFORM "ARM")
	endif()
else()
	message(FATAL_ERROR "Requested platform not supported")
endif()

message(STATUS "Target platform: ${TARGET_PLATFORM}")


# =============================================================================
# Architecture
# =============================================================================

set(IS_X86 false)
set(IS_X64 false)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(TARGET_ARCH x64)
	set(IS_X64 TRUE)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(TARGET_ARCH x86)
	set(IS_X86 TRUE)
endif()

MESSAGE(STATUS "Target architecture: ${TARGET_ARCH}")

# =============================================================================
# Build Type
# =============================================================================

# based on https://cmake.org/pipermail/cmake/2012-March/049620.html
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(BUILD_TYPE "Debug")
	set(IS_DEBUG TRUE)
	add_definitions(-D_DEBUG)
else()
	set(BUILD_TYPE "Release")
	set(IS_DEBUG FALSE)
endif()


message(STATUS "Build type: ${BUILD_TYPE}")

# =============================================================================
# Build Output Path
# =============================================================================

# For the moment the linux build does not support multi-build project files

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin/Debug")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/Release")
elseif(UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()

# =============================================================================
# Helper Macros/Functions
# =============================================================================

macro(add_lib_type CONFIGURATION FILES)
	set(LIBS_${CONFIGURATION} ${LIBS_${CONFIGURATION}} ${FILES})
endmacro()

macro(add_libs FILES)
	set(LIBS_COMMON ${LIBS_COMMON} ${FILES})
endmacro()

macro(copy_dlls FILES)
	set(COPY_LIST_DLL ${COPY_LIST_DLL} ${FILES})
endmacro()

macro(copy_dll_type CONFIGURATION FILES)
	set(COPY_LIST_${CONFIGURATION} ${COPY_LIST_${CONFIGURATION}} ${FILES})
endmacro()

macro(add_includes DIR)
	set(INCLUDE_DIRS ${INCLUDE_DIRS} ${DIR})
endmacro()

macro(add_sources FILES)
	set(SOURCE_FILES ${SOURCE_FILES} ${FILES})
endmacro()

# =============================================================================
# Libraries
# =============================================================================

set(LIB_DIRS "")
set(LIBS_RELEASE "")
set(LIBS_DEBUG "")
set(ADDITIONAL_LIBS "")

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/../libs" CACHE PATH "Libraries directory")

set(INCLUDE_DIRS "")
set(INCLUDE_DIRS ${INCLUDE_DIRS} "${LIBS_DIR}")

set(COPY_LIST_DLL "")
set(COPY_LIST_DEBUG "")
set(COPY_LIST_RELEASE "")

if( "${LIBS_DIR}" STREQUAL "")
	message(FATAL_ERROR "Please setLIBS_DIR Libraries directory")
endif()

# =============================================================================
# Source Files
# =============================================================================

file(GLOB_RECURSE SYNC_DEPENDS_HEADERS ${SOURCE_ROOT}/*.h ${SOURCE_ROOT}/*.hpp)
file(GLOB_RECURSE SYNC_DEPENDS_IMPL ${SOURCE_ROOT}/*.c ${SOURCE_ROOT}/*.cpp)
set(SOURCE_FILES ${SYNC_DEPENDS_HEADERS} ${SYNC_DEPENDS_IMPL})

# =============================================================================
# Libraries Configuration
# =============================================================================

# -----------------------------------------------------------------------------
# GLEW library
# -----------------------------------------------------------------------------

if(NOT OPENGL_ES)
	set(LIB_GLEW ${LIBS_DIR}/GLEW/${TARGET_ARCH}/${TARGET_PLATFORM})
	add_includes(${LIBS_DIR}/GLEW/include)
	# add_sources(${LIBS_DIR}/GLEW/src/glew.c)
	# add_compile_definitions(GLEW_STATIC)
	if(WIN32)
		add_libs(${LIB_GLEW}/glew32.lib)
		copy_dlls(${LIB_GLEW}/glew32.dll)
		add_libs("opengl32.lib")
	elseif(UNIX)
		add_libs(${LIB_GLEW}/libGLEW.so.2.1)
		copy_dlls(${LIB_GLEW}/libGLEW.so.2.1)
		add_libs(libGLU.so)
		add_libs(libGL.so)
	endif()
endif()

# -----------------------------------------------------------------------------
# GLFW library
# -----------------------------------------------------------------------------

set(LIB_GLFW ${LIBS_DIR}/GLFW/${TARGET_ARCH}/${TARGET_PLATFORM})

if(WIN32)
	add_libs(${LIB_GLFW}/glfw3dll.lib)
	copy_dlls(${LIB_GLFW}/glfw3.dll)
else(UNIX)
	file(GLOB LIB_FILES ${LIB_GLFW}/*.so.*)
	add_libs("${LIB_FILES}")
	copy_dlls("${LIB_FILES}")
endif()

# -----------------------------------------------------------------------------
# OpenGL Engine Config
# -----------------------------------------------------------------------------

set(INCLUDE_DIRS ${INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/Source")
set(LIB_Engine "${CMAKE_SOURCE_DIR}/lib/${TARGET_ARCH}/${TARGET_PLATFORM}")

if(WIN32)

	add_lib_type(DEBUG ${LIB_Engine}/Debug/Engine.lib)
	add_lib_type(RELEASE ${LIB_Engine}/Release/Engine.lib)

	file(GLOB FILES_RELEASE ${LIB_Engine}/Release/*.dll)
	copy_dll_type(RELEASE ${FILES_RELEASE})

	file(GLOB FILES_DEBUG ${LIB_Engine}/Debug/*.dll)
	copy_dll_type(DEBUG ${FILES_DEBUG})

	# copy dll's used only by engine
	copy_dlls(${LIB_Engine}/assimp.dll)

elseif(UNIX)
	set(LIB_Engine ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../../)
	file(GLOB LIB_FILES ${LIB_Engine}/*.so.*)
	add_libs("${LIB_FILES}")
	copy_dlls("${LIB_FILES}")
endif()

# =============================================================================
# Copy Dynamic Libraries
# =============================================================================

if(WIN32)
	file(COPY ${COPY_LIST_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
	file(COPY ${COPY_LIST_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
	file(COPY ${COPY_LIST_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
	file(COPY ${COPY_LIST_RELEASE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
elseif(UNIX)
	file(COPY ${COPY_LIST_DLL} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	file(COPY ${COPY_LIST_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	file(COPY ${COPY_LIST_RELEASE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# =============================================================================
# Additional libraries configuration
# =============================================================================

link_directories(${ADDITIONAL_LIBS})

# =============================================================================
# Copy additional resources
# =============================================================================

file(COPY ${CMAKE_SOURCE_DIR}/App/Config DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY ${CMAKE_SOURCE_DIR}/App/Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# =============================================================================
# Executable
# =============================================================================

message ("=====================================================================")

include_directories(${INCLUDE_DIRS})
add_executable(${APP_NAME} ${SOURCE_FILES})

set(_LIBS_RELEASE )
foreach(LIB ${LIBS_RELEASE})
	set(_LIBS_RELEASE ${_LIBS_RELEASE} optimized ${LIB})
endforeach()

set(_LIBS_DEBUG "")
foreach(LIB ${LIBS_DEBUG})
	set(_LIBS_DEBUG ${_LIBS_DEBUG} debug ${LIB})
endforeach()

target_link_libraries(${APP_NAME} ${LIBS_COMMON} ${_LIBS_RELEASE} ${_LIBS_DEBUG})
set_target_properties(${APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${APP_NAME} PROPERTIES VS_STARTUP_PROJECT ${APP_NAME})

# add_custom_command(TARGET ${APP_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${LIB_Engine}/Debug/*.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# =============================================================================
# Compiler Flags
# =============================================================================

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_target_properties(${APP_NAME} PROPERTIES CXX_STANDARD 14)

if(MSVC)

	set_source_files_properties("${CMAKE_SOURCE_DIR}/${SOURCE_ROOT}/Modules/DepthReconstruction/DepthReconstructionPCL.cpp" PROPERTIES COMPILE_FLAGS "/std:c++14")

	set(COMMON_FLAGS "-D_CRT_SECURE_NO_WARNINGS \
	-D_WINSOCK_DEPRECATED_NO_WARNINGS -D_WINDOWS \
	-D_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING \
	-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")

	set(CMAKE_CXX_FLAGS_DEBUG ${COMMON_FLAGS})
	set(CMAKE_CXX_FLAGS_RELEASE ${COMMON_FLAGS})

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /nologo /W3 /WX-
								/sdl /MP /Od /EHsc /RTC1 /MDd /GS
								/fp:precise /Gd /errorReport:prompt")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /nologo /W3
								 /WX- /sdl /MP /O2 /Oi /Ot /Oy- /GT
								 /EHsc /MD /GS /Gy /fp:precise
								 /Zc:wchar_t /Zc:forScope /Zc:inline
								 /Gd /errorReport:prompt")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE /Zi /fp:fast /sdl-")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE /MP")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

	# target_link_libraries(${APP_NAME} stdc++fs)
endif()

# =============================================================================
# Linker Flags
# =============================================================================

if(MSVC)
	# cl flags
	set_property(TARGET ${APP_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG "")
	set_property(TARGET ${APP_NAME} APPEND PROPERTY LINK_FLAGS_RELEASE "/MACHINE:X86 /DEBUG /OPT:REF /OPT:ICF /LTCG:incremental")
endif()

# =============================================================================
# Group Files by Directory
# =============================================================================
# Taken from: http://stackoverflow.com/a/33813154

foreach(_source IN ITEMS ${SOURCE_FILES})
	get_filename_component(_source_path "${_source}" PATH)
	file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/${SOURCE_ROOT}" "${_source_path}")
	string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
	source_group("${_group_path}" FILES "${_source}")
endforeach()
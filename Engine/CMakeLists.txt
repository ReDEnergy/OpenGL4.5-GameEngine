cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

# =============================================================================
# Modules
# =============================================================================

# =============================================================================
# General variables
# =============================================================================

set(SOURCE_ROOT "Source")
set(LIB_NAME "Engine")
set(APP_VERSION "0.2.0")

project(${LIB_NAME})

# =============================================================================
# App configuration
# =============================================================================


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
# Build Output Path
# =============================================================================

# For the moment the linux build does not support multi-build project files

if(WIN32)
	set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_ARCH}/${TARGET_PLATFORM})

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})

	# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR}/Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}/Release)
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

macro(copy_dll_type CONFIGURATION FILES)
	set(COPY_LIST_${CONFIGURATION} ${COPY_LIST_${CONFIGURATION}} ${FILES})
endmacro()

macro(copy_dlls FILES)
	set(COPY_LIST ${COPY_LIST} ${FILES})
endmacro()

macro(add_includes DIR)
	set(INCLUDE_DIRS ${INCLUDE_DIRS} ${DIR})
endmacro()

macro(add_sources FILES)
	set(SOURCE_FILES ${SOURCE_FILES} ${FILES})
endmacro()

macro(exclude_sources DIR)
	set(EXCLUDE_DIR "${CMAKE_SOURCE_DIR}/${SOURCE_ROOT}/${DIR}")
	# Remove modules that will not be used
	foreach (FILE_PATH ${SOURCE_FILES})
		string (FIND ${FILE_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
		# message (STATUS "${FILE_PATH} => ${EXCLUDE_DIR_FOUND}")
		if (${EXCLUDE_DIR_FOUND} EQUAL 0)
			list (REMOVE_ITEM SOURCE_FILES ${FILE_PATH})
			# message (STATUS "Removed path")
		endif ()
	endforeach()
endmacro()

# =============================================================================
# Source Files
# =============================================================================

file(GLOB_RECURSE SYNC_DEPENDS_HEADERS ${SOURCE_ROOT}/*.h ${SOURCE_ROOT}/*.hpp)
file(GLOB_RECURSE SYNC_DEPENDS_IMPL ${SOURCE_ROOT}/*.c ${SOURCE_ROOT}/*.cpp)
set(SOURCE_FILES ${SYNC_DEPENDS_HEADERS} ${SYNC_DEPENDS_IMPL})

# =============================================================================
# Libraries
# =============================================================================

set(LIB_DIRS "")
set(LIBS_RELEASE "")
set(LIBS_DEBUG "")

set(LIBS_DIR "${CMAKE_SOURCE_DIR}/../libs" CACHE PATH "Libraries directory")

set(INCLUDE_DIRS "")
add_includes(${LIBS_DIR})
add_includes(${CMAKE_SOURCE_DIR}/${SOURCE_ROOT})

set(COPY_LIST_DEBUG "")
set(COPY_LIST_RELEASE "")

if( "${LIBS_DIR}" STREQUAL "")
	message(FATAL_ERROR "Please set LIBS_DIR Libraries directory")
endif()

# =============================================================================
# Libraries Configuration
# =============================================================================

add_compile_definitions(NATIVE_CONTEXT)

set(OPENGL_ES FALSE CACHE BOOL "OPENGL_ES")
if(OPENGL_ES)

	add_compile_definitions(OPENGL_ES)

	if(WIN32)
		add_libs("${LIBS_DIR}/OpenGL ES/Mali/${TARGET_ARCH}/libEGL.lib")
		add_libs("${LIBS_DIR}/OpenGL ES/Mali/${TARGET_ARCH}/libGLESv2.lib")
	elseif(UNIX)
		add_libs("libEGL.so")
		add_libs("libGLESv2.so")
	endif()
else()
	if(WIN32)
		add_libs("opengl32.lib")
	endif()
endif()

# -----------------------------------------------------------------------------
# Pugixml
# -----------------------------------------------------------------------------

add_sources(${LIBS_DIR}/pugixml/pugixml.cpp)

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
	elseif(UNIX)
		add_libs(${LIB_GLEW}/libGLEW.so.2.1)
	endif()
endif()

# -----------------------------------------------------------------------------
# GLFW library
# -----------------------------------------------------------------------------

set(LIB_GLFW ${LIBS_DIR}/GLFW/${TARGET_ARCH}/${TARGET_PLATFORM})

if(WIN32)
	add_libs(${LIB_GLFW}/glfw3dll.lib)
	copy_dlls(${LIB_GLFW}/glfw3.dll)
elseif(UNIX)
	add_libs(${LIB_GLFW}/libglfw.so.3)
endif()

# -----------------------------------------------------------------------------
# Freetype library
# -----------------------------------------------------------------------------

set (Module_Text FALSE CACHE BOOL "Add text module (Freetype)")
if (Module_Text)
	add_compile_definitions(TEXT_RENDERING)
	set(LIB_FreeType "${LIBS_DIR}/FreeType")
	add_includes(${LIB_FreeType})

	set(LIB_FreeTypeGL "${LIBS_DIR}/freetype-gl")
	add_includes(${LIB_FreeTypeGL})

	if(WIN32)
		add_libs(${LIB_FreeType}/${TARGET_ARCH}/Release/freetype_static.lib)
		add_libs(${LIB_FreeTypeGL}/${TARGET_ARCH}/Release/freetype-gl.lib)
	elseif(UNIX)
		# add_libs(${LIB_FreeType}/${TARGET_ARCH}/${TARGET_PLATFORM}/libfreetype.so)
		add_libs(${LIB_FreeType}/${TARGET_ARCH}/${TARGET_PLATFORM}/libfreetype.a)
		# add_libs(libfreetype.a)
		add_libs(${LIB_FreeTypeGL}/${TARGET_ARCH}/${TARGET_PLATFORM}/libfreetype-gl.a)
	endif()
endif()

# -----------------------------------------------------------------------------
# Assimp library
# -----------------------------------------------------------------------------

set(LIB_Assimp "${LIBS_DIR}/assimp")
add_includes(${LIB_Assimp})

if(WIN32)
	add_libs(${LIB_Assimp}/${TARGET_ARCH}/Release/assimp.lib)
	copy_dlls(${LIB_Assimp}/${TARGET_ARCH}/Release/assimp.dll)
elseif(UNIX)
	add_libs(${LIB_Assimp}/${TARGET_ARCH}/${TARGET_PLATFORM}/libassimp.so.3)
endif()

# =============================================================================
# Copy Dynamic Libraries
# =============================================================================

if(WIN32)
	file(COPY ${COPY_LIST} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	file(COPY ${COPY_LIST_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
	file(COPY ${COPY_LIST_RELEASE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

# =============================================================================
# Generate Library Project
# =============================================================================

message ("=====================================================================")

add_compile_definitions(ENGINE_DLL_EXPORTS)

include_directories(${INCLUDE_DIRS})
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${LIB_NAME})
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(${LIB_NAME} PROPERTIES VERSION ${APP_VERSION})

set(_LIBS_RELEASE "")
foreach(LIB ${LIBS_RELEASE})
	set(_LIBS_RELEASE ${_LIBS_RELEASE} optimized ${LIB})
endforeach()

set(_LIBS_DEBUG "")
foreach(LIB ${LIBS_DEBUG})
	set(_LIBS_DEBUG ${_LIBS_DEBUG} debug ${LIB})
endforeach()

target_link_libraries(${LIB_NAME} ${LIBS_COMMON} ${_LIBS_RELEASE} ${_LIBS_DEBUG})

# =============================================================================
# Compiler Flags
# =============================================================================

# set_target_properties(${LIB_NAME} PROPERTIES CXX_STANDARD 17)

if(MSVC)
	set (COMMON_FLAGS "-D_CRT_SECURE_NO_WARNINGS \
	-D_WINSOCK_DEPRECATED_NO_WARNINGS -D_WINDOWS \
	-D_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING \
	-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")

	set(CMAKE_CXX_FLAGS_DEBUG ${COMMON_FLAGS})
	set(CMAKE_CXX_FLAGS_RELEASE ${COMMON_FLAGS})

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /nologo /W3 /WX-
								/sdl /MP /Od /EHsc /RTC1 /MDd /GS
								/fp:precise /openmp /Gd /errorReport:prompt")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /nologo /W3
								 /WX- /sdl /MP /O2 /Oi /Ot /Oy- /GT
								 /EHsc /MD /GS /Gy /fp:precise
								 /Zc:wchar_t /Zc:forScope /Zc:inline /openmp
								 /Gd /errorReport:prompt")

	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE /Zi /fp:fast /sdl-")
	#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE /MP")
	#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
endif()

if(UNIX)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# =============================================================================
# Linker Flags
# =============================================================================

if(MSVC)
	# cl flags
	SET_PROPERTY(TARGET ${LIB_NAME} APPEND PROPERTY LINK_FLAGS_DEBUG "")
	SET_PROPERTY(TARGET ${LIB_NAME} APPEND PROPERTY LINK_FLAGS_RELEASE "/MACHINE:X86 /DEBUG /OPT:REF /OPT:ICF /LTCG:incremental")
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

message ("Configuration Done")
message ("=====================================================================")

# =============================================================================
# Demo project
# =============================================================================

# message ("")
# message ("Demo configuration")

add_subdirectory(Demo)

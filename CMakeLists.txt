cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(discord-connector VERSION 0.3.7)

if (NOT DCC_VERSION) # allow version override
	set(DCC_VERSION ${PROJECT_VERSION})
endif()

# Conan builds of some third-party packages require CMake policy minimum set.
set(ENV{CMAKE_POLICY_VERSION_MINIMUM} "3.5")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")
include(SAMPPluginToolkit)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


if(MSVC)
	set(CMAKE_BUILD_TYPE "" CACHE STRING "" FORCE)
	add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING) # silence deprecation warning for stdext::make_checked_array_iterator in fmt 7.1.3
endif()

if(UNIX)
	set(
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/artifact/"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/artifact/"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/artifact/"
	)
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	SET(CMAKE_INSTALL_RPATH "$\{ORIGIN\}")
endif()

# -- CONAN
# cmake-conan 0.18.1 doesn't recognize MSVC 19.44+ (VS2022 17.10+).
# Temporarily clamp MSVC_VERSION for conan detection only.
if(MSVC AND MSVC_VERSION GREATER_EQUAL 1940)
	set(DCC_ORIG_MSVC_VERSION "${MSVC_VERSION}")
	set(MSVC_VERSION 1939)
	set(DCC_CONAN_MSVC_VERSION_OVERRIDDEN TRUE)
endif()

include(libs/cmake-conan/conan.cmake)
if(MSVC)
	set(CONAN_SETTINGS_WINDOWS
		"compiler.version=17"
		"compiler.toolset=v143"
	)
endif()
function(add_conan_lib pkg_name conan_pkg)
	conan_check(REQUIRED)

	conan_cmake_run(
		REQUIRES ${conan_pkg}
		ARCH x86
		BASIC_SETUP CMAKE_TARGETS
		BUILD missing
		SETTINGS ${CONAN_SETTINGS_WINDOWS}
	)

	set_target_properties(${pkg_name} PROPERTIES IMPORTED_GLOBAL TRUE)
endfunction()

if (MSVC)
	# link runtime statically
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
elseif(UNIX)
	# hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fno-builtin-getentropy")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fno-builtin-getentropy")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

	# link runtime statically
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath='$ORIGIN' -static-libstdc++ -static-libgcc")
endif()

if(NOT DEFINED OPENSSL_USE_STATIC_LIBS)
	set(OPENSSL_USE_STATIC_LIBS ON)
	set(OPENSSL_MSVC_STATIC_RT ON)
endif()

if(USING_BOOST_NO_CONAN)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_DEBUG_LIBS ON)
	set(Boost_USE_RELEASE_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost 1.69 QUIET)
	if(NOT Boost_FOUND)
		set(BOOST_ROOT "${BOOST_ROOT}" CACHE PATH "Set the Boost root folder.")
		mark_as_advanced(CLEAR BOOST_ROOT)
		message(FATAL_ERROR "Could NOT find Boost; specify additional paths.")
	endif()
	mark_as_advanced(BOOST_ROOT Boost_DIR)

	set(Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIRS}" CACHE INTERNAL "Boost include directories")
	set(Boost_INCLUDE_DIR "${Boost_INCLUDE_DIR}" CACHE INTERNAL "Boost include directory")
else()
	conan_check(REQUIRED)
	conan_cmake_run(
		REQUIRES boost/1.79.0
		ARCH x86
		BASIC_SETUP CMAKE_TARGETS
		BUILD missing
		OPTIONS boost:header_only=True boost:zlib=False
		SETTINGS ${CONAN_SETTINGS_WINDOWS}
	)
	set_target_properties(CONAN_PKG::boost PROPERTIES IMPORTED_GLOBAL TRUE)
	get_target_property(DCC_BOOST_INCLUDE_DIRS CONAN_PKG::boost INTERFACE_INCLUDE_DIRECTORIES)
	set(Boost_INCLUDE_DIRS "${DCC_BOOST_INCLUDE_DIRS}" CACHE INTERNAL "Boost include directories")
endif()

set(CONAN_HAS_FMT TRUE CACHE INTERNAL FALSE)
add_conan_lib(CONAN_PKG::fmt fmt/7.1.3)

if(USING_OPENSSL_NO_CONAN)
	if(NOT DEFINED OPENSSL_USE_STATIC_LIBS)
		set(OPENSSL_USE_STATIC_LIBS ON)
		set(OPENSSL_MSVC_STATIC_RT ON)
	endif()
	if(DEFINED OPENSSL_ROOT AND NOT DEFINED OPENSSL_ROOT_DIR)
		set(OPENSSL_ROOT_DIR "${OPENSSL_ROOT}")
	endif()
	find_package(OpenSSL)
	if (NOT OPENSSL_FOUND)
		set(OPENSSL_ROOT_DIR "${OPENSSL_ROOT_DIR}" CACHE PATH "Set the OpenSSL root folder.")
		mark_as_advanced(CLEAR OPENSSL_ROOT_DIR)
		message(FATAL_ERROR "Could NOT find OpenSSL; specify additional paths.")
	endif()
else()
	add_conan_lib(CONAN_PKG::openssl openssl/1.1.1k)
endif()

add_subdirectory(libs/log-core)
add_subdirectory(src)

if(DCC_CONAN_MSVC_VERSION_OVERRIDDEN)
	set(MSVC_VERSION "${DCC_ORIG_MSVC_VERSION}")
	unset(DCC_ORIG_MSVC_VERSION)
	unset(DCC_CONAN_MSVC_VERSION_OVERRIDDEN)
endif()

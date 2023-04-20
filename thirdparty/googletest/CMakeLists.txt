cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(3rd_googletest VERSION 1.0)

find_package(Git)
if (NOT Git_FOUND)
	message(FATAL_ERROR "Git not found, can't initialize!")
endif()

if (WIN32)
    set(GTEST_BRANCH release-1.8.0)
else()
    set(GTEST_BRANCH release-1.10.0)
endif()

#If clone is unsuccessful, try again several times
foreach(count RANGE 1 3)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} clone --depth 1 --branch ${GTEST_BRANCH} https://github.com/google/googletest.git ./googletest
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE result
	)
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest" AND IS_DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}/googletest")
		message(STATUS "Clone googletest repository successed...")
		break()
	endif()
	message(WARNING "CMake step [clone] for googletest, may be an error in cloning the repository, try again...")
endforeach()

if(result)
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
		message(FATAL_ERROR "CMake step [clone] for googletest, may be an error in cloning the repository: ${result}")
	endif()
endif()

set(THIRDPARTY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest)

set(INSTALL_GTEST OFF CACHE BOOL "Disable install googletest")

# By default, new Visual Studio projects link the C runtimes dynamically but
# Google Test links them statically. This will generate an error that looks
# something like the following: gtest.lib(gtest-all.obj) : error LNK2038: mismatch
# detected for 'RuntimeLibrary': value 'MTd_StaticDebug' doesn't match value
# 'MDd_DynamicDebug' in main.obj
#
# Google Test already has a CMake option for this: `gtest_force_shared_crt`
#
# Enabling this option will make gtest link the runtimes dynamically too, and
# match the project in which it is included.
if (WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)   
endif()

# error C2220: warning treated as error
# see : https://github.com/google/googletest/issues/1373
#
# `/w` disables all warnings. This is needed because `gtest` enables
# `/WX` (equivalent of `-Werror`) for some reason, breaking builds
# when MSVS adds new warnings.
if(MSVC)
    add_compile_options(/w)
endif()

add_subdirectory(${THIRDPARTY_SOURCE_DIR})

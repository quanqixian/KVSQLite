cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(KVSQLite VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(KVSQLITE_WITH_TESTS "Compile and run test executables" ON)
option(KVSQLITE_WITH_EXAMPLE "Compile examples" ON)
option(KVSQLITE_BUILD_SHARED_LIBS "Build lib as a shared library." ON)
option(KVSQLITE_BUILD_STATIC_LIBS "Build lib as a static library." ON)
option(KVSQLITE_BUILD_DOXYGEN_DOC "Generate API documentation using doxygen." ON)

add_subdirectory(thirdparty)
add_subdirectory(src)

if(KVSQLITE_WITH_TESTS)
	enable_testing()
	include(CTest)
	add_subdirectory(test)
endif()

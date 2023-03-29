cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(tests VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(KVSQLITE_TESTS_DIR
    test01
)

foreach(testDir ${KVSQLITE_TESTS_DIR})
	add_subdirectory(${testDir})
endforeach()

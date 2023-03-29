cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(thirdparty VERSION 1.0)

if(KVSQLITE_WITH_TESTS)
	add_subdirectory(googletest)
endif()

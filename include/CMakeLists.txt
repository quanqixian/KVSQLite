cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(KVSQLite_include VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/KVSQLite" TYPE INCLUDE)

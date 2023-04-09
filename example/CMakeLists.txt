cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Examples VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXAMPLES
    example01
    example02
)

foreach(example ${EXAMPLES})
	add_executable(${example} ${example}.cpp)
	target_include_directories(${example} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
	target_link_libraries(${example} KVSQLite)
	set_target_properties(${example} PROPERTIES INSTALL_RPATH "$ORIGIN;../lib")
	install(TARGETS ${example})
endforeach()

add_custom_target(examples ALL DEPENDS ${EXAMPLES})


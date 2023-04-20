cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(tests VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

function(AddCoverage target)
	if(NOT UNIX)
		message(STATUS "Will not build coverage report, only support in unix system")
        return()
	endif()

    find_program(LCOV_PATH lcov)
    if(${LCOV_PATH} STREQUAL "LCOV_PATH-NOTFOUND")
		message(STATUS "lcov not found, will not build coverage report.")
        return()
	endif()

    find_program(GENHTML_PATH genhtml)
    if(${GENHTML_PATH} STREQUAL "GENHTML_PATH-NOTFOUND")
		message(STATUS "genhtml not found, will not build coverage report.")
        return()
	endif()

    add_custom_target(coverage
        COMMENT "Running coverage for ${target}..."
		COMMAND ${LCOV_PATH} -d . --zerocounters
        COMMAND $<TARGET_FILE:${target}>
		COMMAND ${LCOV_PATH} -d . --capture -o coverage.info
        COMMAND ${LCOV_PATH} -r coverage.info '*gtest*' -o coverage.info
        COMMAND ${LCOV_PATH} -r coverage.info '*thirdparty*' -o coverage.info
        COMMAND ${LCOV_PATH} -r coverage.info '/usr/include/*' -o filtered.info
        COMMAND ${GENHTML_PATH} -o coverage filtered.info --legend
        COMMAND rm -rf coverage.info filtered.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction()

set(UNIT_TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/Status.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/DB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sqlite/sqlite3.c
	main.cpp
)

add_executable(unitTest ${UNIT_TEST_SOURCES})

#Compile with source code, defining KVSQLITE_BUILD_WITH_SOURCES macro will
# affect the definition of KVSQLITE_EXPORT macro.
target_compile_definitions(unitTest PRIVATE KVSQLITE_BUILD_WITH_SOURCES)

set_target_properties(unitTest PROPERTIES OUTPUT_NAME unitTest)
target_include_directories(unitTest PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sqlite
	)
target_link_libraries(unitTest gtest)
if (UNIX)
	target_link_libraries(unitTest ${CMAKE_DL_LIBS})
endif()

if(THREADS_HAVE_PTHREAD_ARG)
	set_property(TARGET unitTest PROPERTY COMPILE_OPTIONS "-pthread")
	set_property(TARGET unitTest PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
	target_link_libraries(unitTest "${CMAKE_THREAD_LIBS_INIT}")
endif()

if ((CMAKE_BUILD_TYPE STREQUAL Debug) AND UNIX)
	target_compile_options(unitTest PRIVATE --coverage)
	target_link_options(unitTest PUBLIC --coverage)
	add_custom_command(TARGET unitTest PRE_BUILD 
		COMMAND find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -exec rm {} +
		)
endif()

add_test(NAME unitTest COMMAND unitTest)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    AddCoverage(unitTest)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/coverage" TYPE DOC)
endif()

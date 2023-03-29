cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(libKVSQLite VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(KVSQLITE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sqlite/sqlite3.c
    Status.cpp
)

find_package(Threads REQUIRED)

if(KVSQLITE_BUILD_SHARED_LIBS)
	add_library(KVSQLite SHARED ${KVSQLITE_SOURCES})
	set_target_properties(KVSQLite PROPERTIES OUTPUT_NAME KVSQLite)

	target_include_directories(KVSQLite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sqlite/
	)
	if (UNIX)
		target_link_libraries(KVSQLite ${CMAKE_DL_LIBS})
    endif()

	if(THREADS_HAVE_PTHREAD_ARG)
		set_property(TARGET KVSQLite PROPERTY COMPILE_OPTIONS "-pthread")
		set_property(TARGET KVSQLite PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
		target_link_libraries(KVSQLite "${CMAKE_THREAD_LIBS_INIT}")
	endif()

	install(TARGETS KVSQLite)
endif()

if(KVSQLITE_BUILD_STATIC_LIBS)
	add_library(KVSQLite_static STATIC ${KVSQLITE_SOURCES})
	target_include_directories(KVSQLite_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/sqlite/
	)

    #On Windows you should probably give each library a different name, since there is a ".lib" file for both shared and static.
    #But on Linux and Mac you can even give both libraries the same name (e.g. libMyLib.a and libMyLib.so)
    if (WIN32)
        set_target_properties(KVSQLite_static PROPERTIES OUTPUT_NAME KVSQLite_static)
    else()
        set_target_properties(KVSQLite_static PROPERTIES OUTPUT_NAME KVSQLite)
		target_link_libraries(KVSQLite_static ${CMAKE_DL_LIBS})
    endif()

	if(THREADS_HAVE_PTHREAD_ARG)
		set_property(TARGET KVSQLite_static PROPERTY COMPILE_OPTIONS "-pthread")
		set_property(TARGET KVSQLite_static PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
		target_link_libraries(KVSQLite_static "${CMAKE_THREAD_LIBS_INIT}")
	endif()

	install(TARGETS KVSQLite_static)
endif()

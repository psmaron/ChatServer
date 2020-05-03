cmake_minimum_required(VERSION 3.11.4)
project(
        ChatServer
        VERSION 0.0.1
        LANGUAGES CXX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(BOOST_MIN_VERSION 1.66)

set(GTEST_INCLUDE_DIR include/googletest_v1.10.0/)
set(GTEST_MAIN_DIR include/googletest_v1.10.0/)
set(GMOCK_INCLUDE_DIR include/googletest_v1.10.0/)

set(GTEST_LIBRARY lib64/googletest_v1.10.0/)
set(GTEST_MAIN_LIBRARY lib64/googletest_v1.10.0/)
set(GMOCK_LIBRARY lib64/googletest_v1.10.0/)
set(GMOCK_MAIN_LIBRARY lib64/googletest_v1.10.0/)

set(BOOST_LIBRARYDIR lib64/boost_v1.69.0/)
set(BOOST_INCLUDEDIR include/boost_v1.69.0/)

set(OZO_INCLUDE include/ozo_v1/)
set(RESOURCE_POOL_INCLUDE include/resource_pool_v1/)
set(SPAWN_INCLUDE include/spawn_v1/)

set(LOG4CPP_INCLUDE include/log4cpp_v1.1.3/)
set(LOG4CPP_LIBRARY lib64/log4cpp_v1.1.3/)

set(BACKWARDCPP_INCLUDE include/backward-cpp/)
set(BACKWARDCPP_SRC include/backward.cpp)

set(DWARF_INCLUDE /usr/include/libdwarf/)

find_package(PostgreSQL 9.3.0 REQUIRED)
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED)

set(ChatServer_LIBS
        stdc++fs
        boost_system
        boost_context
        boost_locale
        boost_program_options
        pthread
        log4cpp
        dl
        dw
        pqxx
        PostgreSQL::PostgreSQL)
set(ChatServer_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ChatServer_MAIN ${ChatServer_SRC_DIR}/main.cpp)
set(INSTALL_DESTINATION /usr/local/ChatServer)

add_compile_definitions(BOOST_HANA_CONFIG_ENABLE_STRING_UDL)
add_compile_definitions(BOOST_COROUTINES_NO_DEPRECATION_WARNING)


include_directories(SYSTEM ${OZO_INCLUDE} ${SPAWN_INCLUDE} ${RESOURCE_POOL_INCLUDE} ${LOG4CPP_INCLUDE})
include_directories(SYSTEM ${Boost_INCLUDE_DIR} ${PostgreSQL_INCLUDE_DIRS})
link_directories(${LOG4CPP_LIBRARY} ${BOOST_LIBRARYDIR})
add_definitions(-DBOOST_PROPERTY_TREE_RAPIDXML_STATIC_POOL_SIZE=512)


option(COVERAGE_ANALYSIS_BUILD "Compile with support for gcov coverage analysis" OFF)
if (COVERAGE_ANALYSIS_BUILD)
    message("Build for COVERAGE ANALYSIS")
    list(APPEND ChatServer_LIBS gcov)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_compile_options(-Wall -Wextra -Woverloaded-virtual -Wsign-conversion -Wduplicated-cond -Wuseless-cast -O0)
else ()
    message("Forcing build WITHOUT OPTIMIZATION for coverage testing")
    # TODO: Optimizations should be set to at least -O1 lvl, but we can test -O2 for production as well
    add_compile_options(-Wall -Wextra -Woverloaded-virtual -Wsign-conversion -Wduplicated-cond -Wduplicated-branches
            -Wvla -pipe -Wuseless-cast -Wnull-dereference -Wswitch-enum -O0)
endif ()

add_compile_definitions(BACKWARD_HAS_DW=1)
add_compile_options(-g)

# GTEST
option(PACKAGE_TESTS "Build the tests" ON)
if (PACKAGE_TESTS)
    message("UT ON")
    add_compile_definitions(TVIRTUAL=virtual)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(test)
else ()
    message("UT OFF")
    add_compile_definitions(TVIRTUAL=)
endif ()

add_subdirectory(src)

add_subdirectory(dscheck)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/logger.config $ENV{PWD}/src COPYONLY)

install(DIRECTORY config/ DESTINATION ${INSTALL_DESTINATION} PATTERN *.config)


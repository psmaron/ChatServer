cmake_minimum_required(VERSION 3.11.4)
set(TEST_LIBS gtest gmock gtest_main)
set(ChatServer_TEST_DIR ${CMAKE_SOURCE_DIR}/test)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# keeps CACHE cleaner
mark_as_advanced(
        BUILD_GMOCK
        BUILD_GTEST
        BUILD_SHARED_LIBS
        gmock_build_tests
        gtest_build_samples
        gtest_build_tests
        gtest_disable_pthreads
        gtest_force_shared_crt
        gtest_hide_internal_symbols)

function(add_unit_test)
    set(options "")
    set(oneValueArgs TEST_NAME)
    set(multiValueArgs TEST_SRC)
    cmake_parse_arguments(add_test "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set_source_files_properties(${BACKWARDCPP_SRC} PROPERTIES COMPILE_FLAGS "-w")
    include_directories(${ChatServer_TEST_DIR} SYSTEM ${GTEST_MAIN_DIR} ${GTEST_MAIN_LIBRARY} ${DWARF_INCLUDE})
    link_directories(${GTEST_MAIN_LIBRARY})
    add_executable(${add_test_TEST_NAME} ${add_test_TEST_SRC})

    target_link_libraries(${add_test_TEST_NAME} ${TEST_LIBS} ${PROJECT_NAME}_lib ${ChatServer_LIBS})
    target_include_directories(${add_test_TEST_NAME} PRIVATE ${ChatServer_SRC_DIR})

    set_target_properties(${add_test_TEST_NAME} PROPERTIES CXX_STANDARD 17)
    add_test(${add_test_TEST_NAME} ${add_test_TEST_NAME})
endfunction()

file(GLOB TEST_SRC_EXCLUDED ${ChatServer_TEST_DIR}/manual_tests/*.cpp)
file(GLOB_RECURSE TESTS_SRC ${ChatServer_TEST_DIR}/*.cpp)
list(REMOVE_ITEM TESTS_SRC ${TEST_SRC_EXCLUDED})
list(APPEND TESTS_SRC ${BACKWARDCPP_SRC})

add_unit_test(TEST_NAME ${PROJECT_NAME}_tests TEST_SRC ${TESTS_SRC})

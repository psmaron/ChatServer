file(GLOB_RECURSE ChatServer_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM ChatServer_SRC ${ChatServer_MAIN})

if (Boost_FOUND AND PostgreSQL_FOUND)
    add_executable(${PROJECT_NAME} ${ChatServer_MAIN} ${BACKWARDCPP_SRC})
    add_library(${PROJECT_NAME}_lib STATIC ${ChatServer_SRC})
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib ${ChatServer_LIBS})
    set_source_files_properties(${BACKWARDCPP_SRC} PROPERTIES COMPILE_FLAGS "-w")

    include_directories(${ChatServer_SRC_DIR} ${DWARF_INCLUDE})
    # INTERPROCEDURAL_OPTIMIZATION TRUE
    set_target_properties(
            ${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
    set_target_properties(
            ${PROJECT_NAME}_lib PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)

    install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DESTINATION})
endif ()

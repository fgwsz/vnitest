cmake_minimum_required(VERSION 3.0)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
# include
    #include_directories(include)
    #aux_source_directory(source/${PROJECT_NAME} SOURCE_DIR)
# single-include
    include_directories(single-include)
    aux_source_directory(single-include/${PROJECT_NAME} SOURCE_DIR)
# tests
include_directories(tests)
aux_source_directory(tests TESTS_DIR)
add_executable(${PROJECT_NAME} ${SOURCE_DIR} ${TESTS_DIR})
#msvc set utf-8
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif()

cmake_minimum_required(VERSION 3.0)

# 获取项目名称
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 设置源文件
file(GLOB LIB_SOURCES "source/${PROJECT_NAME}/*.cpp")

# 创建静态库
add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# 创建共享库
add_library(${PROJECT_NAME}_shared SHARED ${LIB_SOURCES})
set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

# 设置安装路径
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install CACHE PATH "Installation Directory")

# 设置安装规则
install(TARGETS ${PROJECT_NAME}_static ${PROJECT_NAME}_shared
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/
        DESTINATION include/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# 创建安装后打包目标
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} --install . --prefix ${CMAKE_CURRENT_BINARY_DIR}/dist
    COMMAND tar -czvf ${PROJECT_NAME}-${PROJECT_VERSION}.tar.gz -C ${CMAKE_CURRENT_BINARY_DIR}/dist include lib
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Creating distribution package"
)

# 平台特定设置
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
endif()

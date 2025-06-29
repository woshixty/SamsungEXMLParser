cmake_minimum_required(VERSION 3.16)
project(SamsungEXMLParser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找tinyxml2
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(TINYXML2 tinyxml2)
endif()

if(NOT TINYXML2_FOUND)
    # 如果没有找到系统安装的tinyxml2，尝试使用FetchContent
    include(FetchContent)
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 9.0.0
    )
    FetchContent_MakeAvailable(tinyxml2)
    set(TINYXML2_LIBRARIES tinyxml2::tinyxml2)
    set(TINYXML2_INCLUDE_DIRS ${tinyxml2_SOURCE_DIR})
endif()

# 创建动态库
add_library(exml_parser SHARED
    src/exml_parser.cpp
)

# 设置包含目录
target_include_directories(exml_parser
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${TINYXML2_INCLUDE_DIRS}
)

# 链接tinyxml2
target_link_libraries(exml_parser PRIVATE ${TINYXML2_LIBRARIES})

# 设置库的属性
set_target_properties(exml_parser PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/exml_parser.h
)

# 创建测试可执行文件
add_executable(test_exml_parser tests/test_main.cpp)
target_link_libraries(test_exml_parser PRIVATE exml_parser)

# 启用测试
enable_testing()
add_test(NAME test_exml_parser COMMAND test_exml_parser)

# 安装规则
install(TARGETS exml_parser
    EXPORT exml_parserTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(FILES include/exml_parser.h
    DESTINATION include
)

# 导出配置
install(EXPORT exml_parserTargets
    FILE exml_parserTargets.cmake
    NAMESPACE exml_parser::
    DESTINATION lib/cmake/exml_parser
)

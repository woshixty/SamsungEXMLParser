cmake_minimum_required(VERSION 3.16)
project(SamsungEXMLParser VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找tinyxml2 - 优先使用系统安装的版本
find_package(tinyxml2 QUIET)
if(tinyxml2_FOUND)
    message(STATUS "Found system tinyxml2")
    set(TINYXML2_LIBRARIES tinyxml2::tinyxml2)
    set(TINYXML2_INCLUDE_DIRS "")
else()
    # 尝试使用pkg-config
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(TINYXML2 tinyxml2)
    endif()
    
    # 如果仍然没有找到，使用本地的tinyxml2源码
    if(NOT TINYXML2_FOUND)
        message(STATUS "tinyxml2 not found, using local tinyxml2 source...")
        
        # 检查本地tinyxml2源码是否存在
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyxml2/CMakeLists.txt)
            message(STATUS "Found local tinyxml2 at libs/tinyxml2")
            add_subdirectory(libs/tinyxml2)
            set(TINYXML2_LIBRARIES tinyxml2)
            set(TINYXML2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyxml2)
        else()
            message(FATAL_ERROR "Local tinyxml2 not found at libs/tinyxml2. Please ensure the tinyxml2 source code is available in the libs/tinyxml2 directory.")
        endif()
    endif()
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

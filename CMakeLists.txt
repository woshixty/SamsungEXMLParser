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
    
    # 如果仍然没有找到，尝试手动下载
    if(NOT TINYXML2_FOUND)
        message(STATUS "tinyxml2 not found, attempting to download...")
        include(FetchContent)
        
        # 设置网络选项
        set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
        set(FETCHCONTENT_QUIET OFF)
        
        # 尝试使用URL下载
        FetchContent_Declare(
            tinyxml2
            URL https://github.com/leethomason/tinyxml2/archive/refs/tags/9.0.0.tar.gz
            URL_HASH SHA256=cc2f1417c308b1f6acc54f88eb70771a0bf65f76282ce5c40e54cfe52952702c
        )
        
        FetchContent_GetProperties(tinyxml2)
        if(NOT tinyxml2_POPULATED)
            FetchContent_Populate(tinyxml2)
        endif()
        
        if(tinyxml2_POPULATED)
            set(TINYXML2_LIBRARIES tinyxml2)
            set(TINYXML2_INCLUDE_DIRS ${tinyxml2_SOURCE_DIR})
            add_subdirectory(${tinyxml2_SOURCE_DIR} ${tinyxml2_BINARY_DIR})
        else()
            message(FATAL_ERROR "Failed to download tinyxml2. Please install it manually:")
            message(STATUS "1. vcpkg install tinyxml2")
            message(STATUS "2. Download from https://github.com/leethomason/tinyxml2")
            message(STATUS "3. Set TINYXML2_DIR to point to tinyxml2 installation")
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

#[[
  CMakeLists for PeripheralSuite_c
   Created on: 2026_06_1
       Author: Rev_RoastDuck
       Github: https://github.com/Rev-RoastedDuck
  
  :copyright: (c) 2023 by Rev-RoastedDuck.
]]

#[[----------------- 基础配置信息 --------------------
=====================================================]]
cmake_minimum_required(VERSION 3.0)
project(PeripheralSuite_c C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")

#[[------------------ 定义过滤列表 ---------------------
=====================================================]]
set(FILTER_KEYWORDS
    "CMakeFiles"
    "Test"
    "build"
)

#[[------------------ 搜集源文件 ---------------------
=====================================================]]
set(SRC_LIST)
list(APPEND SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/interface/peripheral_suite.c)

#[[------------------ 搜集头文件 ---------------------
=====================================================]]
set(INC_LIST)
list(APPEND INC_LIST ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND INC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src)
list(APPEND INC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/interface)
list(APPEND INC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/stdkit/stdkit)

#[[------------------ 添加子模块 ---------------------
=====================================================]]
# add_subdirectory(include/AlgorithmSuite/AlgorithmSuite)

#[[------------------ 创建静态库 -------------------
=====================================================]]
add_library(peripheralsuite STATIC ${SRC_LIST})

#[[------------------ 配置头文件路径 --------------------
=====================================================]]
target_include_directories(peripheralsuite PUBLIC
    ${INC_LIST}
)

#[[------------------ 添加依赖库 -------------------
=====================================================]]
target_link_libraries(peripheralsuite 
    PUBLIC algorithmsuite
)

if(UNIX AND NOT APPLE)
    target_link_libraries(peripheralsuite PRIVATE m)
endif()
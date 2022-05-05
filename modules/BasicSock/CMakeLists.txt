cmake_minimum_required(VERSION 3.18)

project(BasicSock 
    VERSION 0.1.0 
    DESCRIPTION "basic socket communcation." 
    # LANGUAGES CXX
)

if (MSVC)
    # 设置 msvc 代码编码格式为 utf-8
    set(CMAKE_C_FLAGS "/source-charset:utf-8 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "/source-charset:utf-8 ${CMAKE_CXX_FLAGS}")
    message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif()

message(STATUS "${PROJECT_NAME} status:")
message(STATUS "    version: ${${PROJECT_NAME}_VERSION}")
message(STATUS "    description: ${${PROJECT_NAME}_DESCRIPTION}")
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install) # 安装目录
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

########################################################################################
## socket server & client
message(STATUS "socket server & client")
add_compile_definitions(WIN32_LEAN_AND_MEAN)
add_executable(client client.cpp)
add_executable(server server.cpp)

install(TARGETS client DESTINATION bin)
install(TARGETS server DESTINATION bin)





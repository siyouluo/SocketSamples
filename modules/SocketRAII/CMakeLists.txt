cmake_minimum_required(VERSION 3.18)

project(SocketRAII 
    VERSION 0.1.0 
    DESCRIPTION "Socket Class (follow the RAII idiom)." 
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
## WinSock2
set(WinSock2_LIBS WS2_32)
add_compile_definitions(WIN32_LEAN_AND_MEAN)

########################################################################################
## ServerSocket
message(STATUS "ServerSocket")
# add files
include_directories("${PROJECT_SOURCE_DIR}/ServerSocket")
file(GLOB ServerSocket_SRCS "${PROJECT_SOURCE_DIR}/ServerSocket/ServerSocket.*")
message(STATUS "ServerSocket_SRCS=${ServerSocket_SRCS}") # 在配置的时候打印输出变量ServerSocket_SRCS
# add library
add_library(ServerSocket STATIC ${ServerSocket_SRCS}) # 生成静态库
set(ServerSocket_LIBS ServerSocket)
# add target
add_executable(devel_ServerSocket "${PROJECT_SOURCE_DIR}/ServerSocket/test_ServerSocket.cpp" ${ServerSocket_SRCS})
target_link_libraries(devel_ServerSocket ${WinSock2_LIBS})

add_executable(test_ServerSocket "${PROJECT_SOURCE_DIR}/ServerSocket/test_ServerSocket.cpp")
target_link_libraries(test_ServerSocket ${ServerSocket_LIBS} ${WinSock2_LIBS})
# install
install(TARGETS test_ServerSocket DESTINATION bin)
install(FILES "${PROJECT_SOURCE_DIR}/ServerSocket/ServerSocket.h" DESTINATION include)
install(TARGETS ${ServerSocket_LIBS} DESTINATION lib)

########################################################################################
## ClientSocket
message(STATUS "ClientSocket")
# add files
include_directories("${PROJECT_SOURCE_DIR}/ClientSocket")
file(GLOB ClientSocket_SRCS "${PROJECT_SOURCE_DIR}/ClientSocket/ClientSocket.*")
message(STATUS "ClientSocket_SRCS=${ClientSocket_SRCS}") # 在配置的时候打印输出变量ClientSocket_SRCS
# add library
add_library(ClientSocket STATIC ${ClientSocket_SRCS}) # 生成静态库
set(ClientSocket_LIBS ClientSocket)
# add target
add_executable(devel_ClientSocket "${PROJECT_SOURCE_DIR}/ClientSocket/test_ClientSocket.cpp" ${ClientSocket_SRCS})
target_link_libraries(devel_ClientSocket ${WinSock2_LIBS})

add_executable(test_ClientSocket "${PROJECT_SOURCE_DIR}/ClientSocket/test_ClientSocket.cpp")
target_link_libraries(test_ClientSocket ${ClientSocket_LIBS} ${WinSock2_LIBS})
# install
install(TARGETS test_ClientSocket DESTINATION bin)
install(FILES "${PROJECT_SOURCE_DIR}/ClientSocket/ClientSocket.h" DESTINATION include)
install(TARGETS ${ClientSocket_LIBS} DESTINATION lib)



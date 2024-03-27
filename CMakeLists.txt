cmake_minimum_required(VERSION 2.8)
project(demo_start)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(SRC_LIST "src/main.c" "src/client1.c")
set(LIBUV_PATH external/libuv)

include_directories(include ${LIBUV_PATH}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/libuv)


add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} uv)

# windows
if (MSVC)
    # set(UV_DEBUG_DLL_FILE ${LIBRARY_OUTPUT_PATH}/Debug/sublib.dll)
    # set(UV_RELEASE_DLL_FILE ${LIBRARY_OUTPUT_PATH}/Release/sublib.dll)
    # # 自定义复制 dll 文件
    # add_custom_command(TARGET ${PROJECT_NAME}
    #     POST_BUILD # 目标中的其他规则都已经执行后台运行
    # )
    # ......
endif()

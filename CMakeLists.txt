cmake_minimum_required(VERSION 3.5)

project(ipc_app)

set (CMAKE_C_COMPILER ${PROJECT_SOURCE_DIR}/prebuilt/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc)
set (CMAKE_CXX_COMPILER ${PROJECT_SOURCE_DIR}/prebuilt/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory (src SRCS)
include_directories (include)
link_directories(${PROJECT_SOURCE_DIR}/libs)

add_executable(ipc_app ${SRCS})
target_link_libraries(ipc_app pthread rockit rockchip_mpp rkaiq aec_bf_process rkaudio_common m freetype rockiva iconv rknnmrt rga stdc++ rksysutils app_shared)

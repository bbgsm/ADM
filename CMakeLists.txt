cmake_minimum_required(VERSION 3.28)
project(ADM)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build for linux, Only Test, Incomplete
#add_definitions(-DLINUX)

set(CMAKE_CXX_STANDARD 20)
# 设置输出路径为项目根目录的out
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

add_subdirectory(libhv)

include_directories(.)
include_directories(MemTools)
include_directories(MemTools/dmaLibs/include)
include_directories(imgui)
include_directories(imgui/backends)
include_directories(cheat)
include_directories(render)
include_directories(CEPlugin)
include_directories(CEPlugin/Memory)
include_directories(CEPlugin/Hooks)
include_directories(CEPlugin/CheatEngine)
include_directories(httpserver/include)
include_directories(tools/include)
include_directories(libhv/include)
include_directories(libhv/include/hv)
include_directories(dmaLibs/include)

file(GLOB_RECURSE RENDER_SOURCES "render/*.cpp" "render/*.h")
file(GLOB_RECURSE CE_PLUGIN_SOURCES "CEPlugin/*.cpp" "CEPlugin/*.h" "CEPlugin/*.hpp" "CEPlugin/*.def" "cheat/*.cpp" "cheat/*.hpp")
file(GLOB_RECURSE CHEAT_SRC_SOURCES "cheat/*.cpp" "cheat/*.hpp" "cheat/*.h")
file(GLOB_RECURSE TEST_GUI_SRC_SOURCES "testGui/*.cpp" "testGui/*.hpp" "testGui/*.h")
file(GLOB_RECURSE IMGUI_SOURCES
        "imgui/imgui.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_widgets.cpp"
        "imgui/backends/imgui_impl_dx11.cpp"
        "imgui/backends/imgui_impl_win32.cpp"
)
file(GLOB_RECURSE MEMORY_TOOLS_SOURCES
        "MemTools/DirectMemoryTools.cpp"
        "MemTools/DmaMemoryTools.cpp"
        "MemTools/DumpMemoryTools.cpp"
        "MemTools/MemoryToolsBase.cpp"
        "MemTools/*.h"
)

add_executable(ADM
        ${MEMORY_TOOLS_SOURCES}
        ${RENDER_SOURCES}
        ${IMGUI_SOURCES}
        ${CHEAT_SRC_SOURCES}
        ${HTTPSERVER_SOURCES}
        ${TOOLS_SOURCES}
)

add_library(ADM_CE_Plugin SHARED
        ${MEMORY_TOOLS_SOURCES}
        ${RENDER_SOURCES}
        ${IMGUI_SOURCES}
        ${CE_PLUGIN_SOURCES}
)

add_executable(testGui
        ${IMGUI_SOURCES}
        ${RENDER_SOURCES}
        ${TEST_GUI_SRC_SOURCES}
)

add_executable(blockScreen
        blockScreen/main.cpp
)

add_executable(test
        test.cpp
        ${MEMORY_TOOLS_SOURCES}
        ${RENDER_SOURCES}
        ${IMGUI_SOURCES}
        ${HTTPSERVER_SOURCES}
        ${TOOLS_SOURCES}
)

target_link_options(blockScreen PRIVATE "/SUBSYSTEM:WINDOWS")

target_link_libraries(ADM
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/leechcore.lib"
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/vmm.lib"
        ws2_32
        d3d11
        hv_static
)

target_link_libraries(test
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/leechcore.lib"
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/vmm.lib"
        ws2_32
        d3d11
        hv_static
)

target_link_libraries(ADM_CE_Plugin
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/leechcore.lib"
        "${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/win32/vmm.lib"
        ws2_32
        d3d11
        hv_static
)

target_link_libraries(testGui
        ws2_32
        d3d11
)

########### 文件复制 #############
# 无论如何设置都获取不到正确的编译输出目录，只能手动拼接剩下的路径(Release/Debug)
file(COPY "webMap" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
file(COPY "webMap" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")

file(GLOB DATA_FILES "MemTools/dmaLibs/win32/*.dll" "MemTools/dmaLibs/win32/*.db")

# 遍历文件列表并复制每个文件
foreach (DATA_FILE ${DATA_FILES})
    file(COPY ${DATA_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
    file(COPY ${DATA_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
endforeach ()
########### 文件复制 #############
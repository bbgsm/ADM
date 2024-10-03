cmake_minimum_required(VERSION 3.28)
project(ADM)
if (UNIX) # Linux
    add_definitions(-DLINUX)
elseif (WIN32) # Windows MSVC参数
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif ()

set(CMAKE_CXX_STANDARD 20)

if (UNIX) # Linux
    # 设置输出路径为项目根目录的out
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE}")

elseif (WIN32) # Windows
    # 设置输出路径为项目根目录的out
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")
endif ()

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
include_directories(kmBox)

file(GLOB_RECURSE RENDER_SOURCES "render/*.cpp" "render/*.h")
file(GLOB_RECURSE CE_PLUGIN_SOURCES "CEPlugin/*.cpp" "CEPlugin/*.h" "CEPlugin/*.hpp" "CEPlugin/*.def" "cheat/*.cpp" "cheat/*.hpp")
file(GLOB_RECURSE CHEAT_SRC_SOURCES "cheat/*.cpp" "cheat/*.hpp" "cheat/*.h")
file(GLOB_RECURSE TEST_GUI_SRC_SOURCES "testGui/*.cpp" "testGui/*.hpp" "testGui/*.h")
file(GLOB_RECURSE KMBOX_SOURCES "kmBox/*.cpp" "kmBox/*.hpp" "kmBox/*.h")

if (UNIX) # Linux
    file(GLOB IMGUI_SOURCES
            "imgui/*.cpp"
            "imgui/backends/imgui_impl_glfw.cpp"
            "imgui/backends/imgui_impl_opengl3.cpp"
    )
elseif (WIN32) # Windows MSVC参数
    file(GLOB_RECURSE IMGUI_SOURCES
            "imgui/imgui.cpp"
            "imgui/imgui_draw.cpp"
            "imgui/imgui_tables.cpp"
            "imgui/imgui_widgets.cpp"
            "imgui/backends/imgui_impl_dx11.cpp"
            "imgui/backends/imgui_impl_win32.cpp"
    )
endif ()

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
        ${KMBOX_SOURCES}
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
        cheat/Game.cpp
        ${MEMORY_TOOLS_SOURCES}
        ${RENDER_SOURCES}
        ${IMGUI_SOURCES}
        ${HTTPSERVER_SOURCES}
        ${TOOLS_SOURCES}
)


if (UNIX)
    # vmm.so
    add_library(vmm SHARED IMPORTED)
    set_target_properties(vmm PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/linuxArm64/vmm.so)
    # leechcore.so
    add_library(leechcore SHARED IMPORTED)
    set_target_properties(leechcore PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/MemTools/dmaLibs/linuxArm64/leechcore.so)

    target_link_libraries(ADM
            vmm
            leechcore
            hv_static
            GL glfw X11 Xext
    )
    target_link_libraries(test
            vmm
            leechcore
            hv_static
    )

    target_link_libraries(ADM_CE_Plugin
            vmm
            leechcore
            hv_static
    )

    target_link_libraries(testGui
            GL glfw X11 Xext
    )
elseif (WIN32)

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
endif ()


########### 文件复制 #############
# 架构判断
if (UNIX) # Linux
    file(COPY "webMap" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")  # x64架构
        file(GLOB DATA_FILES "MemTools/dmaLibs/linuxX64/*.so" "MemTools/dmaLibs/linuxX64/*.db")
    elseif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64") # arm64架构
        file(GLOB DATA_FILES "MemTools/dmaLibs/linuxArm64/*.so" "MemTools/dmaLibs/linuxArm64/*.db")
    endif ()
    # 遍历文件列表并复制每个文件
    foreach (DATA_FILE ${DATA_FILES})
        # 无论如何设置都获取不到正确的编译输出目录，只能手动拼接剩下的路径
        file(COPY ${DATA_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    endforeach ()
elseif (WIN32) # Windows
    # 无论如何设置都获取不到正确的编译输出目录，只能手动拼接剩下的路径(Release/Debug)
    file(COPY "webMap" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
    file(COPY "webMap" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
    file(GLOB DATA_FILES "MemTools/dmaLibs/win32/*.dll" "MemTools/dmaLibs/win32/*.db")
    # 遍历文件列表并复制每个文件
    foreach (DATA_FILE ${DATA_FILES})
        file(COPY ${DATA_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
        file(COPY ${DATA_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
    endforeach ()
endif ()
########### 文件复制 #############

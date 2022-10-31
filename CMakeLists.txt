cmake_minimum_required(VERSION 3.13)
set (CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME KeyboardLatencyTester)
project(${PROJECT_NAME})

set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/imgui)
message(${IMGUI_SOURCE_DIR})

set(ATOMIC_QUEUE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/atomic_queue)

# atomic_queue
set(ATOMIC_QUEUE_HDR_DIR ${ATOMIC_QUEUE_SOURCE_DIR}/include/atomic_queue)

set(ATOMIC_QUEUE_PUBLIC_HDRS
        ${ATOMIC_QUEUE_HDR_DIR}/atomic_queue_mutex.h
        ${ATOMIC_QUEUE_HDR_DIR}/atomic_queue.h
        ${ATOMIC_QUEUE_HDR_DIR}/barrier.h
        ${ATOMIC_QUEUE_HDR_DIR}/defs.h
        ${ATOMIC_QUEUE_HDR_DIR}/spinlock.h)

add_library(Atomic_queue INTERFACE)

target_sources(Atomic_queue INTERFACE ${ATOMIC_QUEUE_PUBLIC_HDRS})
target_include_directories(Atomic_queue INTERFACE
        $<BUILD_INTERFACE:${ATOMIC_QUEUE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )


# ImGui
if (EXISTS ${IMGUI_SOURCE_DIR}/imconfig.h)
    file(RENAME
            ${IMGUI_SOURCE_DIR}/imconfig.h
            ${IMGUI_SOURCE_DIR}/imconfig.h.bak)
endif ()

file(COPY ${CMAKE_CURRENT_LIST_DIR}/imconfig.h DESTINATION ${IMGUI_SOURCE_DIR})

set(IMGUI_PUBLIC_HDRS
        ${IMGUI_SOURCE_DIR}/imgui.h
        ${IMGUI_SOURCE_DIR}/imgui_internal.h
        ${IMGUI_SOURCE_DIR}/imstb_rectpack.h
        ${IMGUI_SOURCE_DIR}/imstb_textedit.h
        ${IMGUI_SOURCE_DIR}/imstb_truetype.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx12.h
        )

set(IMGUI_SRCS
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
        )

add_library(ImGui INTERFACE)
target_sources(ImGui INTERFACE ${IMGUI_SRCS} ${IMGUI_PUBLIC_HDRS})
target_include_directories(ImGui INTERFACE
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}/backends>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

add_executable(${PROJECT_NAME} src/main.cpp src/ui.cpp src/input.cpp)
target_link_libraries(KeyboardLatencyTester ImGui)
target_link_libraries(KeyboardLatencyTester Atomic_queue)
target_link_libraries(${PROJECT_NAME} "d3d12.lib" "dxgi.lib" "d3dcompiler.lib") 
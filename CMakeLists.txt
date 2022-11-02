cmake_minimum_required(VERSION 3.13)
set (CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME KeyboardLatencyTester)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/buildsystems/cmake"
)

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


find_package(ImGui)

add_library(ImGui INTERFACE)
target_sources(ImGui INTERFACE ${IMGUI_SRCS} ${IMGUI_PUBLIC_HDRS})
target_include_directories(ImGui INTERFACE
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}/backends>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

set(OUTPUT_NAME_DX12 ${PROJECT_NAME}D3D12)
add_executable(${OUTPUT_NAME_DX12} src/main.cpp src/ui.cpp src/input.cpp src/session.cpp)
target_link_libraries(${OUTPUT_NAME_DX12} ImGui)
target_link_libraries(${OUTPUT_NAME_DX12} Atomic_queue)
target_link_libraries(${OUTPUT_NAME_DX12} "d3d12.lib" "dxgi.lib" "d3dcompiler.lib") 

find_package(DirectX11)
add_executable(dx11test ${IMGUI_SOURCE_DIR}/examples/example_win32_directx11/main.cpp)
target_link_libraries(dx11test ImGui)
target_link_libraries(dx11test ${DirectX11_LIBRARY})

cmake_minimum_required(VERSION 3.13)
set (CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME KeyboardLatencyTester)
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_LIST_DIR}/buildsystems/cmake"
)

set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/imgui)
set(IMPLOT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/external/implot)

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
find_package(ImPlot)

if (WIN32)
    set(OUTPUT_NAME_DX12 ${PROJECT_NAME}D3D12)
    add_executable(${OUTPUT_NAME_DX12} src/main_d3d12.cpp src/component/session.cpp src/imcontext.cpp src/imcontext.hpp)
    target_link_libraries(${OUTPUT_NAME_DX12} ImGui)
    target_link_libraries(${OUTPUT_NAME_DX12} ImPlot)
    target_link_libraries(${OUTPUT_NAME_DX12} Atomic_queue)
    target_link_libraries(${OUTPUT_NAME_DX12} "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")

    find_package(DirectX11)
    set(OUTPUT_NAME_DX11 ${PROJECT_NAME}D3D11)
    add_executable(${OUTPUT_NAME_DX11} src/main_d3d11.cpp src/component/session.cpp src/imcontext.cpp src/imcontext.hpp)
    target_link_libraries(${OUTPUT_NAME_DX11} ImGui)
    target_link_libraries(${OUTPUT_NAME_DX11} ImPlot)
    target_link_libraries(${OUTPUT_NAME_DX11} Atomic_queue)
    target_link_libraries(${OUTPUT_NAME_DX11} ${DirectX11_LIBRARY})
endif()

#add_executable(ImPlotDemo ${IMPLOT_SOURCE_DIR}/implot_demo.cpp)
#target_link_libraries(ImPlotDemo ImGui)
#target_link_libraries(ImPlotDemo ImPlot)
#target_link_libraries(ImPlotDemo ${DirectX11_LIBRARY})
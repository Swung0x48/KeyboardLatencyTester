cmake_minimum_required(VERSION 3.16)
set (CMAKE_CXX_STANDARD 17)
set(PROJECT_NAME KeyboardLatencyTester)
project(${PROJECT_NAME})

if (APPLE)
    enable_language(OBJC)
    enable_language(OBJCXX)
endif()

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


find_package(ImGui REQUIRED)
find_package(ImPlot REQUIRED)

set(KBLT_SRC 
    src/imcontext.cpp src/imcontext.hpp
    src/component/session.cpp
    src/component/comparison.cpp)

if (WIN32)
    set(OUTPUT_NAME_DX12 ${PROJECT_NAME}D3D12)
    add_executable(${OUTPUT_NAME_DX12} src/main_d3d12.cpp 
        ${KBLT_SRC})
    target_link_libraries(${OUTPUT_NAME_DX12} ImGui)
    target_link_libraries(${OUTPUT_NAME_DX12} ImPlot)
    target_link_libraries(${OUTPUT_NAME_DX12} Atomic_queue)
    target_link_libraries(${OUTPUT_NAME_DX12} "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")

    find_package(DirectX11)
    set(OUTPUT_NAME_DX11 ${PROJECT_NAME}D3D11)
    add_executable(${OUTPUT_NAME_DX11} src/main_d3d11.cpp 
        ${KBLT_SRC})
    target_link_libraries(${OUTPUT_NAME_DX11} ImGui)
    target_link_libraries(${OUTPUT_NAME_DX11} ImPlot)
    target_link_libraries(${OUTPUT_NAME_DX11} Atomic_queue)
    target_link_libraries(${OUTPUT_NAME_DX11} ${DirectX11_LIBRARY})
endif()

#find_package(glfw3)

if (APPLE)
    find_package(glfw3 REQUIRED)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(APPKIT_LIBRARY AppKit)
    find_library(COREDATA_LIBRARY CoreData)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    message(STATUS Cocoa.framework=${COCOA_LIBRARY})
    message(STATUS Metal.framework=${METAL_LIBRARY})
    message(STATUS AppKit=${APPKIT_LIBRARY})
    message(STATUS CoreData=${COREDATA_LIBRARY})
    message(STATUS Foundation=${FOUNDATION_LIBRARY})
    message(STATUS QuartzCore=${QUARTZCORE_LIBRARY})
    set(OUTPUT_NAME_GLFW_METAL ${PROJECT_NAME}GlfwMetal)
    add_executable(${OUTPUT_NAME_GLFW_METAL} MACOSX_BUNDLE
                src/main_glfw_metal.mm
                src/component/session.cpp
                src/imcontext.cpp
                src/imcontext.hpp)
    target_link_libraries(${OUTPUT_NAME_GLFW_METAL} ImGui)
    target_link_libraries(${OUTPUT_NAME_GLFW_METAL} ImPlot)
    target_link_libraries(${OUTPUT_NAME_GLFW_METAL} Atomic_queue)
    target_link_libraries(${OUTPUT_NAME_GLFW_METAL} glfw)
    target_link_libraries(${OUTPUT_NAME_GLFW_METAL}
            ${COCOA_LIBRARY}
            ${METAL_LIBRARY}
            ${APPKIT_LIBRARY}
            ${COREDATA_LIBRARY}
            ${FOUNDATION_LIBRARY}
            ${QUARTZCORE_LIBRARY})
    set_property (TARGET ${OUTPUT_NAME_GLFW_METAL} APPEND_STRING PROPERTY
            COMPILE_FLAGS "-fobjc-arc")
endif()

message(STATUS "CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")

find_package(glfw3 REQUIRED)
if (${glfw3_FOUND})
    find_package(OpenGL REQUIRED)
        if (${OpenGL_FOUND})
            find_library(COCOA_LIBRARY Cocoa)
            find_library(METAL_LIBRARY Metal)
            find_library(APPKIT_LIBRARY AppKit)
            find_library(COREDATA_LIBRARY CoreData)
            find_library(FOUNDATION_LIBRARY Foundation)
            find_library(QUARTZCORE_LIBRARY QuartzCore)
            
            set(OUTPUT_NAME_GLFW_OPENGL ${PROJECT_NAME}GlfwOpenGL3)
            add_executable(${OUTPUT_NAME_GLFW_OPENGL} ${IMGUI_SRCS} src/main_glfw_opengl.cpp ${KBLT_SRC})
            target_link_libraries(${OUTPUT_NAME_GLFW_OPENGL} ImGui)
            target_link_libraries(${OUTPUT_NAME_GLFW_OPENGL} ImPlot)
            target_link_libraries(${OUTPUT_NAME_GLFW_OPENGL} Atomic_queue)
            target_link_libraries(${OUTPUT_NAME_GLFW_OPENGL} glfw ${OPENGL_LIBRARY})
            target_link_libraries(${OUTPUT_NAME_GLFW_OPENGL}
                            ${COCOA_LIBRARY}
                            ${METAL_LIBRARY}
                            ${APPKIT_LIBRARY}
                            ${COREDATA_LIBRARY}
                            ${FOUNDATION_LIBRARY}
                            ${QUARTZCORE_LIBRARY})
        endif()
endif()